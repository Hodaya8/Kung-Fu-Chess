#include "application/ui_application.hpp"

#include <exception>
#include <iostream>
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

    const std::string WINDOW_NAME =
        "Image";
}

UiApplication::UiApplication(
    const std::string& serverUri,
    const std::string& assetsDirectory)
    : serverUri(serverUri),
      renderer(assetsDirectory),
      firstStateRendered(false),
      mouseInput(
          [this](
              const MouseClick& click)
          {
              handleMouseClick(click);
          }
      ),
      webSocketClient(
          [this](
              const std::string& message)
          {
              handleServerMessage(message);
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
        const std::exception& exception
    )
    {
        cv::destroyAllWindows();

        std::cerr
            << "[UI ERROR] "
            << exception.what()
            << std::endl;

        return 1;
    }
}

void UiApplication::handleServerMessage(
    const std::string& message)
{
    try
    {
        const Json json =
            Json::parse(message);

        const std::string messageType =
            json.value(
                "messageType",
                std::string{}
            );

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

        if (
            messageType ==
            "SelectionState"
        )
        {
            const bool hasSelection =
                json.at(
                    "hasSelection"
                ).get<bool>();

            std::lock_guard<std::mutex>
                lock(serverStateMutex);

            if (hasSelection)
            {
                selectedPosition.emplace(
                    json.at("row")
                        .get<int>(),
                    json.at("col")
                        .get<int>()
                );
            }
            else
            {
                selectedPosition.reset();
            }
        }
    }
    catch (
        const std::exception& exception
    )
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
    const std::string message =
        ClientProtocol::
            createClickRequestMessage(
                click
            );

    if (webSocketClient.send(
            message))
    {
        std::cout
            << "[UI] Click sent: ("
            << click.x
            << ", "
            << click.y
            << ")"
            << std::endl;
    }
}

void UiApplication::renderLatestState()
{
    std::optional<ServerGameState>
        stateForRendering;

    std::optional<Position>
        selectionForRendering;

    // נועלים רק בזמן העתקת המידע.
    {
        std::lock_guard<std::mutex>
            lock(serverStateMutex);

        stateForRendering =
            latestServerState;

        selectionForRendering =
            selectedPosition;
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

        firstStateRendered = true;
    }

    const RenderModel renderModel =
        buildRenderModel(
            stateForRendering
                ->snapshot
        );

    renderer.render(
        renderModel,
        stateForRendering
            ->gameOver,
        selectionForRendering
    );
}