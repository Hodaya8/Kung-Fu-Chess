#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <string>

#include <nlohmann/json.hpp>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "engine/GameEngine.hpp"
#include "input/BoardMapper.hpp"
#include "input/Controller.hpp"
#include "io/board_parser.hpp"
#include "model/board.hpp"
#include "model/game_snapshot.hpp"
#include "model/position.hpp"

using Json = nlohmann::json;

using Server =
    websocketpp::server<
        websocketpp::config::asio
    >;

using ConnectionHandle =
    websocketpp::connection_hdl;

namespace
{
    constexpr int SERVER_PORT = 9002;
    constexpr int GAME_TICK_MS = 20;
    constexpr int BOARD_ROWS = 8;
    constexpr int BOARD_COLUMNS = 8;
}

Board createInitialBoard()
{
    const std::string initialBoard =
        "Board:\n"
        "bR bN bB bQ bK bB bN bR\n"
        "bP bP bP bP bP bP bP bP\n"
        ". . . . . . . .\n"
        ". . . . . . . .\n"
        ". . . . . . . .\n"
        ". . . . . . . .\n"
        "wP wP wP wP wP wP wP wP\n"
        "wR wN wB wQ wK wB wN wR\n";

    std::stringstream stream(
        initialBoard
    );

    return BoardParser::parse(stream);
}

// כל המידע הפעיל של המשחק בשרת
struct ServerState
{
    Board board;
    GameEngine engine;
    BoardMapper mapper;
    Controller controller;

    std::set<
        ConnectionHandle,
        std::owner_less<ConnectionHandle>
    > connections;

    ServerState()
        : board(createInitialBoard()),
          engine(board),
          mapper(
              BOARD_ROWS,
              BOARD_COLUMNS
          ),
          controller(
              board,
              mapper,
              engine
          )
    {
    }
};

std::string pieceStateToString(
    PieceState state)
{
    switch (state)
    {
        case PieceState::idle:
            return "idle";

        case PieceState::move:
            return "move";

        case PieceState::jump:
            return "jump";

        case PieceState::long_rest:
            return "long_rest";

        case PieceState::short_rest:
            return "short_rest";

        case PieceState::captured:
            return "captured";
    }

    return "unknown";
}

Json snapshotToJson(
    const GameSnapshot& snapshot,
    bool gameOver)
{
    Json pieces = Json::array();

    for (const auto& piece :
         snapshot.getPieces())
    {
        pieces.push_back({
            {
                "type",
                piece.type
            },
            {
                "state",
                pieceStateToString(
                    piece.state
                )
            },
            {
                "row",
                piece.row
            },
            {
                "col",
                piece.col
            },
            {
                "destinationRow",
                piece.destinationRow
            },
            {
                "destinationCol",
                piece.destinationCol
            },
            {
                "actionStartTime",
                piece.actionStartTime
            },
            {
                "actionEndTime",
                piece.actionEndTime
            },
            {
                "timeRemaining",
                piece.timeRemaining
            },
            {
                "stateDuration",
                piece.stateDuration
            }
        });
    }

    return {
        {
            "messageType",
            "GameState"
        },
        {
            "currentTime",
            snapshot.getCurrentTime()
        },
        {
            "gameOver",
            gameOver
        },
        {
            "pieces",
            pieces
        }
    };
}

// שליחת מצב המשחק לכל הלקוחות
void broadcastGameState(
    Server* server,
    ServerState* state)
{
    if (state->connections.empty())
    {
        return;
    }

    const GameSnapshot snapshot =
        state->engine.getSnapshot();

    const Json json =
        snapshotToJson(
            snapshot,
            state->engine.isGameOver()
        );

    const std::string message =
        json.dump();

    for (const auto& connection :
         state->connections)
    {
        websocketpp::lib::error_code error;

        server->send(
            connection,
            message,
            websocketpp::frame::opcode::text,
            error
        );

        if (error)
        {
            std::cerr
                << "[SERVER] Failed to send state: "
                << error.message()
                << std::endl;
        }
    }
}

// שליחת מצב הבחירה ללקוח שביצע את הקליק
void sendSelectionState(
    Server* server,
    ServerState* state,
    ConnectionHandle connection)
{
    Json response = {
        {
            "messageType",
            "SelectionState"
        },
        {
            "hasSelection",
            state->controller.isSelected()
        }
    };

    if (state->controller.isSelected())
    {
        const Position selected =
            state->controller.getSelected();

        response["row"] =
            selected.getRow();

        response["col"] =
            selected.getCol();
    }

    websocketpp::lib::error_code error;

    server->send(
        connection,
        response.dump(),
        websocketpp::frame::opcode::text,
        error
    );

    if (error)
    {
        std::cerr
            << "[SERVER] Failed to send selection: "
            << error.message()
            << std::endl;
    }
}

