// #pragma once
// #include "model/board.hpp" // מכיר את הלוח כי הוא בתוך התיקייה
// #include <vector>
// #include <string>
// #include <sstream>
// #include <stdexcept>

// class BoardParser {
// public:
//     static Board parse(const std::vector<std::string>& lines) {
//         std::vector<std::vector<std::string>> grid;
//         int expectedCols = -1;

//         for (const auto& line : lines) {
//             std::vector<std::string> row;
//             std::stringstream ss(line);
//             std::string token;
            
//             while (ss >> token) {
//                 row.push_back(token);
//             }

//             // אימות מלבניות הלוח
//             if (expectedCols == -1) expectedCols = row.size();
//             else if (static_cast<int>(row.size()) != expectedCols) 
//                 throw std::invalid_argument("Inconsistent row length");
            
//             grid.push_back(row);
//         }
//         return Board(grid.size(), expectedCols, grid);
//     }
// };

#pragma once

#include <istream>
#include "model/board.hpp"


class BoardParser
{
private:
    static bool isValidToken(const std::string& token);
public:
    static Board parse(std::istream& input);

};