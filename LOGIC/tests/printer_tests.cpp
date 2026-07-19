#include <gtest/gtest.h>
#include "io/board_printer.hpp"
#include "model/board.hpp"
#include <sstream>

TEST(BoardPrinterTest, PrintFormat) {
    std::vector<std::vector<std::string>> grid = {
        {"wR", "bK"},
        {".", "wP"}
    };
    Board board(grid);
    std::ostringstream out;
    
    BoardPrinter::print(board, out);
    
    std::string expected = "wR bK\n. wP\n";
    EXPECT_EQ(out.str(), expected);
}