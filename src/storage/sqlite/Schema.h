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

inline constexpr char CREATE_TRANSACTIONS_TABLE[] = R"sql(
CREATE TABLE IF NOT EXISTS transactions (
    id                  INTEGER PRIMARY KEY,
    cell_id             INTEGER NOT NULL,
    created_by_user_id  INTEGER NOT NULL,
    type                TEXT NOT NULL
                        CHECK(type IN ('INCOME', 'EXPENSE')),
    description         TEXT NOT NULL
                        CHECK(length(description) BETWEEN 1 AND 200),
    amount_minor        INTEGER NOT NULL CHECK(amount_minor > 0),
    occurred_at         TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    created_at          TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at          TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (cell_id) REFERENCES cells(id) ON DELETE CASCADE,
    FOREIGN KEY (created_by_user_id) REFERENCES users(id)
);
)sql";

inline constexpr char CREATE_TRANSACTIONS_CELL_INDEX[] = R"sql(
CREATE INDEX IF NOT EXISTS idx_transactions_cell_date
ON transactions(cell_id, occurred_at, id);
)sql";

inline constexpr char ADD_TRANSACTION_CATEGORY[] = R"sql(
ALTER TABLE transactions
ADD COLUMN category TEXT NOT NULL DEFAULT 'General'
CHECK(length(category) BETWEEN 1 AND 50);
)sql";

inline constexpr char CREATE_CATEGORIES_TABLE[] = R"sql(
CREATE TABLE categories (
    id          INTEGER PRIMARY KEY,
    cell_id     INTEGER NOT NULL,
    name        TEXT NOT NULL COLLATE NOCASE
                CHECK(length(name) BETWEEN 1 AND 50 AND name = trim(name)),
    created_at  TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    UNIQUE (cell_id, name),
    UNIQUE (cell_id, id),
    FOREIGN KEY (cell_id) REFERENCES cells(id) ON DELETE CASCADE
);
)sql";

inline constexpr char BACKFILL_DEFAULT_CATEGORIES[] = R"sql(
INSERT INTO categories (cell_id, name)
SELECT id, 'General' FROM cells;
)sql";

inline constexpr char BACKFILL_TRANSACTION_CATEGORIES[] = R"sql(
INSERT OR IGNORE INTO categories (cell_id, name)
SELECT cell_id, MIN(normalized_name)
FROM (
    SELECT cell_id,
           CASE WHEN length(trim(category)) BETWEEN 1 AND 50
                THEN trim(category) ELSE 'General' END AS normalized_name
    FROM transactions
)
GROUP BY cell_id, lower(normalized_name);
)sql";

inline constexpr char CREATE_TRANSACTIONS_V5_TABLE[] = R"sql(
CREATE TABLE transactions_v5 (
    id                  INTEGER PRIMARY KEY,
    cell_id             INTEGER NOT NULL,
    created_by_user_id  INTEGER NOT NULL,
    type                TEXT NOT NULL
                        CHECK(type IN ('INCOME', 'EXPENSE')),
    description         TEXT NOT NULL
                        CHECK(length(description) BETWEEN 1 AND 200),
    amount_minor        INTEGER NOT NULL CHECK(amount_minor > 0),
    occurred_at         TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    created_at          TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at          TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    category_id         INTEGER NOT NULL,
    FOREIGN KEY (cell_id) REFERENCES cells(id) ON DELETE CASCADE,
    FOREIGN KEY (created_by_user_id) REFERENCES users(id),
    FOREIGN KEY (cell_id, category_id) REFERENCES categories(cell_id, id)
);
)sql";

inline constexpr char COPY_TRANSACTIONS_TO_V5[] = R"sql(
INSERT INTO transactions_v5 (
    id, cell_id, created_by_user_id, type, description, amount_minor,
    occurred_at, created_at, updated_at, category_id)
SELECT t.id, t.cell_id, t.created_by_user_id, t.type, t.description,
       t.amount_minor, t.occurred_at, t.created_at, t.updated_at,
       c.id
FROM transactions t
JOIN categories c
  ON c.cell_id = t.cell_id
 AND c.name = CASE WHEN length(trim(t.category)) BETWEEN 1 AND 50
                   THEN trim(t.category) ELSE 'General' END;
)sql";

inline constexpr char DROP_LEGACY_TRANSACTIONS[] = "DROP TABLE transactions;";
inline constexpr char RENAME_TRANSACTIONS_V5[] =
    "ALTER TABLE transactions_v5 RENAME TO transactions;";

inline constexpr char CREATE_DEFAULT_CATEGORY_TRIGGER[] = R"sql(
CREATE TRIGGER add_default_cell_category
AFTER INSERT ON cells
BEGIN
    INSERT INTO categories (cell_id, name) VALUES (NEW.id, 'General');
END;
)sql";

inline constexpr char CREATE_TRANSACTION_CREATOR_MEMBERSHIP_TRIGGER[] = R"sql(
CREATE TRIGGER require_transaction_creator_membership
BEFORE INSERT ON transactions
WHEN NOT EXISTS (
    SELECT 1 FROM cell_members
    WHERE cell_id = NEW.cell_id AND user_id = NEW.created_by_user_id
)
BEGIN
    SELECT RAISE(ABORT, 'transaction creator must be a cell member');
END;
)sql";

inline constexpr char ADD_CATEGORY_MONTHLY_BUDGET[] = R"sql(
ALTER TABLE categories
ADD COLUMN budget_minor INTEGER NOT NULL DEFAULT 0
CHECK(budget_minor >= 0);
)sql";
}
