#include "application/server_application.hpp"

#include <exception>
#include <iostream>
#include <string>

#include "protocol/json_protocol.hpp"

KungFuChessServerApplication::
    KungFuChessServerApplication(
        int port)
    : port(port)
{
}

void KungFuChessServerApplication::
    configureServer()
{
    server.clear_access_channels(
        websocketpp::log::alevel::all
    );

    server.init_asio();
    server.set_reuse_addr(true);

    server.set_open_handler(
        [this](ConnectionHandle connection)
        {
            onOpen(connection);
        }
    );

    server.set_close_handler(
        [this](ConnectionHandle connection)
        {
            onClose(connection);
        }
    );

    server.set_message_handler(
        [this](
            ConnectionHandle connection,
            Server::message_ptr message)
        {
            onMessage(
                connection,
                message
            );
        }
    );
}

int KungFuChessServerApplication::run()
{
    try
    {
        configureServer();

        server.listen(port);
        server.start_accept();

        scheduleGameTick();

        std::cout
            << "[SERVER] Listening on port "
            << port
            << std::endl;

        server.run();

        return 0;
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
}

bool KungFuChessServerApplication::
    isSideAssigned(
        Color playerColor) const
{
    for (const auto& player : players)
    {
        if (player.second == playerColor)
        {
            return true;
        }
    }

    return false;
}

void KungFuChessServerApplication::onOpen(
    ConnectionHandle connection)
{
    if (players.size() >= 2)
    {
        sendTextMessage(
            connection,
            JsonProtocol::
                createGameFullMessage()
        );

        websocketpp::lib::error_code error;

        server.close(
            connection,
            websocketpp::close::status::normal,
            "Game is full.",
            error
        );

        if (error)
        {
            std::cerr
                << "[SERVER] Failed to close "
                << "extra connection: "
                << error.message()
                << std::endl;
        }

        return;
    }

    const Color playerColor =
        isSideAssigned(Color::WHITE)
            ? Color::BLACK
            : Color::WHITE;

    players.emplace(
        connection,
        playerColor
    );

    sendTextMessage(
        connection,
        JsonProtocol::
            createPlayerAssignedMessage(
                playerColor
            )
    );

    std::cout
        << "[SERVER] Player connected as "
        << (
            playerColor == Color::WHITE
                ? "white"
                : "black"
        )
        << ". Total players: "
        << players.size()
        << std::endl;

    broadcastGameState();

    sendSelectionState(
        connection,
        playerColor
    );
}

void KungFuChessServerApplication::onClose(
    ConnectionHandle connection)
{
    const std::size_t removed =
        players.erase(connection);

    if (removed == 0)
    {
        return;
    }

    std::cout
        << "[SERVER] Player disconnected. Total: "
        << players.size()
        << std::endl;
}

void KungFuChessServerApplication::onMessage(
    ConnectionHandle connection,
    Server::message_ptr message)
{
    const auto player =
        players.find(connection);

    if (player == players.end())
    {
        return;
    }

    try
    {
        const JsonProtocol::ClickRequest request =
            JsonProtocol::parseClickRequest(
                message->get_payload()
            );

        const Color playerColor =
            player->second;

        if (
            request.button ==
            JsonProtocol::ClickButton::Left
        )
        {
            game.handleLeftClick(
                playerColor,
                request.x,
                request.y
            );
        }
        else
        {
            game.handleRightClick(
                playerColor,
                request.x,
                request.y
            );
        }

        sendSelectionState(
            connection,
            playerColor
        );

        broadcastGameState();
    }
    catch (
        const std::exception& exception
    )
    {
        std::cerr
            << "[SERVER] Invalid request: "
            << exception.what()
            << std::endl;
    }
}

void KungFuChessServerApplication::
    broadcastGameState()
{
    if (players.empty())
    {
        return;
    }

    const GameSnapshot snapshot =
        game.getSnapshot();

    const std::string message =
        JsonProtocol::createGameStateMessage(
            snapshot,
            game.isGameOver()
        );

    for (const auto& player : players)
    {
        sendTextMessage(
            player.first,
            message
        );
    }
}

void KungFuChessServerApplication::
    sendSelectionState(
        ConnectionHandle connection,
        Color playerColor)
{
    const std::string message =
        JsonProtocol::
            createSelectionStateMessage(
                game.getSelectedPosition(
                    playerColor
                )
            );

    sendTextMessage(
        connection,
        message
    );
}

void KungFuChessServerApplication::
    sendTextMessage(
        ConnectionHandle connection,
        const std::string& message)
{
    websocketpp::lib::error_code error;

    server.send(
        connection,
        message,
        websocketpp::frame::opcode::text,
        error
    );

    if (error)
    {
        std::cerr
            << "[SERVER] Failed to send message: "
            << error.message()
            << std::endl;
    }
}

void KungFuChessServerApplication::
    scheduleGameTick()
{
    server.set_timer(
        GAME_TICK_MS,
        [this](
            const websocketpp::lib::error_code&
                error)
        {
            if (error)
            {
                return;
            }

            game.advanceTime(
                GAME_TICK_MS
            );

            broadcastGameState();

            scheduleGameTick();
        }
    );
}