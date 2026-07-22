#ifndef EVENT_BUS_HPP
#define EVENT_BUS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

// הגדרת סוג הפונקציה שכל מנוי צריך לספק כדי לקבל את המידע
using EventHandler = std::function<void(const std::string&)>;

class EventBus {
private:
    // מפה ששומרת עבור כל נושא (כמו "GameEnd") רשימה של פונקציות שמאזינות לו
    std::unordered_map<std::string, std::vector<EventHandler>> subscribers;

public:
    // פונקציה להרשמה (Subscribe) לאירוע
    void subscribe(const std::string& eventType, EventHandler handler) {
        subscribers[eventType].push_back(handler);
    }

    // פונקציה לפרסום (Publish) של אירוע
    void publish(const std::string& eventType, const std::string& payload = "") {
        // אם יש מנויים לאירוע הזה, נעבור על כולם ונפעיל אותם עם המידע שהתקבל
        if (subscribers.find(eventType) != subscribers.end()) {
            for (auto& handler : subscribers[eventType]) {
                handler(payload);
            }
        }
    }
};

#endif // EVENT_BUS_HPP