void onOpen(
    Server* server,
    ServerState* state,
    ConnectionHandle connection)
{
    state->connections.insert(
        connection
    );

    std::cout
        << "[SERVER] Client connected. Total: "
        << state->connections.size()
        << std::endl;

    broadcastGameState(
        server,
        state
    );

    sendSelectionState(
        server,
        state,
        connection
    );
}

void onClose(
    ServerState* state,
    ConnectionHandle connection)
{
    state->connections.erase(
        connection
    );

    std::cout
        << "[SERVER] Client disconnected. Total: "
        << state->connections.size()
        << std::endl;
}

void onMessage(
    Server* server,
    ServerState* state,
    ConnectionHandle connection,
    Server::message_ptr message)
{
    try
    {
        const Json request =
            Json::parse(
                message->get_payload()
            );

        const std::string messageType =
            request.value(
                "messageType",
                std::string{}
            );

        if (messageType !=
            "ClickRequest")
        {
            std::cout
                << "[SERVER] Unsupported message: "
                << messageType
                << std::endl;

            return;
        }

        const int x =
            request.at("x")
                   .get<int>();

        const int y =
            request.at("y")
                   .get<int>();

        const std::string button =
            request.at("button")
                   .get<std::string>();

        if (x < 0 || y < 0)
        {
            std::cout
                << "[SERVER] Invalid coordinates."
                << std::endl;

            return;
        }

        if (button == "left")
        {
            state->controller.click(
                x,
                y
            );
        }
        else if (button == "right")
        {
            state->controller.jump(
                x,
                y
            );
        }
        else
        {
            std::cout
                << "[SERVER] Unknown button."
                << std::endl;

            return;
        }

        std::cout
            << "[SERVER] Click received: "
            << button
            << " ("
            << x
            << ", "
            << y
            << ")"
            << std::endl;

        // שליחת מצב הבחירה ללקוח
        sendSelectionState(
            server,
            state,
            connection
        );

        // שליחת מצב המשחק המעודכן
        broadcastGameState(
            server,
            state
        );
    }
    catch (
        const Json::exception& exception
    )
    {
        std::cerr
            << "[SERVER] Invalid JSON: "
            << exception.what()
            << std::endl;
    }
}

void scheduleGameTick(
    Server* server,
    ServerState* state)
{
    server->set_timer(
        GAME_TICK_MS,
        [server, state](
            const websocketpp::lib::error_code&
                error)
        {
            if (error)
            {
                return;
            }

            state->engine.wait(
                GAME_TICK_MS
            );

            broadcastGameState(
                server,
                state
            );

            scheduleGameTick(
                server,
                state
            );
        }
    );
}

int main()
{
    try
    {
        ServerState state;
        Server server;

        // ביטול הודעות מעקב פנימיות
        server.clear_access_channels(
            websocketpp::log::alevel::all
        );

        server.init_asio();
        server.set_reuse_addr(true);

        server.set_open_handler(
            websocketpp::lib::bind(
                &onOpen,
                &server,
                &state,
                websocketpp::lib::placeholders::_1
            )
        );

        server.set_close_handler(
            websocketpp::lib::bind(
                &onClose,
                &state,
                websocketpp::lib::placeholders::_1
            )
        );

        server.set_message_handler(
            websocketpp::lib::bind(
                &onMessage,
                &server,
                &state,
                websocketpp::lib::placeholders::_1,
                websocketpp::lib::placeholders::_2
            )
        );

        server.listen(
            SERVER_PORT
        );

        server.start_accept();

        scheduleGameTick(
            &server,
            &state
        );

        std::cout
            << "[SERVER] Listening on port "
            << SERVER_PORT
            << std::endl;

        server.run();
    }
    catch (
        const websocketpp::exception&
            exception
    )
    {
        std::cerr
            << "[SERVER ERROR] "
            << exception.what()
            << std::endl;

        return 1;
    }
    catch (
        const std::exception& exception
    )
    {
        std::cerr
            << "[SERVER ERROR] "
            << exception.what()
            << std::endl;

        return 1;
    }

    return 0;
}