#include <iostream>
#include <mutex>
#include <optional>
#include <string>

#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>

#include "model/position.hpp"
#include "network/server_game_state.hpp"
#include "network/websocket_client.hpp"
#include "render_model.hpp"
#include "renderer.hpp"

using Json = nlohmann::json;

namespace
{
    constexpr int FRAME_DURATION_MS = 20;
    constexpr int ESC_KEY_CODE = 27;

    const std::string SERVER_URI =
        "ws://127.0.0.1:9002";

    // המידע שפונקציית העכבר צריכה
    struct MouseContext
    {
        WebSocketClient* webSocketClient;
    };

    void onMouseClick(
        int event,
        int x,
        int y,
        int flags,
        void* userData)
    {
        static_cast<void>(flags);

        auto* context =
            static_cast<MouseContext*>(
                userData
            );

        if (!context ||
            !context->webSocketClient)
        {
            return;
        }

        std::string button;

        if (event ==
            cv::EVENT_LBUTTONDOWN)
        {
            button = "left";
        }
        else if (
            event ==
            cv::EVENT_RBUTTONDOWN)
        {
            button = "right";
        }
        else
        {
            return;
        }

        const Json request = {
            {
                "messageType",
                "ClickRequest"
            },
            {
                "x",
                x
            },
            {
                "y",
                y
            },
            {
                "button",
                button
            }
        };

        if (
            context
                ->webSocketClient
                ->send(request.dump())
        )
        {
            std::cout
                << "[UI] Click sent: "
                << button
                << " ("
                << x
                << ", "
                << y
                << ")"
                << std::endl;
        }
    }
}

int main()
{
    try
    {
        const std::string assetsDirectory =
            R"(C:\Users\USER\Desktop\chess game\UI\asset)";

        Renderer renderer(
            assetsDirectory
        );

        // מצב משותף בין תהליכון הרשת
        // לבין תהליכון הציור
        std::mutex serverStateMutex;

        std::optional<ServerGameState>
            latestServerState;

        std::optional<Position>
            selectedPosition;

        // קבלת הודעות מהשרת
        WebSocketClient webSocketClient(
            [
                &serverStateMutex,
                &latestServerState,
                &selectedPosition
            ](const std::string& message)
            {
                try
                {
                    const Json json =
                        Json::parse(message);

                    const std::string
                        messageType =
                            json.value(
                                "messageType",
                                std::string{}
                            );

                    // מצב המשחק
                    if (
                        messageType ==
                        "GameState"
                    )
                    {
                        const auto parsedState =
                            parseServerGameState(
                                message
                            );

                        if (!parsedState.has_value())
                        {
                            return;
                        }

                        std::lock_guard<
                            std::mutex
                        > lock(
                            serverStateMutex
                        );

                        latestServerState =
                            parsedState;

                        return;
                    }

                    // מצב הבחירה
                    if (
                        messageType ==
                        "SelectionState"
                    )
                    {
                        const bool hasSelection =
                            json.at(
                                "hasSelection"
                            ).get<bool>();

                        std::lock_guard<
                            std::mutex
                        > lock(
                            serverStateMutex
                        );

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
                    const std::exception&
                        exception
                )
                {
                    std::cerr
                        << "[UI] Invalid server message: "
                        << exception.what()
                        << std::endl;
                }
            }
        );

        if (!webSocketClient.connect(
                SERVER_URI))
        {
            std::cerr
                << "[UI] Could not connect "
                << "to server."
                << std::endl;
        }

        cv::namedWindow("Image");

        MouseContext mouseContext{
            &webSocketClient
        };

        cv::setMouseCallback(
            "Image",
            onMouseClick,
            &mouseContext
        );

        std::cout
            << "[UI] Waiting for server state..."
            << std::endl;

        bool firstStateRendered = false;

        while (true)
        {
            std::optional<ServerGameState>
                stateForRendering;

            std::optional<Position>
                selectionForRendering;

            // העתקה קצרה ובטוחה
            {
                std::lock_guard<std::mutex>
                    lock(serverStateMutex);

                stateForRendering =
                    latestServerState;

                selectionForRendering =
                    selectedPosition;
            }

            if (stateForRendering.has_value())
            {
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

            const int key =
                cv::waitKey(
                    FRAME_DURATION_MS
                );

            if (key ==
                ESC_KEY_CODE)
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
        std::cerr
            << "[UI ERROR] "
            << exception.what()
            << std::endl;

        return 1;
    }
}