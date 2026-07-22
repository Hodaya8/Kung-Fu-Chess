#include "input/mouse_input.hpp"

#include <utility>

#include <opencv2/opencv.hpp>

MouseInput::MouseInput(
    ClickHandler clickHandler)
    : clickHandler(
          std::move(clickHandler))
{
}

void MouseInput::attachToWindow(
    const std::string& windowName)
{
    cv::setMouseCallback(
        windowName,
        &MouseInput::mouseCallback,
        this
    );
}

void MouseInput::mouseCallback(
    int event,
    int x,
    int y,
    int flags,
    void* userData)
{
    static_cast<void>(flags);

    auto* mouseInput =
        static_cast<MouseInput*>(
            userData
        );

    if (!mouseInput)
    {
        return;
    }

    mouseInput->handleMouseEvent(
        event,
        x,
        y
    );
}

void MouseInput::handleMouseEvent(
    int event,
    int x,
    int y)
{
    MouseButton button;

    if (event == cv::EVENT_LBUTTONDOWN)
    {
        button = MouseButton::Left;
    }
    else if (
        event == cv::EVENT_RBUTTONDOWN)
    {
        button = MouseButton::Right;
    }
    else
    {
        return;
    }

    if (clickHandler)
    {
        clickHandler(
            MouseClick{
                x,
                y,
                button
            }
        );
    }
}