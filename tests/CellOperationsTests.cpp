#include <stdexcept>
#include <string>

#include "application/CellService.h"
#include "application/TransactionService.h"
#include "application/UserService.h"
#include "security/PasswordHasher.h"
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
    SodiumPasswordHasher passwordHasher;
    UserService userService(users, passwordHasher);

    require(users.insertUser("owner", "Cell Owner", "secret1"), "create owner");
    require(users.insertUser("member", "Cell Member", "secret2"), "create member");
    require(users.insertUser("guest", "Cell Guest", "secret3"), "create guest");
    const auto owner = users.findUserByUsername("owner");
    const auto member = users.findUserByUsername("member");
    const auto guest = users.findUserByUsername("guest");
    require(owner && member && guest, "load users");
    require(userService.authenticateUser("owner", "secret1").has_value(),
            "legacy plaintext login succeeds once");
    require(passwordHasher.isEncodedHash(
                users.findUserByUsername("owner")->getPasswordHash()),
            "legacy password upgraded to Argon2id");
    require(!userService.authenticateUser("owner", "wrong-password"),
            "invalid password rejected");
    std::string secureUsername = "secure";
    std::string secureName = "Secure User";
    std::string securePassword = "secret4";
    require(userService.createUser(secureUsername, secureName, securePassword),
            "hashed account creation");
    require(passwordHasher.isEncodedHash(
                users.findUserByUsername("secure")->getPasswordHash()),
            "new account stores Argon2id hash");

    require(cellService.createCell(" Family Budget ", owner->getUserId(), " Shared expenses "),
            "create cell");
    const auto ownedCells = cellService.getCellsForUser(owner->getUserId());
    require(ownedCells.size() == 1, "owner can see new cell");
    const uint64_t cellId = ownedCells.front().getCellId();
    require(cells.findMember(cellId, owner->getUserId())->role == CellRole::OWNER,
            "owner membership created");

    require(cellService.addMemberToCell(
                owner->getUserId(), cellId, member->getUserId(), CellRole::MEMBER) == CellOperationResult::SUCCESS,
            "owner adds member");
    require(cellService.addMemberToCell(
                owner->getUserId(), cellId, guest->getUserId(), CellRole::GUEST) == CellOperationResult::SUCCESS,
            "owner adds guest");
    require(cellService.addMemberToCell(
                member->getUserId(), cellId, guest->getUserId(), CellRole::MEMBER) == CellOperationResult::NOT_AUTHORIZED,
            "member cannot manage membership");
    require(cellService.getCellsForUser(member->getUserId()).size() == 1,
            "member can see joined cell");

    const auto income = transactionService.addTransaction(
        owner->getUserId(), cellId, TransactionType::INCOME, "Salary", 10000,
        "2026-07-01", "Salary");
    const auto expense = transactionService.addTransaction(
        member->getUserId(), cellId, TransactionType::EXPENSE, "Food", 2500);
    require(income && expense, "owner and member add transactions");
    require(income->getCategory() == "Salary", "transaction category persisted");
    require(transactionService.getTransactionsForCell(
                owner->getUserId(), cellId, "2026-07-01", "2026-07-01")->size() == 1,
            "transactions filter by date");
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

    require(cellService.updateCell(
                member->getUserId(), cellId, "Denied", "Denied") == CellOperationResult::NOT_AUTHORIZED,
            "member cannot edit cell");
    require(cellService.updateCell(
                owner->getUserId(), cellId, "Updated Budget", "Updated description") == CellOperationResult::SUCCESS,
            "owner edits cell");
    require(cellService.removeMemberFromCell(
                owner->getUserId(), cellId, owner->getUserId()) == CellOperationResult::CANNOT_MODIFY_OWNER,
            "owner cannot be removed");
    require(cellService.removeMemberFromCell(
                owner->getUserId(), cellId, guest->getUserId()) == CellOperationResult::SUCCESS,
            "owner removes guest");

    require(cellService.deleteCell(owner->getUserId(), cellId) == CellOperationResult::SUCCESS, "owner deletes cell");
    require(!cells.findCellById(cellId), "cell deleted");
    require(!transactions.findTransactionById(income->getTransactionId()),
            "cell deletion cascades transactions");
}
