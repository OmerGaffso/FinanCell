#include <cstdlib>
#include <iostream>
#include <string>

#include <QVariantMap>

#include "application/CellService.h"
#include "storage/sqlite/SQLiteCellRepository.h"
#include "storage/sqlite/SQLiteDatabase.h"
#include "storage/sqlite/SQLiteMigrations.h"
#include "storage/sqlite/SQLiteUserRepository.h"
#include "ui/qt/controllers/CellController.h"
#include "ui/qt/session/SessionState.h"

namespace
{
/** @brief Stops the test executable when a condition fails. @param condition Expected condition. @param message Failure description. */
void require(bool condition, const std::string& message)
{
    if (condition) return;
    std::cerr << "FAILED: " << message << '\n';
    std::exit(1);
}
}

/** @brief Exercises Qt cell-controller flows against an in-memory SQLite database. */
int main()
{
    SQLiteDatabase database(":memory:");
    SQLiteMigrations::apply(database);
    SQLiteUserRepository userRepository(database);
    SQLiteCellRepository cellRepository(database);
    CellService cellService(cellRepository, userRepository);

    require(
        userRepository.insertUser("owner", "Cell Owner", "unused-test-hash"),
        "insert owner fixture");
    require(
        userRepository.insertUser("other", "Other User", "unused-test-hash"),
        "insert second-user fixture");
    const auto owner = userRepository.findUserByUsername("owner");
    const auto other = userRepository.findUserByUsername("other");
    require(owner && other, "load user fixtures");

    SessionState session;
    CellController controller(cellService, session);
    require(!controller.loadCells(), "logged-out users cannot load cells");

    session.setUser(
        owner->getUserId(),
        QStringLiteral("owner"),
        QStringLiteral("Cell Owner"));
    require(controller.loadCells() && controller.cells().isEmpty(),
            "new owner starts with an empty cell list");
    require(!controller.createCell(QStringLiteral("x"), QString()),
            "invalid cell name is rejected");
    require(controller.createCell(
                QStringLiteral("Home Budget"),
                QStringLiteral("Shared household costs")),
            "valid financial cell is created");
    require(controller.cells().size() == 1,
            "created financial cell appears in the controller list");

    const QVariantMap createdCell = controller.cells().front().toMap();
    const qulonglong cellId = createdCell.value(QStringLiteral("cellId")).toULongLong();
    require(cellId != 0 && controller.selectCell(cellId),
            "owner can select the created financial cell");
    require(controller.hasSelectedCell() &&
                controller.selectedCell().value(QStringLiteral("name")).toString() ==
                    QStringLiteral("Home Budget"),
            "selected cell exposes its public details");

    session.setUser(
        other->getUserId(),
        QStringLiteral("other"),
        QStringLiteral("Other User"));
    require(controller.cells().isEmpty() && !controller.hasSelectedCell(),
            "identity changes clear cached cells and selection");
    require(controller.loadCells() && controller.cells().isEmpty(),
            "unrelated user cannot list the owner's cell");
    require(!controller.selectCell(cellId),
            "unrelated user cannot select the owner's cell");

    std::cout << "Qt cell controller tests passed.\n";
    return 0;
}
