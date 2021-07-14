#include <algorithm>
#include <fstream>
#include <sstream>

#include "statement.h"
#include "database.h"
#include "utils/util.h"
#include "api/logging/logging.h"

using namespace std::literals;

namespace projectfarm::shared::persistence
{
    void Statement::Shutdown() noexcept
    {
        if (this->_statements.empty())
        {
            api::logging::Log("Statement has already been shutdown.");
            return;
        }

        auto res = std::all_of(this->_statements.begin(), this->_statements.end(),
                      [this](auto& statement) -> bool
                      {
                        if (auto r = sqlite3_finalize(statement); r != SQLITE_OK)
                        {
                            auto message = sqlite3_errstr(r);
                            api::logging::Log("Failed to finalize statement with error: "s + message);
                            return false;
                        }

                        return true;
                      });

        if (!res)
        {
            api::logging::Log("Failed to finalize statement.");
            return;
        }

        this->_statements.clear();
    }

    bool Statement::PrepareFromFile(const std::filesystem::path& path) noexcept
    {
        const auto database = this->_database.lock();
        if (!database)
        {
            api::logging::Log("Database has already been shutdown.");
            return false;
        }

        std::ifstream file(path);
        if (!file.is_open())
        {
            api::logging::Log("Failed to open file: " + path.u8string());
            return false;
        }

        std::stringstream ss;
        ss << file.rdbuf();

        auto commands = pfu::split(";", ss.str());

        auto res = std::all_of(commands.begin(), commands.end(),
                      [this, &database](const auto& command)
                      {
                        auto c = pfu::trim(command);
                        if (c.empty())
                        {
                            return true;
                        }

                        sqlite3_stmt* statement = nullptr;

                        if (auto r = sqlite3_prepare_v3(database->GetConnection(), c.c_str(), -1,
                                                          SQLITE_PREPARE_PERSISTENT, &statement, nullptr);
                                r != SQLITE_OK)
                        {
                            auto message = sqlite3_errstr(r);
                            api::logging::Log("Failed to prepare statement with error: "s + message);
                            return false;
                        }

                        this->_statements.push_back(statement);

                        return true;
                      });

        if (!res)
        {
            api::logging::Log("Failed to prepare statement.");
            return false;
        }

        return true;
    }

    bool Statement::SetParameterInt(uint8_t index, uint64_t parameter) noexcept
    {
        if (this->_statements.empty())
        {
            api::logging::Log("Statement has not been setup.");
            return false;
        }

        for (auto& statement : this->_statements)
        {
            // we take a 0 based index. sqlite expects it to be base 1 here
            auto i = index + 1;

            // this may be a compound statement, so not all statements may have
            // the same number of statements
            // TODO: Really, parameters should also take a statement index rather
            // than checking like this
            auto numberOfParameters = sqlite3_bind_parameter_count(statement);
            if (i > numberOfParameters)
            {
                continue;
            }

            if (auto res = sqlite3_bind_int(statement, i, static_cast<int>(parameter));
                res != SQLITE_OK)
            {
                auto message = sqlite3_errstr(res);
                api::logging::Log("Failed to set parameter int: "s + std::to_string(parameter) + " with error: "s + message);
                // It may be that a statement doesn't use this index as the statement
                // is part of a sql file. Log the error, but ignore it
            }
        }

        return true;
    }

    bool Statement::SetParameterString(uint8_t index, const std::string& parameter) noexcept
    {
        if (this->_statements.empty())
        {
            api::logging::Log("Statement has not been setup.");
            return false;
        }

        for (auto& statement : this->_statements)
        {
            // we take a 0 based index. sqlite expects it to be base 1 here
            auto i = index + 1;

            // this may be a compound statement, so not all statements may have
            // the same number of statements
            // TODO: Really, parameters should also take a statement index rather
            // than checking like this
            auto numberOfParameters = sqlite3_bind_parameter_count(statement);
            if (i > numberOfParameters)
            {
                continue;
            }

            if (auto res = sqlite3_bind_text(statement, i, parameter.c_str(), -1, SQLITE_TRANSIENT);
                res != SQLITE_OK)
            {
                auto message = sqlite3_errstr(res);
                api::logging::Log("Failed to set parameter string: "s + parameter + " with error: "s + message);
                // It may be that a statement doesn't use this index as the statement
                // is part of a sql file. Log the error, but ignore it
            }
        }

        return true;
    }

    bool Statement::Run() noexcept
    {
        auto res = std::all_of(this->_statements.begin(), this->_statements.end(),
                               [this](auto& statement) -> bool
                               {
                                 if (auto r = sqlite3_step(statement);
                                     r != SQLITE_DONE && r != SQLITE_ROW)
                                 {
                                     auto message = sqlite3_errstr(r);
                                     api::logging::Log("Failed to run statement with error: "s + message);
                                     return false;
                                 }

                                 if (!this->GleanReturnValues(statement))
                                 {
                                     api::logging::Log("Failed to glean return values.");
                                     return false;
                                 }

                                 if (auto r = sqlite3_reset(statement);
                                     r != SQLITE_OK)
                                 {
                                     auto message = sqlite3_errstr(r);
                                     api::logging::Log("Failed to reset statement with error: "s + message);
                                     return false;
                                 }

                                 if (auto r = sqlite3_clear_bindings(statement);
                                     r != SQLITE_OK)
                                 {
                                     auto message = sqlite3_errstr(r);
                                     api::logging::Log("Failed to clear bindings for statement with error: "s + message);
                                     return false;
                                 }

                                 return true;
                               });

        if (!res)
        {
            api::logging::Log("Failed to run statement.");
            return false;
        }

        return true;
    }

    bool Statement::GleanReturnValues(sqlite3_stmt* statement) noexcept
    {
        auto numberOfValues {static_cast<decltype(this->_intReturnValues)::value_type>
                                (sqlite3_column_count(statement))};

        this->_intReturnValues.clear();
        this->_stringReturnValues.clear();

        for (auto i = 0u; i < numberOfValues; ++i)
        {
            auto type = sqlite3_column_type(statement, i);

            switch (type)
            {
                case SQLITE_INTEGER:
                {
                    auto res = sqlite3_column_int(statement, i);
                    this->_intReturnValues.push_back(res);
                    break;
                }
                case SQLITE_TEXT:
                {
                    auto res = reinterpret_cast<const char*>(sqlite3_column_text(statement, i));
                    this->_stringReturnValues.emplace_back(res);
                    break;
                }
                case SQLITE_NULL:
                {
                    api::logging::Log("Got SQLITE_NULL with value: " + std::to_string(i));
                    break;
                }
                default:
                {
                    // we only support ints and strings at the moment
                    api::logging::Log("Ignoring column result type: " + std::to_string(type) +
                                     " at column index: " + std::to_string(i));
                    continue;
                }
            }
        }

        return true;
    }
}