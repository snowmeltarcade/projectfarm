#include "test_util.h"

std::filesystem::path CurrentWorkingDirectory;

std::filesystem::path GetScriptingFilePath(std::filesystem::path fileName)
{
    auto path = CurrentWorkingDirectory / "../../src/shared_library/tests/test_data/scripting/" / fileName;
    return path.lexically_normal();
}

std::filesystem::path GetCSSFilePath(std::filesystem::path fileName)
{
    auto path = CurrentWorkingDirectory / "../../src/shared_library/tests/test_data/css/" / fileName;
    return path.lexically_normal();
}