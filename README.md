# FinanCell

FinanCell is a C++17 console application for managing shared financial cells. It
stores data in SQLite, hashes passwords with Argon2id through libsodium, and uses
integer minor units for exact monetary calculations.

## MVP features

- Account registration and login with normalized usernames and hashed passwords.
- Financial-cell creation, listing, selection, editing, and deletion.
- Owner-managed membership with `OWNER`, `MEMBER`, and read-only `GUEST` roles.
- Managed, reusable categories scoped to each financial cell.
- Income and expense CRUD with descriptions, exact amounts, calendar dates, and
  category references.
- Authorized transaction listing and date-range filtering.
- Cell balances plus monthly income, expense, net, and category summaries.
- Versioned SQLite migrations and cascading cleanup when a cell is deleted.
- Console validation, password hiding, and confirmations for destructive actions.
- File-backed persistence, migration, reporting, and console-flow tests.

## Roles

| Capability | Owner | Member | Guest |
| --- | :---: | :---: | :---: |
| View cells, members, transactions, and reports | Yes | Yes | Yes |
| View categories | Yes | Yes | Yes |
| Create categories | Yes | Yes | No |
| Add transactions | Yes | Yes | No |
| Edit or delete own transactions | Yes | Yes | No |
| Edit or delete any transaction in the cell | Yes | No | No |
| Manage members or cell details | Yes | No | No |

## Dependencies

- CMake 3.16 or newer
- A C++17 compiler
- SQLite3 development files
- libsodium development files
- Doxygen (optional, for API documentation)

On Debian or Ubuntu, the required packages can be installed with:

```sh
sudo apt install build-essential cmake libsqlite3-dev libsodium-dev doxygen
```

## Build and run

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/FinanCell
```

The application creates its database at `data/financell.db`.

Passwords are stored only as libsodium Argon2 hashes. Databases from early
development builds that stored plaintext passwords require account recreation;
plaintext fallback authentication is intentionally not supported.

## Test

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

## Generate documentation

When Doxygen is installed, CMake exposes a `docs` target:

```sh
cmake --build build --target docs
```

Open `docs/html/index.html` after generation. You can also run `doxygen Doxyfile`
directly from the repository root.
