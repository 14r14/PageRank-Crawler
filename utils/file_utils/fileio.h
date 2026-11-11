#pragma once
#include <string>
#include <vector>
#include <variant>

namespace utils::file
{
    std::vector<std::variant<int, double, std::string>> readAllLines(std::string filePath);
}