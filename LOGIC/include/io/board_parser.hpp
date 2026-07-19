

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