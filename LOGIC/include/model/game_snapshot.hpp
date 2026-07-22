// #pragma once

// #include <vector>
// #include <string>
// #include "piece.hpp"
// // מבנה שמייצג כלי בודד על הלוח ברגע נתון
// struct PieceSnapshot {
//     std::string type;         // סוג הכלי 
//     double row;               // שורה  (מיקום נוכחי)
//     double col;               // עמודה  (מיקום נוכחי)
//     PieceState state;        // מצב הכלי:
//     double animationProgress; // אחוז ההתקדמות של האנימציה או התנועה (0.0 עד 1.0)
//     int timeRemaining;    //זמן נותן למנוחה
// };

// class GameSnapshot {
// private:
//     std::vector<PieceSnapshot> pieces;    //רשימת הכלים

// public:
//     GameSnapshot(const std::vector<PieceSnapshot>& pieces);  //בנאי
//     const std::vector<PieceSnapshot>& getPieces() const;
// };

#pragma once

#include <string>
#include <vector>

#include "model/piece.hpp"

struct PieceSnapshot
{
    std::string type;

    PieceState state = PieceState::idle;

    // המיקום הלוגי של הכלי
    int row = 0;
    int col = 0;

    // יעד של תנועה. בכלי שאינו נע, היעד זהה למיקום.
    int destinationRow = 0;
    int destinationCol = 0;

    // זמני הפעולה של תנועה או קפיצה
    int actionStartTime = 0;
    int actionEndTime = 0;

    // נתוני מנוחה
    int timeRemaining = -1;
    int stateDuration = 0;
};

class GameSnapshot
{
private:
    std::vector<PieceSnapshot> pieces;
    int currentTime;

public:
    GameSnapshot(
        const std::vector<PieceSnapshot>& pieces,
        int currentTime
    );

    const std::vector<PieceSnapshot>& getPieces() const;
    int getCurrentTime() const;
};