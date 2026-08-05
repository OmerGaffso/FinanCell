#pragma once

#include <filesystem>

namespace GuiDatabasePath
{
/**
 * @brief Resolves and prepares the writable SQLite path for the desktop GUI.
 * @return Absolute database path in the platform application-data directory,
 *         or the path selected by FINANCELL_DB_PATH.
 * @throws std::runtime_error When the directory or legacy database copy cannot be created.
 */
std::filesystem::path resolve();
}
