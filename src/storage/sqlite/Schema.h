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

inline constexpr char CREATE_CELL_MEMBERS_TABLE[] = R"sql(
CREATE TABLE IF NOT EXISTS cell_members (
    cell_id       INTEGER NOT NULL,
    user_id       INTEGER NOT NULL,
    role          TEXT NOT NULL
                  CHECK(role IN ('OWNER', 'MEMBER', 'GUEST')),
    joined_at     TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (cell_id, user_id),
    FOREIGN KEY (cell_id) REFERENCES cells(id) ON DELETE CASCADE,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
);
)sql";

inline constexpr char CREATE_CELL_MEMBERS_USER_INDEX[] = R"sql(
CREATE INDEX IF NOT EXISTS idx_cell_members_user_id
ON cell_members(user_id);
)sql";

inline constexpr char CREATE_CELL_OWNER_MEMBERSHIP_TRIGGER[] = R"sql(
CREATE TRIGGER IF NOT EXISTS add_cell_owner_membership
AFTER INSERT ON cells
BEGIN
    INSERT INTO cell_members (cell_id, user_id, role)
    VALUES (NEW.id, NEW.owner_user_id, 'OWNER');
END;
)sql";

inline constexpr char BACKFILL_CELL_OWNER_MEMBERSHIPS[] = R"sql(
INSERT OR IGNORE INTO cell_members (cell_id, user_id, role)
SELECT id, owner_user_id, 'OWNER'
FROM cells;
)sql";
}
