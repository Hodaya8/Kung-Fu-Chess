#include "io/board_printer.hpp"
#include <iostream>

void BoardPrinter::print(const Board& board, std::ostream& output)
{
    for (int row = 0; row < board.rows(); row++)
    {
        for (int col = 0; col < board.cols(); col++)
        {
            auto piece = board.at(row, col);
            
            if (piece == nullptr) {
                output << ".";
            } else {
                // נניח שיש לך פונקציה toString במחלקה Piece שמחזירה "wK"
                output << piece->toString();
            }

            if (col + 1 < board.cols())
            {
                output << " ";
            }
        }
        output << '\n';
    }
}