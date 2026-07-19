#include "io\board_parser.hpp"

#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>

bool BoardParser::isValidToken(const std::string& token)
{
    if (token == ".")
        return true;

    if (token.size() != 2)
        return false;

    char color = token[0];
    char piece = token[1];

    return (color == 'w' || color == 'b') &&
           (piece == 'K' ||
            piece == 'Q' ||
            piece == 'R' ||
            piece == 'B' ||
            piece == 'N' ||
            piece == 'P');
}


//המרת הקלט למבנה לוח
Board BoardParser::parse(std::istream& input)
{
    // 1. נכין מטריצה של אובייקטים (כפי שהלוח מצפה לקבל)
    std::vector<std::vector<std::shared_ptr<Piece>>> cells;
    std::string line;
    bool readingBoard = false;
    int row_index = 0;

    while (std::getline(input, line))
    {
        while (!line.empty() && std::isspace(line.front()))
            line.erase(line.begin());

        if (line == "Board:") { readingBoard = true; continue; }
        if (line == "Commands:") break;
        if (!readingBoard) continue;
        
        std::stringstream ss(line);
        std::string cell;
        
        // 2. יצירת שורה של אובייקטים
        std::vector<std::shared_ptr<Piece>> row;
        int col_index = 0;

        while (ss >> cell)
        {
            if (!isValidToken(cell))
                throw std::runtime_error("UNKNOWN_TOKEN");

            // 3. המרה ישירה של הטוקן לאובייקט
            if (cell == ".") {
                row.push_back(nullptr);
            } else {
                Color color = (cell[0] == 'w') ? Color::WHITE : Color::BLACK;
                PieceType type;
                char t = cell[1];
                if (t == 'R') type = PieceType::ROOK;
                else if (t == 'B') type = PieceType::BISHOP;
                else if (t == 'Q') type = PieceType::QUEEN;
                else if (t == 'N') type = PieceType::KNIGHT;
                else if (t == 'K') type = PieceType::KING;
                else type = PieceType::PAWN;

                row.push_back(std::make_shared<Piece>(row_index * 8 + col_index, color, type, Position(row_index, col_index)));
            }
            col_index++;
        }

        if (!cells.empty() && row.size() != cells[0].size())
            throw std::runtime_error("ROW_WIDTH_MISMATCH");

        cells.push_back(row);
        row_index++;
    }

    return Board(cells);
}