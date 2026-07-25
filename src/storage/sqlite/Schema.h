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

inline constexpr char CREATE_CELLS_TABLE[] = R"sql(
CREATE TABLE IF NOT EXISTS cells (
    id             INTEGER PRIMARY KEY,
    name           TEXT NOT NULL
                   CHECK(length(name) BETWEEN 3 AND 50),
    description    TEXT NOT NULL DEFAULT ''
                   CHECK(length(description) <= 200),
    currency       TEXT NOT NULL
                   CHECK(length(currency) = 3),
    owner_user_id  INTEGER NOT NULL,
    created_at     TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (owner_user_id) REFERENCES users(id)
);
)sql";
}
