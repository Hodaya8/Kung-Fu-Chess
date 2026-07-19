#pragma once

#include <vector>
#include <string>
#include "piece.hpp"
// מבנה שמייצג כלי בודד על הלוח ברגע נתון
struct PieceSnapshot {
    std::string type;         // סוג הכלי 
    double row;               // שורה  (מיקום נוכחי)
    double col;               // עמודה  (מיקום נוכחי)
    PieceState state;        // מצב הכלי:
    double animationProgress; // אחוז ההתקדמות של האנימציה או התנועה (0.0 עד 1.0)
    int timeRemaining;    //זמן נותן למנוחה
};

class GameSnapshot {
private:
    std::vector<PieceSnapshot> pieces;    //רשימת הכלים

public:
    GameSnapshot(const std::vector<PieceSnapshot>& pieces);  //בנאי
    const std::vector<PieceSnapshot>& getPieces() const;
};