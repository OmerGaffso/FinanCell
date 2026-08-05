#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include <QCoreApplication>

#include "ui/qt/GuiDatabasePath.h"

namespace
{
/** @brief Stops the path test when an expected filesystem behavior fails. */
void require(bool condition, const std::string& message)
{
    if (condition) return;
    std::cerr << "FAILED: " << message << '\n';
    std::exit(1);
}

/** @brief Owns an isolated working and application-data directory. */
class TemporaryDirectory final
{
public:
    /** @brief Creates a unique path under the system temporary directory. */
    TemporaryDirectory()
    {
        const auto suffix = std::chrono::steady_clock::now().time_since_epoch().count();
        m_path = std::filesystem::temp_directory_path() /
                 ("financell-gui-path-test-" + std::to_string(suffix));
        std::filesystem::create_directories(m_path);
    }

    /** @brief Removes the isolated test data. */
    ~TemporaryDirectory()
    {
        std::error_code error;
        std::filesystem::remove_all(m_path, error);
    }

    /** @brief Returns the isolated directory. */
    const std::filesystem::path& path() const { return m_path; }

private:
    std::filesystem::path m_path;
};

/** @brief Restores the process working directory after a legacy-copy test. */
class WorkingDirectoryGuard final
{
public:
    /** @brief Stores the current directory and changes to the supplied path. */
    explicit WorkingDirectoryGuard(const std::filesystem::path& path)
        : m_original(std::filesystem::current_path())
    {
        std::filesystem::current_path(path);
    }

    /** @brief Restores the original working directory. */
    ~WorkingDirectoryGuard() { std::filesystem::current_path(m_original); }

private:
    std::filesystem::path m_original;
};
}

/** @brief Verifies override resolution and non-destructive legacy migration. */
int main(int argc, char* argv[])
{
    TemporaryDirectory directory;
    const std::filesystem::path dataHome = directory.path() / "application-data";
    qputenv("XDG_DATA_HOME", QByteArray::fromStdString(dataHome.string()));
    QCoreApplication application(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("FinanCell"));
    QCoreApplication::setApplicationName(QStringLiteral("FinanCell"));

    const std::filesystem::path overridePath =
        directory.path() / "override" / "custom.db";
    qputenv("FINANCELL_DB_PATH", QByteArray::fromStdString(overridePath.string()));
    require(GuiDatabasePath::resolve() == overridePath,
            "environment override selects the exact absolute path");
    require(std::filesystem::is_directory(overridePath.parent_path()),
            "override parent directory is created");

    qunsetenv("FINANCELL_DB_PATH");
    const std::filesystem::path workingPath = directory.path() / "working";
    std::filesystem::create_directories(workingPath / "data");
    const std::filesystem::path legacyPath = workingPath / "data" / "financell.db";
    {
        std::ofstream legacyDatabase(legacyPath);
        legacyDatabase << "legacy database marker";
    }

    WorkingDirectoryGuard workingDirectory(workingPath);
    const std::filesystem::path resolvedPath = GuiDatabasePath::resolve();
    require(resolvedPath.parent_path() == dataHome / "FinanCell" / "FinanCell",
            "GUI database uses the platform application-data directory (resolved " +
                resolvedPath.string() + ")");
    require(std::filesystem::exists(resolvedPath), "legacy database is copied");
    require(std::filesystem::exists(legacyPath), "legacy database remains untouched");
    require(std::filesystem::file_size(resolvedPath) ==
                std::filesystem::file_size(legacyPath),
            "legacy database copy retains its contents");

    std::ofstream existingDatabase(resolvedPath, std::ios::trunc);
    existingDatabase << "existing destination";
    existingDatabase.close();
    require(GuiDatabasePath::resolve() == resolvedPath,
            "existing production database remains selected");
    require(std::filesystem::file_size(resolvedPath) == 20,
            "existing production database is not overwritten by the legacy copy");
    return 0;
}
