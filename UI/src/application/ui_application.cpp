#include "application/ui_application.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>

#include "protocol/client_protocol.hpp"
#include "render/render_model.hpp"

using Json = nlohmann::json;

namespace
{
    constexpr int FRAME_DURATION_MS = 20;
    constexpr int ESC_KEY_CODE = 27;

    constexpr int BOARD_ROWS = 8;
    constexpr int BOARD_COLUMNS = 8;

    const std::string WINDOW_NAME =
        "Image";
}

UiApplication::UiApplication(
    const std::string& serverUri,
    const std::string& assetsDirectory,
    const std::string& username,
    const std::string& password)
    : serverUri(serverUri),
      username(username),
      password(password),
      renderer(assetsDirectory),
      boardMapper(
          BOARD_ROWS,
          BOARD_COLUMNS
      ),
      controller(
          boardMapper,
          [this](
              Position source,
              Position destination)
          {
              sendMoveRequest(
                  source,
                  destination
              );
          },
          [this](Position cell)
          {
              sendJumpRequest(
                  cell
              );
          }
      ),
      firstStateRendered(false),
      mouseInput(
          [this](const MouseClick& click)
          {
              handleMouseClick(
                  click
              );
          }
      ),
      webSocketClient(
          [this](const std::string& message)
          {
              handleServerMessage(
                  message
              );
          },
          [this]()
          {
              handleConnected();
          }
      )
{
}

int UiApplication::run()
{
    try
    {
        if (!webSocketClient.connect(
                serverUri))
        {
            std::cerr
                << "[UI] Could not connect "
                << "to server."
                << std::endl;
        }

        cv::namedWindow(
            WINDOW_NAME
        );

        mouseInput.attachToWindow(
            WINDOW_NAME
        );

        std::cout
            << "[UI] Waiting for server state..."
            << std::endl;

        while (true)
        {
            renderLatestState();

            const int key =
                cv::waitKey(
                    FRAME_DURATION_MS
                );

            if (key == ESC_KEY_CODE)
            {
                break;
            }
        }

        cv::destroyAllWindows();

        return 0;
    }
    catch (
        const std::exception&
            exception)
    {
        cv::destroyAllWindows();

        std::cerr
            << "[UI ERROR] "
            << exception.what()
            << std::endl;

        return 1;
    }
}

void UiApplication::handleConnected()
{
    const std::string message =
        ClientProtocol::createLoginRequestMessage(
            username,
            password
        );

    if (webSocketClient.send(
            message))
    {
        std::cout
            << "[UI] Login request sent for: "
            << username
            << std::endl;

        password.clear();
    }
}

void UiApplication::handleServerMessage(
    const std::string& message)
{
    try
    {
        const Json json =
            Json::parse(
                message
            );

        const std::string messageType =
            json.value(
                "messageType",
                std::string{}
            );

        if (messageType == "LoginAccepted")
        {
            const std::string acceptedUsername =
                json.at("username")
                    .get<std::string>();

            const int rating =
                json.at("rating")
                    .get<int>();

            const bool registered =
                json.at("registered")
                    .get<bool>();

            std::cout
                << "[UI] "
                << (
                    registered
                        ? "New user registered: "
                        : "Logged in as: "
                )
                << acceptedUsername
                << std::endl;

            std::cout
                << "[UI] Rating: "
                << rating
                << std::endl;

            sendFindMatchRequest();

            return;
        }

        if (messageType == "LoginRejected")
        {
            std::cerr
                << "[UI] Login rejected: "
                << json.at("reason")
                       .get<std::string>()
                << std::endl;

            return;
        }

        if (messageType == "WaitingForMatch")
        {
            std::cout
                << "[UI] Waiting for a matching player..."
                << std::endl;

            return;
        }

        if (messageType == "MatchFound")
        {
            const int assignedRoomId =
                json.at("roomId")
                    .get<int>();

            const std::string side =
                json.at("side")
                    .get<std::string>();

            Color assignedColor;

            if (side == "white")
            {
                assignedColor =
                    Color::WHITE;
            }
            else if (side == "black")
            {
                assignedColor =
                    Color::BLACK;
            }
            else
            {
                throw std::invalid_argument(
                    "Unknown player side."
                );
            }

            {
                std::lock_guard<std::mutex>
                    lock(serverStateMutex);

                roomId =
                    assignedRoomId;

                playerColor =
                    assignedColor;
            }

            std::cout
                << "[UI] Match found. Room: "
                << assignedRoomId
                << ", side: "
                << side
                << std::endl;

            return;
        }

        if (messageType == "GameState")
        {
            const auto parsedState =
                parseServerGameState(
                    message
                );

            if (!parsedState.has_value())
            {
                return;
            }

            std::lock_guard<std::mutex>
                lock(serverStateMutex);

            latestServerState =
                parsedState;

            return;
        }
    }
    catch (
        const std::exception&
            exception)
    {
        std::cerr
            << "[UI] Invalid server message: "
            << exception.what()
            << std::endl;
    }
}

