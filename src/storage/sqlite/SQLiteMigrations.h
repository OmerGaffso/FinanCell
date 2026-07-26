#pragma once

class SQLiteDatabase;

namespace SQLiteMigrations
{
inline constexpr unsigned int LATEST_SCHEMA_VERSION = 2;

void apply(SQLiteDatabase& database);
}
