#include <fstream>
#include <sstream>

#include "database.h"
#include "api/logging/logging.h"

using namespace std::string_literals;

namespace projectfarm::shared::persistence
{
    bool Database::Open(const std::filesystem::path& path, bool openInMemory) noexcept
    {
        if (this->IsOpen())
        {
            api::logging::Log("Database already open: '" + path.u8string() + "'");
            return false;
        }

        auto databasePath = openInMemory ? ":memory:"s : path.u8string();

        auto res = sqlite3_open(databasePath.c_str(), &this->_db);
        if (res != SQLITE_OK)
        {
            auto message = sqlite3_errstr(res);
            api::logging::Log("Failed to open database: '" + path.u8string() +
                "' with error: " + message);
            return false;
        }

        if (openInMemory)
        {
            if (!this->RunSQLFromFile(path))
            {
                api::logging::Log("Failed to run SQL in file: " + path.u8string());
                return false;
            }
        }

        api::logging::Log("Opened database: " + path.u8string());

        return true;
    }

    bool Database::Shutdown() noexcept
    {
        if (!this->IsOpen())
        {
            api::logging::Log("Database already closed.");
            return false;
        }

        for (auto& statement : this->_statements)
        {
            statement->Shutdown();
        }

        this->_statements.clear();

        if (auto res = sqlite3_close(this->_db); res != SQLITE_OK)
        {
            auto message = sqlite3_errstr(res);
            api::logging::Log("Failed to close database with error: "s + message);
            return false;
        }

        this->_db = nullptr;

        api::logging::Log("Closed database.");

        return true;
    }

    bool Database::RunSQLFromFile(const std::filesystem::path& path) noexcept
    {
        if (!this->IsOpen())
        {
            api::logging::Log("Database is not connected.");
            return false;
        }

        std::ifstream fs(path);

        if (!fs.is_open())
        {
            api::logging::Log("Failed to open SQL file: " + path.u8string());
            return false;
        }

        std::stringstream ss;
        ss << fs.rdbuf();

        char* sqlError = nullptr;

        if (auto res = sqlite3_exec(this->_db, ss.str().c_str(), nullptr, nullptr, &sqlError);
            res != SQLITE_OK)
        {
            auto message = sqlite3_errstr(res);
            api::logging::Log("Failed to run SQL from file: "s + path.u8string() +
                " with error: " + message + " and SQL error: " + sqlError);

            if (sqlError)
            {
                sqlite3_free(sqlError);
                sqlError = nullptr;
            }

            return false;
        }

        return true;
    }

    std::shared_ptr<Statement> Database::CreateStatementFromFile(const std::filesystem::path& path) noexcept
    {
        auto statement = std::make_shared<Statement>(this->weak_from_this());

        if (!statement->PrepareFromFile(path))
        {
            api::logging::Log("Failed to create statement from SQL file: " + path.u8string());
            return nullptr;
        }

        this->_statements.push_back(statement);

        return statement;
    }
}