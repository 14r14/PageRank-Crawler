#include "fileio.h"
#include <iostream>
#include <fstream>

namespace utils::file
{
    std::vector<std::variant<int, double, std::string>> readAllLines(std::string filePath)
    {
        std::vector<std::variant<int, double, std::string>> lines;

        std::ifstream inFile(filePath);

        // Check if the file is opened
        if (!inFile.is_open())
        {
            std::clog << "Error: Unable to open file!" << std::endl;
        }
        std::string s;
        while (getline(inFile, s))
        {
            lines.push_back(s);
        }

        // Check for eof
        if (inFile.eof())
            std::clog << "Reached end of file." << std::endl;
        else
            std::clog << "Error: File reading failed!" << std::endl;

        inFile.close();
        return lines;
    }
}