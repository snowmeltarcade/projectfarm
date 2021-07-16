#ifndef PROJECTFARM_DATABASE_H
#define PROJECTFARM_DATABASE_H

#include <filesystem>
#include <memory>
#include <vector>
#include <sqlite3.h>

#include "statement.h"

namespace projectfarm::shared::persistence
{
    class Database final : public std::enable_shared_from_this<Database>
    {
    public:
        Database() = default;
        ~Database()
        {
            if (!this->Shutdown()){}
        }

        [[nodiscard]]
        bool Open(const std::filesystem::path& path, bool openInMemory = false) noexcept;

        [[nodiscard]]
        bool IsOpen() const noexcept
        {
            return this->_db != nullptr;
        }

        [[nodiscard]]
        bool Shutdown() noexcept;

        [[nodiscard]]
        bool RunSQLFromFile(const std::filesystem::path& path) noexcept;

        [[nodiscard]]
        std::shared_ptr<Statement> CreateStatementFromFile(const std::filesystem::path& path) noexcept;

        [[nodiscard]]
        sqlite3* GetConnection() const noexcept
        {
            return this->_db;
        }

    private:
        sqlite3* _db = nullptr;

        std::vector<std::shared_ptr<Statement>> _statements;
    };
}

#endif
