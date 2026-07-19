#include "realtime/Motion.hpp"

//בנאי 
Motion::Motion(Position src, Position dest, int arrival)
    : source(src), destination(dest), arrivalTime(arrival) {}

// החזרת נקודת המוצא
Position Motion::getSource() const {
    return source;
}

// החזרת נקודת היעד
Position Motion::getDestination() const {
    return destination;
}

// החזרת זמן ההגעה המתוכנן
int Motion::getArrivalTime() const {
    return arrivalTime;
}

// בדיקה האם הכלי הגיע ליעד, עפי זמן
bool Motion::isFinished(int currentTime) const {
    return currentTime >= arrivalTime;
}