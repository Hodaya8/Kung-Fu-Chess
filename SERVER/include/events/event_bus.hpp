#pragma once

#include <functional>
#include <utility>
#include <vector>

// Template class allows this bus to handle any event type
template<typename Event>
class EventBus
{
private:
    // Alias for a callback function that takes a const reference to an Event
    using EventHandler = std::function<void(const Event&)>;

    // List of all registered callback functions waiting for this event
    std::vector<EventHandler> subscribers;

public:
    // Default constructor
    EventBus() = default;

    // Prevent copying the event bus to avoid duplicating subscriber lists in memory
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;

    // Registers a new subscriber function to listen for events
    void subscribe(EventHandler handler)
    {
        subscribers.push_back(std::move(handler));
    }

    // Sends the event to all registered subscribers immediately
    void publish(const Event& event) const
    {
        for (const auto& handler : subscribers)
        {
            handler(event);
        }
    }

    // Removes all subscribers
    void clear()
    {
        subscribers.clear();
    }
};