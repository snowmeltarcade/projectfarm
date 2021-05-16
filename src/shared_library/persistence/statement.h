#ifndef PROJECTFARM_STATEMENT_H
#define PROJECTFARM_STATEMENT_H

#include <filesystem>
#include <vector>
#include <string>
#include <sqlite3.h>

#include "logging/consume_logger.h"

namespace projectfarm::shared::persistence
{
    class Database;

    class Statement final : public ConsumeLogger
    {
    public:
        explicit Statement(std::weak_ptr<Database> database)
            : _database(std::move(database))
        {}
        ~Statement() override
        {
            this->Shutdown();
        }

        void Shutdown() noexcept;

        [[nodiscard]] bool PrepareFromFile(const std::filesystem::path& path) noexcept;

        [[nodiscard]] bool SetParameterInt(uint8_t index, uint64_t parameter) noexcept;
        [[nodiscard]] bool SetParameterString(uint8_t index, const std::string& parameter) noexcept;

        [[nodiscard]] bool Run() noexcept;

        [[nodiscard]] const std::vector<std::string>& GetStringReturnValues() const noexcept
        {
            return this->_stringReturnValues;
        }

        [[nodiscard]] const std::vector<uint32_t>& GetIntReturnValues() const noexcept
        {
            return this->_intReturnValues;
        }

    private:
        std::weak_ptr<Database> _database;

        std::vector<sqlite3_stmt*> _statements;

        std::vector<std::string> _stringReturnValues;
        std::vector<uint32_t> _intReturnValues;

        [[nodiscard]] bool GleanReturnValues(sqlite3_stmt* statement) noexcept;
    };
}

#endif
