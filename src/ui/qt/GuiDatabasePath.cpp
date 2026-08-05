#include "ui/qt/GuiDatabasePath.h"

#include <QByteArray>
#include <QStandardPaths>
#include <QString>

#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace
{
constexpr const char* DATABASE_FILE_NAME = "financell.db";

void prepareParentDirectory(const std::filesystem::path& databasePath)
{
    const std::filesystem::path parent = databasePath.parent_path();
    if (!parent.empty()) std::filesystem::create_directories(parent);
}
}

std::filesystem::path GuiDatabasePath::resolve()
{
    const QByteArray overridePath = qgetenv("FINANCELL_DB_PATH");
    if (!overridePath.isEmpty())
    {
        std::filesystem::path databasePath(overridePath.constData());
        if (databasePath.is_relative()) databasePath = std::filesystem::absolute(databasePath);
        prepareParentDirectory(databasePath);
        return databasePath.lexically_normal();
    }

    const QString writableDirectory =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (writableDirectory.isEmpty())
        throw std::runtime_error("Qt could not determine a writable application-data directory.");

    const std::filesystem::path databasePath =
        std::filesystem::path(writableDirectory.toStdString()) / DATABASE_FILE_NAME;
    prepareParentDirectory(databasePath);

    const std::filesystem::path legacyPath =
        std::filesystem::current_path() / "data" / DATABASE_FILE_NAME;
    if (!std::filesystem::exists(databasePath) && std::filesystem::exists(legacyPath))
    {
        std::error_code copyError;
        const bool copied = std::filesystem::copy_file(
            legacyPath, databasePath, std::filesystem::copy_options::none, copyError);
        if (!copied || copyError)
            throw std::runtime_error(
                "The legacy FinanCell database could not be copied to the application-data directory: " +
                copyError.message());
    }

    return databasePath.lexically_normal();
}
