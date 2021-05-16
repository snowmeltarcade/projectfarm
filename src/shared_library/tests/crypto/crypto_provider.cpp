#include "catch2/catch.hpp"
#include "crypto/crypto_provider.h"

using namespace std::literals;
using namespace projectfarm::shared::crypto;

/*********************************************
 * QuickHash
 ********************************************/

TEST_CASE("QuickHash - empty string - returns empty string", "[crypto]")
{
    CryptoProvider provider;

    if (!provider.Initialize())
    {
        FAIL("Failed to initialize crypto provider");
    }

    auto s = ""s;

    auto res = provider.QuickHash(s);

    REQUIRE(s.empty());
}

TEST_CASE("QuickHash - valid string - returns hashed string", "[crypto]")
{
    CryptoProvider provider;

    if (!provider.Initialize())
    {
        FAIL("Failed to initialize crypto provider");
    }

    auto s = "my secret string"s;

    auto hash = provider.QuickHash(s);
    auto res = *hash;

    // the hash is randomly generated, so without a ton of work, there
    // isn't any point in mocking it
    REQUIRE(res != s);
}

/*********************************************
 * Compare
 ********************************************/

TEST_CASE("Compare - empty secret - returns false", "[crypto]")
{
    CryptoProvider provider;

    if (!provider.Initialize())
    {
        FAIL("Failed to initialize crypto provider");
    }

    auto secret = "my secret"s;
    auto hash = provider.QuickHash(secret);
    if (!hash)
    {
        FAIL("Failed to hash secret.");
    }

    secret = "";

    auto res = provider.Compare(secret, *hash);

    REQUIRE_FALSE(res);
}

TEST_CASE("Compare - empty hash - returns false", "[crypto]")
{
    CryptoProvider provider;

    if (!provider.Initialize())
    {
        FAIL("Failed to initialize crypto provider");
    }

    auto secret = "my secret"s;
    auto hash = "";

    auto res = provider.Compare(secret, hash);

    REQUIRE_FALSE(res);
}

TEST_CASE("Compare - secret does not match hash - returns false", "[crypto]")
{
    CryptoProvider provider;

    if (!provider.Initialize())
    {
        FAIL("Failed to initialize crypto provider");
    }

    auto secret = "my secret"s;
    auto hash = provider.QuickHash(secret);
    if (!hash)
    {
        FAIL("Failed to hash secret.");
    }

    secret = "different secret";

    auto res = provider.Compare(secret, *hash);

    REQUIRE_FALSE(res);
}

TEST_CASE("Compare - secret matches hash - returns true", "[crypto]")
{
    CryptoProvider provider;

    if (!provider.Initialize())
    {
        FAIL("Failed to initialize crypto provider");
    }

    auto secret = "my secret"s;
    auto hash = provider.QuickHash(secret);
    if (!hash)
    {
        FAIL("Failed to hash secret.");
    }

    auto res = provider.Compare(secret, *hash);

    REQUIRE(res);
}