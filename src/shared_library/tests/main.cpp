#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"
#include "test_util.h"
#include <SDL.h>

int main(int argv, char* args[])
{
    Catch::Session session;

    CurrentWorkingDirectory = args[0];

    return session.run(argv, args);
}