void UiApplication::handleMouseClick(
    const MouseClick& click)
{
    std::optional<ServerGameState>
        currentState;

    std::optional<Color>
        currentPlayerColor;

    {
        std::lock_guard<std::mutex>
            lock(serverStateMutex);

        currentState =
            latestServerState;

        currentPlayerColor =
            playerColor;
    }

    if (
        !currentState.has_value() ||
        !currentPlayerColor.has_value()
    )
    {
        return;
    }

    if (currentState->gameOver)
    {
        return;
    }

    if (
        click.button ==
        MouseButton::Left
    )
    {
        controller.handleLeftClick(
            click.x,
            click.y,
            currentState->snapshot,
            currentPlayerColor.value()
        );
    }
    else
    {
        controller.handleRightClick(
            click.x,
            click.y,
            currentState->snapshot,
            currentPlayerColor.value()
        );
    }
}

void UiApplication::sendFindMatchRequest()
{
    const std::string message =
        ClientProtocol::
            createFindMatchRequestMessage();

    if (webSocketClient.send(
            message))
    {
        std::cout
            << "[UI] Match request sent."
            << std::endl;
    }
}

void UiApplication::sendMoveRequest(
    Position source,
    Position destination)
{
    const std::string message =
        ClientProtocol::createMoveRequestMessage(
            source,
            destination
        );

    if (webSocketClient.send(
            message))
    {
        std::cout
            << "[UI] Move request sent: ("
            << source.getRow()
            << ", "
            << source.getCol()
            << ") -> ("
            << destination.getRow()
            << ", "
            << destination.getCol()
            << ")"
            << std::endl;
    }
}

void UiApplication::sendJumpRequest(
    Position cell)
{
    const std::string message =
        ClientProtocol::createJumpRequestMessage(
            cell
        );

    if (webSocketClient.send(
            message))
    {
        std::cout
            << "[UI] Jump request sent: ("
            << cell.getRow()
            << ", "
            << cell.getCol()
            << ")"
            << std::endl;
    }
}

void UiApplication::renderLatestState()
{
    std::optional<ServerGameState>
        stateForRendering;

    {
        std::lock_guard<std::mutex>
            lock(serverStateMutex);

        stateForRendering =
            latestServerState;
    }

    if (!stateForRendering.has_value())
    {
        return;
    }

    if (!firstStateRendered)
    {
        std::cout
            << "[UI] Received "
            << stateForRendering
                   ->snapshot
                   .getPieces()
                   .size()
            << " pieces."
            << std::endl;

        firstStateRendered =
            true;
    }

    const RenderModel renderModel =
        buildRenderModel(
            stateForRendering->snapshot
        );

    renderer.render(
        renderModel,
        stateForRendering->gameOver,
        stateForRendering->whiteScore,
        stateForRendering->blackScore,
        controller.getSelectedPosition()
    );
}