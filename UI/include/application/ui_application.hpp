#pragma once

#include <mutex>
#include <optional>
#include <string>

#include "input/mouse_input.hpp"
#include "model/position.hpp"
#include "network/server_game_state.hpp"
#include "network/websocket_client.hpp"
#include "render/renderer.hpp"

class UiApplication
{
public:
    UiApplication(
        const std::string& serverUri,
        const std::string& assetsDirectory
    );

    int run();

    UiApplication(
        const UiApplication&
    ) = delete;

    UiApplication& operator=(
        const UiApplication&
    ) = delete;

private:
    std::string serverUri;

    Renderer renderer;

    std::mutex serverStateMutex;

    std::optional<ServerGameState>
        latestServerState;

    std::optional<Position>
        selectedPosition;

    bool firstStateRendered;

    MouseInput mouseInput;
    WebSocketClient webSocketClient;

    void handleServerMessage(
        const std::string& message
    );

    void handleMouseClick(
        const MouseClick& click
    );

    void renderLatestState();
};