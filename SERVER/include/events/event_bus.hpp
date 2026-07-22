#ifndef EVENT_BUS_HPP
#define EVENT_BUS_HPP

#include <functional>
#include <utility>
#include <vector>

template<typename Event>
class EventBus {
private:
    using EventHandler = std::function<void(const Event&)>;

    std::vector<EventHandler> subscribers;

public:
    void subscribe(EventHandler handler) {
        subscribers.push_back(std::move(handler));
    }

    void publish(const Event& event) const {
        for (const auto& handler : subscribers) {
            handler(event);
        }
    }
};

#endif // EVENT_BUS_HPP