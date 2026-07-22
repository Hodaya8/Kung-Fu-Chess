#pragma once

#include <functional>
#include <string>

enum class MouseButton
{
    Left,
    Right
};

struct MouseClick
{
    int x;
    int y;
    MouseButton button;
};

class MouseInput
{
public:
    using ClickHandler =
        std::function<void(const MouseClick&)>;

    explicit MouseInput(
        ClickHandler clickHandler
    );

    void attachToWindow(
        const std::string& windowName
    );

private:
    ClickHandler clickHandler;

    static void mouseCallback(
        int event,
        int x,
        int y,
        int flags,
        void* userData
    );

    void handleMouseEvent(
        int event,
        int x,
        int y
    );
};