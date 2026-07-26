#include <stdexcept>
#include <string>

#include "application/CellService.h"
#include "application/TransactionService.h"
#include "storage/sqlite/SQLiteCellRepository.h"
#include "storage/sqlite/SQLiteDatabase.h"
#include "storage/sqlite/SQLiteMigrations.h"
#include "storage/sqlite/SQLiteTransactionRepository.h"
#include "storage/sqlite/SQLiteUserRepository.h"

namespace
{
void require(bool condition, const std::string& message)
{
    if (!condition) throw std::runtime_error(message);
}
}

int main()
{
    SQLiteDatabase database(":memory:");
    SQLiteMigrations::apply(database);
    SQLiteUserRepository users(database);
    SQLiteCellRepository cells(database);
    SQLiteTransactionRepository transactions(database);
    CellService cellService(cells, users);
    TransactionService transactionService(transactions, cells);

    require(users.insertUser("owner", "Cell Owner", "secret1"), "create owner");
    require(users.insertUser("member", "Cell Member", "secret2"), "create member");
    require(users.insertUser("guest", "Cell Guest", "secret3"), "create guest");
    const auto owner = users.findUserByUsername("owner");
    const auto member = users.findUserByUsername("member");
    const auto guest = users.findUserByUsername("guest");
    require(owner && member && guest, "load users");

    require(cellService.createCell(" Family Budget ", owner->getUserId(), " Shared expenses "),
            "create cell");
    const auto ownedCells = cellService.getCellsForUser(owner->getUserId());
    require(ownedCells.size() == 1, "owner can see new cell");
    const uint64_t cellId = ownedCells.front().getCellId();
    require(cells.findMember(cellId, owner->getUserId())->role == CellRole::OWNER,
            "owner membership created");

    require(cellService.addMemberToCell(
                owner->getUserId(), cellId, member->getUserId(), CellRole::MEMBER),
            "owner adds member");
    require(cellService.addMemberToCell(
                owner->getUserId(), cellId, guest->getUserId(), CellRole::GUEST),
            "owner adds guest");
    require(!cellService.addMemberToCell(
                member->getUserId(), cellId, guest->getUserId(), CellRole::MEMBER),
            "member cannot manage membership");
    require(cellService.getCellsForUser(member->getUserId()).size() == 1,
            "member can see joined cell");

    const auto income = transactionService.addTransaction(
        owner->getUserId(), cellId, TransactionType::INCOME, "Salary", 10000);
    const auto expense = transactionService.addTransaction(
        member->getUserId(), cellId, TransactionType::EXPENSE, "Food", 2500);
    require(income && expense, "owner and member add transactions");
    require(!transactionService.addTransaction(
                guest->getUserId(), cellId, TransactionType::EXPENSE, "Denied", 100),
            "guest cannot add transaction");
    require(!transactionService.editTransaction(
                member->getUserId(), income->getTransactionId(),
                TransactionType::INCOME, "Changed", 50000),
            "member cannot edit owner's transaction");
    require(transactionService.editTransaction(
                owner->getUserId(), expense->getTransactionId(),
                TransactionType::EXPENSE, "Groceries", 2000),
            "owner edits any transaction");
    require(transactionService.getCellBalance(owner->getUserId(), cellId) == 8000,
            "balance reflects transaction edit");

    require(!cellService.updateCell(
                member->getUserId(), cellId, "Denied", "Denied"),
            "member cannot edit cell");
    require(cellService.updateCell(
                owner->getUserId(), cellId, "Updated Budget", "Updated description"),
            "owner edits cell");
    require(!cellService.removeMemberFromCell(
                owner->getUserId(), cellId, owner->getUserId()),
            "owner cannot be removed");
    require(cellService.removeMemberFromCell(
                owner->getUserId(), cellId, guest->getUserId()),
            "owner removes guest");

    require(cellService.deleteCell(owner->getUserId(), cellId), "owner deletes cell");
    require(!cells.findCellById(cellId), "cell deleted");
    require(!transactions.findTransactionById(income->getTransactionId()),
            "cell deletion cascades transactions");
}
