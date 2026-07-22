#pragma once

#include <optional>
#include <string>

#include "model/game_snapshot.hpp"

// מצב המשחק כפי שהתקבל מהשרת
struct ServerGameState
{
    GameSnapshot snapshot;
    bool gameOver = false;

    int whiteScore = 0;
    int blackScore = 0;
};

// ממירה הודעת JSON שהתקבלה מהשרת
// למבני C++ שה-UI מכיר
std::optional<ServerGameState>
parseServerGameState(
    const std::string& message
);