// #include "realtime/Motion.hpp"

// //בנאי 
// Motion::Motion(Position src, Position dest, int arrival)
//     : source(src), destination(dest), arrivalTime(arrival) {}

// // החזרת נקודת המוצא
// Position Motion::getSource() const {
//     return source;
// }

// // החזרת נקודת היעד
// Position Motion::getDestination() const {
//     return destination;
// }

// // החזרת זמן ההגעה המתוכנן
// int Motion::getArrivalTime() const {
//     return arrivalTime;
// }

// // בדיקה האם הכלי הגיע ליעד, עפי זמן
// bool Motion::isFinished(int currentTime) const {
//     return currentTime >= arrivalTime;
// }


#include "realtime/Motion.hpp"

Motion::Motion(
    Position source,
    Position destination,
    int startTime,
    int arrivalTime)
    : source(source),
      destination(destination),
      startTime(startTime),
      arrivalTime(arrivalTime)
{
}

Position Motion::getSource() const
{
    return source;
}

Position Motion::getDestination() const
{
    return destination;
}

int Motion::getStartTime() const
{
    return startTime;
}

int Motion::getArrivalTime() const
{
    return arrivalTime;
}

bool Motion::isFinished(int currentTime) const
{
    return currentTime >= arrivalTime;
}