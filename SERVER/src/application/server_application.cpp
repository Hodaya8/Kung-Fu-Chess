#include "application/server_application.hpp"

#include <exception>
#include <iostream>
#include <optional>
#include <string>

#include "protocol/json_protocol.hpp"

KungFuChessServerApplication::KungFuChessServerApplication(
    int port)
    : port(port)
{
}

void KungFuChessServerApplication::configureServer()
{
    server.clear_access_channels(
        websocketpp::log::alevel::all
    );

    server.init_asio();
    server.set_reuse_addr(true);

    server.set_open_handler(
        [this](
            ConnectionHandle connection)
        {
            onOpen(connection);
        }
    );

    server.set_close_handler(
        [this](
            ConnectionHandle connection)
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

void KungFuChessServerApplication::onOpen(
    ConnectionHandle connection)
{
    connections.insert(connection);

    std::cout
        << "[SERVER] Client connected. Total: "
        << connections.size()
        << std::endl;

    broadcastGameState();

    sendSelectionState(connection);
}

void KungFuChessServerApplication::onClose(
    ConnectionHandle connection)
{
    connections.erase(connection);

    std::cout
        << "[SERVER] Client disconnected. Total: "
        << connections.size()
        << std::endl;
}

void KungFuChessServerApplication::onMessage(
    ConnectionHandle connection,
    Server::message_ptr message)
{
    try
    {
        const JsonProtocol::ClickRequest
            request =
                JsonProtocol::parseClickRequest(
                    message->get_payload()
                );

        if (
            request.button ==
            JsonProtocol::ClickButton::Left
        )
        {
            game.handleLeftClick(
                request.x,
                request.y
            );
        }
        else
        {
            game.handleRightClick(
                request.x,
                request.y
            );
        }

        std::cout
            << "[SERVER] Click received: ("
            << request.x
            << ", "
            << request.y
            << ")"
            << std::endl;

        sendSelectionState(connection);
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

void KungFuChessServerApplication::broadcastGameState()
{
    if (connections.empty())
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

    for (const auto& connection :
         connections)
    {
        sendTextMessage(
            connection,
            message
        );
    }
}

void KungFuChessServerApplication::sendSelectionState(
    ConnectionHandle connection)
{
    const std::optional<Position> selected =
        game.getSelectedPosition();

    const std::string message =
        JsonProtocol::
            createSelectionStateMessage(
                selected
            );

    sendTextMessage(
        connection,
        message
    );
}

void KungFuChessServerApplication::sendTextMessage(
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

void KungFuChessServerApplication::scheduleGameTick()
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