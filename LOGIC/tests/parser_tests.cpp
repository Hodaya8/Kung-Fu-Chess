#include <gtest/gtest.h>
#include "io/board_parser.hpp"
#include <sstream>

TEST(BoardParserTest, ParseValidStream) {
    std::string input = "Board:\nwR bK\n. wP\nCommands:\n";
    std::istringstream stream(input);
    
    Board b = BoardParser::parse(stream);
    EXPECT_EQ(b.rows(), 2);
    EXPECT_EQ(b.cols(), 2);
    EXPECT_EQ(b.at(0,1), "bK");
}

TEST(BoardParserTest, ParseInvalidToken) {
    std::string input = "Board:\nXX bK\nCommands:\n";
    std::istringstream stream(input);
    EXPECT_THROW(BoardParser::parse(stream), std::runtime_error);
}
TEST(BoardParserTest, RowWidthMismatchThrows) {
    std::string input = 
        "Board:\n"
        "wR wN\n"
        "wP\n" // שורה קצרה מדי
        "Commands:\n";
    
    std::istringstream stream(input);
    EXPECT_THROW(BoardParser::parse(stream), std::runtime_error);
}

TEST(BoardParserTest, ExtraneousWhitespaces) {
    std::string input = 
        "Board:\n"
        "   wR    bK  \n" // רווחים מרובים
        "  .      wP\n"
        "Commands:\n";
    
    std::istringstream stream(input);
    Board b = BoardParser::parse(stream);
    EXPECT_EQ(b.rows(), 2);
    EXPECT_EQ(b.cols(), 2);
    EXPECT_EQ(b.at(0,0), "wR");
}