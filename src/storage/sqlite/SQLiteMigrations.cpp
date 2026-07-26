#include "storage/sqlite/SQLiteMigrations.h"

#include <stdexcept>
#include <string>

#include "storage/sqlite/SQLiteDatabase.h"
#include "storage/sqlite/SQLiteStatement.h"
#include "storage/sqlite/Schema.h"

namespace
{
unsigned int currentVersion(SQLiteDatabase& database)
{
    SQLiteStatement statement(database, "PRAGMA user_version;");
    if (!statement.next())
    {
        throw std::runtime_error("SQLite did not return a schema version.");
    }

    return static_cast<unsigned int>(statement.columnUInt64(0));
}

void setVersion(SQLiteDatabase& database, unsigned int version)
{
    database.execute("PRAGMA user_version = " + std::to_string(version) + ";");
}

template <typename Migration>
void applyMigration(
    SQLiteDatabase& database,
    unsigned int targetVersion,
    Migration migration)
{
    // Keep each schema change and its user_version marker atomic.
    database.execute("BEGIN IMMEDIATE TRANSACTION;");
    try
    {
        migration();
        setVersion(database, targetVersion);
        database.execute("COMMIT;");
    }
    catch (...)
    {
        try
        {
            database.execute("ROLLBACK;");
        }
        catch (...)
        {
        }
        throw;
    }
}
}

void SQLiteMigrations::apply(SQLiteDatabase& database)
{
    unsigned int version = currentVersion(database);
    if (version > LATEST_SCHEMA_VERSION)
    {
        throw std::runtime_error(
            "Database schema version " + std::to_string(version) +
            " is newer than this application supports.");
    }

    if (version < 1)
    {
        applyMigration(
            database,
            1,
            [&database]
            {
                database.execute(Schema::CREATE_USERS_TABLE);
                database.execute(Schema::CREATE_CELLS_TABLE);
            });
        version = 1;
    }

    if (version < 2)
    {
        applyMigration(
            database,
            2,
            [&database]
            {
                database.execute(Schema::CREATE_CELL_MEMBERS_TABLE);
                database.execute(Schema::CREATE_CELL_MEMBERS_USER_INDEX);
                database.execute(Schema::CREATE_CELL_OWNER_MEMBERSHIP_TRIGGER);
                database.execute(Schema::BACKFILL_CELL_OWNER_MEMBERSHIPS);
            });
        version = 2;
    }

    if (version < 3)
    {
        applyMigration(
            database,
            3,
            [&database]
            {
                database.execute(Schema::CREATE_TRANSACTIONS_TABLE);
                database.execute(Schema::CREATE_TRANSACTIONS_CELL_INDEX);
            });
        version = 3;
    }

    if (version < 4)
    {
        applyMigration(database, 4, [&database]
        {
            database.execute(Schema::ADD_TRANSACTION_CATEGORY);
        });
    }
}
