#pragma once

class SQLiteDatabase;

namespace SQLiteMigrations
{
inline constexpr unsigned int LATEST_SCHEMA_VERSION = 4;

/** @brief Applies pending migrations. @param database Database to migrate. */
void apply(SQLiteDatabase& database);
}
