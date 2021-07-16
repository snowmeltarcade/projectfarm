#ifndef PROJECTFARM_CRYPTO_PROVIDER_H
#define PROJECTFARM_CRYPTO_PROVIDER_H

#include <string>
#include <optional>
#include <sodium.h>

namespace projectfarm::shared::crypto
{
    class CryptoProvider final
    {
    public:
        CryptoProvider() = default;
        ~CryptoProvider() = default;

        [[nodiscard]]
        bool Initialize() const noexcept;

        [[nodiscard]]
        std::optional<std::string> QuickHash(std::string_view secret) const noexcept;

        [[nodiscard]]
        static bool Compare(std::string_view secret, std::string_view hash) noexcept;
    };
}

#endif
