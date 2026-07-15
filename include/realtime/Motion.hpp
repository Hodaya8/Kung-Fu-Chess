#ifndef MOTION_HPP
#define MOTION_HPP

#include "model/Position.hpp" 

class Motion {
private:
    Position source;
    Position destination;
    int arrivalTime; 

public:
    // בנאי
    Motion(Position src, Position dest, int arrival);

    // פונקציות גישה
    Position getSource() const;
    Position getDestination() const;
    int getArrivalTime() const;
    bool isFinished(int currentTime) const;
};

#endif