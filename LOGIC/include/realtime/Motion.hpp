// #pragma once

// #include "model/position.hpp" 

// class Motion {
// private:
//     Position source;
//     Position destination;
//     int arrivalTime; 

// public:
//     // בנאי
//     Motion(Position src, Position dest, int arrival);

//     // פונקציות גישה
//     Position getSource() const;
//     Position getDestination() const;
//     int getArrivalTime() const;
//     bool isFinished(int currentTime) const;
// };


#pragma once

#include "model/position.hpp"

class Motion
{
private:
    Position source;
    Position destination;
    int startTime;
    int arrivalTime;

public:
    Motion(
        Position source,
        Position destination,
        int startTime,
        int arrivalTime
    );

    Position getSource() const;
    Position getDestination() const;

    int getStartTime() const;
    int getArrivalTime() const;

    bool isFinished(int currentTime) const;
};
