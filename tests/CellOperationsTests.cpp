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
    require(userService.authenticateUser("SECURE", "secret4").has_value(),
            "new hashed account authenticates with normalized username");
    require(!userService.createUser(secureUsername, secureName, securePassword),
            "duplicate username rejected");

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
    require(cellService.addMemberToCell(
                owner->getUserId(), cellId, member->getUserId(), CellRole::MEMBER) == CellOperationResult::ALREADY_MEMBER,
            "duplicate membership rejected");
    require(cellService.addMemberToCell(
                owner->getUserId(), cellId, 9999, CellRole::MEMBER) == CellOperationResult::USER_NOT_FOUND,
            "unknown member rejected");
    require(cellService.addMemberToCell(
                owner->getUserId(), cellId, users.findUserByUsername("secure")->getUserId(), CellRole::OWNER) == CellOperationResult::INVALID_ROLE,
            "second owner role rejected");
    require(cellService.getCellsForUser(member->getUserId()).size() == 1,
            "member can see joined cell");
    require(cellService.updateMemberRole(
                owner->getUserId(), cellId, guest->getUserId(), CellRole::MEMBER) == CellOperationResult::SUCCESS,
            "owner changes member role");
    require(cells.findMember(cellId, guest->getUserId())->role == CellRole::MEMBER,
            "changed role persisted");
    require(cellService.updateMemberRole(
                owner->getUserId(), cellId, guest->getUserId(), CellRole::GUEST) == CellOperationResult::SUCCESS,
            "owner restores guest role");

    const auto income = transactionService.addTransaction(
        owner->getUserId(), cellId, TransactionType::INCOME, "Salary", 10000,
        "2026-07-01", "Salary");
    const auto expense = transactionService.addTransaction(
        member->getUserId(), cellId, TransactionType::EXPENSE, "Food", 2500);
    require(income && expense, "owner and member add transactions");
    require(income->getCategory() == "Salary", "transaction category persisted");
    require(!transactionService.addTransaction(
                owner->getUserId(), cellId, TransactionType::EXPENSE, "Bad date", 100,
                "2026-02-29", "General"),
            "invalid calendar date rejected");
    require(TransactionService::isDateValid("2024-02-29"),
            "valid leap date accepted");
    require(!TransactionService::isDateValid("2024-00-01") &&
                !TransactionService::isDateValid("2024-01-00") &&
                !TransactionService::isDateValid("2024-aa-01"),
            "malformed calendar dates rejected");
    require(!transactionService.addTransaction(
                owner->getUserId(), cellId, TransactionType::EXPENSE, "Bad category", 100,
                "2026-07-01", ""),
            "empty category rejected");
    require(transactionService.getTransactionsForCell(
                owner->getUserId(), cellId, "2026-07-01", "2026-07-01")->size() == 1,
            "transactions filter by date");
    require(!transactionService.getTransactionsForCell(
                owner->getUserId(), cellId, "2026-07-01", ""),
            "incomplete date range rejected");
    require(!transactionService.getTransactionsForCell(
                owner->getUserId(), cellId, "2026-08-01", "2026-07-01"),
            "reversed date range rejected");
    require(!transactionService.getTransactionsForCell(
                users.findUserByUsername("secure")->getUserId(), cellId),
            "outsider cannot read transactions");
    require(!transactionService.addTransaction(
                guest->getUserId(), cellId, TransactionType::EXPENSE, "Denied", 100),
            "guest cannot add transaction");
    require(!transactionService.editTransaction(
                member->getUserId(), cellId, income->getTransactionId(),
                TransactionType::INCOME, "Changed", 50000, "", "Salary"),
            "member cannot edit owner's transaction");
    require(transactionService.editTransaction(
                owner->getUserId(), cellId, expense->getTransactionId(),
                TransactionType::EXPENSE, "Groceries", 2000, "2026-07-02", "Food"),
            "owner edits any transaction");
    require(transactionService.editTransaction(
                owner->getUserId(), cellId, income->getTransactionId(),
                TransactionType::INCOME, "Salary revised", 10000, "", ""),
            "blank edit fields retain date and category");
    require(transactions.findTransactionById(income->getTransactionId())->getCategory() == "Salary" &&
                transactions.findTransactionById(income->getTransactionId())->getOccurredAt() == "2026-07-01",
            "retained transaction fields remain unchanged");
    require(transactions.findTransactionById(expense->getTransactionId())->getCategory() == "Food",
            "transaction edit persists category");
    require(transactions.findTransactionById(expense->getTransactionId())->getOccurredAt() == "2026-07-02",
            "transaction edit persists date");
    require(transactionService.getCellBalance(owner->getUserId(), cellId) == 8000,
            "balance reflects transaction edit");

    require(cellService.createCell("Travel Budget", owner->getUserId(), "Trips"),
            "create second cell");
    const uint64_t secondCellId = cellService.getCellsForUser(owner->getUserId()).back().getCellId();
    const auto secondCellTransaction = transactionService.addTransaction(
        owner->getUserId(), secondCellId, TransactionType::EXPENSE, "Flight", 5000,
        "2026-08-01", "Travel");
    require(secondCellTransaction.has_value(), "create transaction in second cell");
    require(!transactionService.editTransaction(
                owner->getUserId(), cellId, secondCellTransaction->getTransactionId(),
                TransactionType::EXPENSE, "Wrong cell", 1, "", "Travel"),
            "transaction edit is scoped to selected cell");
    require(!transactionService.deleteTransaction(
                owner->getUserId(), cellId, secondCellTransaction->getTransactionId()),
            "transaction delete is scoped to selected cell");
    require(!transactionService.deleteTransaction(
                member->getUserId(), cellId, income->getTransactionId()),
            "member cannot delete another user's transaction");
    require(!transactionService.deleteTransaction(
                guest->getUserId(), cellId, expense->getTransactionId()),
            "guest cannot delete transactions");
    require(transactionService.deleteTransaction(
                member->getUserId(), cellId, expense->getTransactionId()),
            "member deletes own transaction");
    require(!transactions.findTransactionById(expense->getTransactionId()),
            "deleted transaction removed");

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
