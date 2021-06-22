#ifndef PROJECTFARM_TEST_UTIL_H
#define PROJECTFARM_TEST_UTIL_H

#include <filesystem>

extern std::filesystem::path CurrentWorkingDirectory;

std::filesystem::path GetScriptingFilePath(std::filesystem::path fileName);

std::filesystem::path GetCSSFilePath(std::filesystem::path fileName);

#endif