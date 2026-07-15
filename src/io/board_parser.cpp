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
    //מטריצת עזר
    std::vector<std::vector<std::string>> cells;

    std::string line;

    bool readingBoard = false;

    //קריאה שורה אחר שורה
    while (std::getline(input, line))
    {
        while (!line.empty() && std::isspace(line.front()))
        {
            line.erase(line.begin());
        }
        if (line == "Board:")
        {
            readingBoard = true;
            continue;
        }
        
        if (line == "Commands:")
        {
            break;
        }

        if (!readingBoard)
            continue;
        
        //לקריאת מילים מתוך השורה
        std::stringstream ss(line);

        //מייצג שורה בודדת
        std::vector<std::string> row;

        //מייצג תא בודד
        std::string cell;

        //קריאת מילה בכל פעם
        while (ss >> cell)
        {
            //בדיקת תקינות
            if (!isValidToken(cell))
            {
                throw std::runtime_error("UNKNOWN_TOKEN");
            }

            row.push_back(cell);
        }

        // אם המטריצה לא ריקה וגם מלבנית
        if (!cells.empty() && row.size() != cells[0].size())
        {
            throw std::runtime_error("ROW_WIDTH_MISMATCH");
        }


        cells.push_back(row);
    }


    return Board(cells);
}