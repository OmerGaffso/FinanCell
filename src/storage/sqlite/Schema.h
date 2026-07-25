#pragma once

namespace Schema
{
inline constexpr char CREATE_USERS_TABLE[] = R"sql(
CREATE TABLE IF NOT EXISTS users (
    id            INTEGER PRIMARY KEY,
    username      TEXT NOT NULL COLLATE NOCASE UNIQUE
                  CHECK(length(username) BETWEEN 3 AND 30),
    display_name  TEXT NOT NULL
                  CHECK(length(display_name) BETWEEN 3 AND 50),
    password_hash TEXT NOT NULL,
    created_at    TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);
)sql";
}
