#include "io/board_printer.hpp"


void BoardPrinter::print(
    const Board& board,
    std::ostream& output
)
{
    for (int row = 0; row < board.rows(); row++)
    {
        for (int col = 0; col < board.cols(); col++)
        {
            output << board.at(row, col);

            if (col + 1 < board.cols())
            {
                output << " ";
            }
        }

        output << '\n';
    }
}