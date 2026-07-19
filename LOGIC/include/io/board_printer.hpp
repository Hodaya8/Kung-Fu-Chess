// #pragma once
// #include "model/board.hpp"
// #include <iostream>

// class BoardPrinter {
// public:
//     static void print(const Board& board) {
//         const auto& grid = board.getGrid();
//         for (const auto& row : grid) {
//             for (size_t i = 0; i < row.size(); ++i) {
//                 std::cout << row[i] << (i == row.size() - 1 ? "" : " ");
//             }
//             std::cout << "\n";
//         }
//     }
// };
#pragma once

#include <ostream>
#include "model/board.hpp"


class BoardPrinter
{
public:

    static void print(const Board& board, std::ostream& output);

};