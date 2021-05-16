#include "crypto_provider.h"

namespace projectfarm::shared::crypto
{
    bool CryptoProvider::Initialize() const noexcept
    {
        this->LogMessage("Initializing crypto provider...");

        if (sodium_init() == -1)
        {
            this->LogMessage("Failed to initialize crypto provider.");
            return false;
        }

        this->LogMessage("Crypto provider initialized.");

        return true;
    }

    std::optional<std::string> CryptoProvider::QuickHash(std::string_view secret) const noexcept
    {
        if (secret.empty())
        {
            return "";
        }

        auto size = sizeof(char) * secret.size();

        char result[crypto_pwhash_STRBYTES];

        if (crypto_pwhash_str(result, secret.data(), size,
                              crypto_pwhash_OPSLIMIT_SENSITIVE, crypto_pwhash_MEMLIMIT_SENSITIVE) != 0)
        {
            this->LogMessage("Failed to hash secret.");
            return {};
        }

        return result;
    }

    bool CryptoProvider::Compare(std::string_view secret, std::string_view hash) noexcept
    {
        if (secret.empty() || hash.empty())
        {
            return false;
        }

        if (crypto_pwhash_str_verify(hash.data(), secret.data(), secret.size()) == 0)
        {
            return true;
        }

        // default to the negative
        return false;
    }
}