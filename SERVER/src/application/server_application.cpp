#include "application/server_application.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

#include "protocol/json_protocol.hpp"

namespace
{
    std::string authFailureReason(
        AuthStatus status)
    {
        switch (status)
        {
            case AuthStatus::InvalidPassword:
                return "InvalidPassword";

            case AuthStatus::InvalidInput:
                return "InvalidInput";

            case AuthStatus::DatabaseError:
                return "DatabaseError";

            case AuthStatus::Authenticated:
            case AuthStatus::Registered:
                return "Unknown";
        }

        return "Unknown";
    }
}

KungFuChessServerApplication::KungFuChessServerApplication(
    int port,
    const std::string& databasePath)
    : port(port),
      databaseManager(databasePath),
      userRepository(databaseManager),
      authService(userRepository)
{
    // עדכון ניקוד המשחק
    pieceRemovedBus.subscribe(
        [this](const PieceRemovedInfo& removedPiece)
        {
            scoreService.handlePieceRemoved(
                removedPiece
            );
        }
    );

    // טיפול בדירוגים בסיום המשחק
    gameEndedBus.subscribe(
        [this](const Color& winnerColor)
        {
            handleGameEnded(
                winnerColor
            );
        }
    );
}

void KungFuChessServerApplication::configureServer()
{
    server.clear_access_channels(
        websocketpp::log::alevel::all
    );

    server.init_asio();
    server.set_reuse_addr(true);

    server.set_open_handler(
        [this](ConnectionHandle connection)
        {
            onOpen(
                connection
            );
        }
    );

    server.set_close_handler(
        [this](ConnectionHandle connection)
        {
            onClose(
                connection
            );
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
        if (!databaseManager.init())
        {
            return 1;
        }

        if (!userRepository.createTable())
        {
            return 1;
        }

        configureServer();

        server.listen(
            port
        );

        server.start_accept();

        scheduleGameTick();

        std::cout
            << "[SERVER] Listening on port "
            << port
            << std::endl;

        server.run();

        return 0;
    }
    catch (const websocketpp::exception& exception)
    {
        std::cerr
            << "[SERVER ERROR] "
            << exception.what()
            << std::endl;

        return 1;
    }
    catch (const std::exception& exception)
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
    clients.emplace(
        connection,
        ClientState{}
    );

    std::cout
        << "[SERVER] Client connected. "
        << "Waiting for login."
        << std::endl;
}

void KungFuChessServerApplication::onClose(
    ConnectionHandle connection)
{
    const auto client =
        clients.find(
            connection
        );

    if (client == clients.end())
    {
        return;
    }

    const std::string username =
        client->second.username;

    clients.erase(
        client
    );

    std::cout
        << "[SERVER] Client disconnected";

    if (!username.empty())
    {
        std::cout
            << ": "
            << username;
    }

    std::cout
        << ". Active players: "
        << loggedInPlayerCount()
        << std::endl;
}

void KungFuChessServerApplication::onMessage(
    ConnectionHandle connection,
    Server::message_ptr message)
{
    try
    {
        const std::string payload =
            message->get_payload();

        const std::string messageType =
            JsonProtocol::getMessageType(
                payload
            );

        if (messageType == "LoginRequest")
        {
            handleLoginRequest(
                connection,
                payload
            );

            return;
        }

        if (messageType == "ClickRequest")
        {
            handleClickRequest(
                connection,
                payload
            );

            return;
        }

        throw std::invalid_argument(
            "Unknown message type."
        );
    }
    catch (const std::exception& exception)
    {
        std::cerr
            << "[SERVER] Invalid request: "
            << exception.what()
            << std::endl;
    }
}

void KungFuChessServerApplication::handleLoginRequest(
    ConnectionHandle connection,
    const std::string& message)
{
    const auto client =
        clients.find(
            connection
        );

    if (client == clients.end())
    {
        return;
    }

    ClientState& clientState =
        client->second;

    if (clientState.loggedIn)
    {
        sendLoginRejected(
            connection,
            "AlreadyLoggedIn"
        );

        return;
    }

    const JsonProtocol::LoginRequest request =
        JsonProtocol::parseLoginRequest(
            message
        );

    if (loggedInPlayerCount() >= 2)
    {
        closeFullConnection(
            connection
        );

        return;
    }

    if (isUsernameConnected(
            request.username))
    {
        sendLoginRejected(
            connection,
            "AlreadyConnected"
        );

        return;
    }

    const AuthResult authResult =
        authService.loginOrRegister(
            request.username,
            request.password
        );

    if (!authResult.isSuccess())
    {
        sendLoginRejected(
            connection,
            authFailureReason(
                authResult.status
            )
        );

        return;
    }

    const Color playerColor =
        isSideAssigned(Color::WHITE)
            ? Color::BLACK
            : Color::WHITE;

    clientState.userId =
        authResult.user.id;

    clientState.username =
        authResult.user.username;

    clientState.rating =
        authResult.user.rating;

    clientState.playerColor =
        playerColor;

    clientState.loggedIn =
        true;

    const bool registered =
        authResult.status ==
        AuthStatus::Registered;

    sendTextMessage(
        connection,
        JsonProtocol::createLoginAcceptedMessage(
            clientState.username,
            clientState.rating,
            registered
        )
    );

    sendTextMessage(
        connection,
        JsonProtocol::createPlayerAssignedMessage(
            playerColor
        )
    );

    std::cout
        << "[SERVER] User "
        << (
            registered
                ? "registered"
                : "authenticated"
        )
        << ": "
        << clientState.username
        << ", rating "
        << clientState.rating
        << ", side "
        << (
            playerColor == Color::WHITE
                ? "white"
                : "black"
        )
        << std::endl;

    broadcastGameState();

    sendSelectionState(
        connection,
        playerColor
    );
}

void KungFuChessServerApplication::handleClickRequest(
    ConnectionHandle connection,
    const std::string& message)
{
    const auto client =
        clients.find(
            connection
        );

    if (client == clients.end())
    {
        return;
    }

    const ClientState& clientState =
        client->second;

    if (!clientState.loggedIn ||
        !clientState.playerColor.has_value())
    {
        return;
    }

    const JsonProtocol::ClickRequest request =
        JsonProtocol::parseClickRequest(
            message
        );

    const Color playerColor =
        clientState.playerColor.value();

    if (request.button ==
        JsonProtocol::ClickButton::Left)
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

void KungFuChessServerApplication::handleGameEnded(
    Color winnerColor)
{
    ClientState* whitePlayer = nullptr;
    ClientState* blackPlayer = nullptr;

    // מציאת שני השחקנים לפי הצבע שלהם
    for (auto& client : clients)
    {
        ClientState& clientState =
            client.second;

        if (!clientState.loggedIn ||
            !clientState.playerColor.has_value())
        {
            continue;
        }

        if (clientState.playerColor.value() ==
            Color::WHITE)
        {
            whitePlayer =
                &clientState;
        }
        else
        {
            blackPlayer =
                &clientState;
        }
    }

    if (!whitePlayer ||
        !blackPlayer)
    {
        std::cerr
            << "[ELO] Could not update ratings: "
            << "both players must be connected."
            << std::endl;

        return;
    }

    const int previousWhiteRating =
        whitePlayer->rating;

    const int previousBlackRating =
        blackPlayer->rating;

    const EloResult newRatings =
        eloService.calculateRatings(
            previousWhiteRating,
            previousBlackRating,
            winnerColor
        );

    const bool whiteSaved =
        userRepository.updateRating(
            whitePlayer->userId,
            newRatings.whiteRating
        );

    const bool blackSaved =
        userRepository.updateRating(
            blackPlayer->userId,
            newRatings.blackRating
        );

    if (!whiteSaved ||
        !blackSaved)
    {
        std::cerr
            << "[ELO] Could not save "
            << "the new ratings."
            << std::endl;

        return;
    }

    // עדכון הדירוגים בזיכרון השרת
    whitePlayer->rating =
        newRatings.whiteRating;

    blackPlayer->rating =
        newRatings.blackRating;

    std::cout
        << "[GAME END] Winner: "
        << (
            winnerColor == Color::WHITE
                ? "white"
                : "black"
        )
        << std::endl;

    std::cout
        << "[ELO] "
        << whitePlayer->username
        << ": "
        << previousWhiteRating
        << " -> "
        << whitePlayer->rating
        << std::endl;

    std::cout
        << "[ELO] "
        << blackPlayer->username
        << ": "
        << previousBlackRating
        << " -> "
        << blackPlayer->rating
        << std::endl;
}

bool KungFuChessServerApplication::isSideAssigned(
    Color playerColor) const
{
    for (const auto& client : clients)
    {
        const ClientState& clientState =
            client.second;

        if (clientState.loggedIn &&
            clientState.playerColor.has_value() &&
            clientState.playerColor.value() ==
                playerColor)
        {
            return true;
        }
    }

    return false;
}

bool KungFuChessServerApplication::isUsernameConnected(
    const std::string& username) const
{
    for (const auto& client : clients)
    {
        if (client.second.loggedIn &&
            client.second.username ==
                username)
        {
            return true;
        }
    }

    return false;
}

std::size_t
KungFuChessServerApplication::loggedInPlayerCount() const
{
    std::size_t count = 0;

    for (const auto& client : clients)
    {
        if (client.second.loggedIn)
        {
            ++count;
        }
    }

    return count;
}

void KungFuChessServerApplication::broadcastGameState()
{
    if (loggedInPlayerCount() == 0)
    {
        return;
    }

    const GameSnapshot snapshot =
        game.getSnapshot();

    const std::string message =
        JsonProtocol::createGameStateMessage(
            snapshot,
            game.isGameOver(),
            scoreService.getWhiteScore(),
            scoreService.getBlackScore()
        );

    for (const auto& client : clients)
    {
        if (!client.second.loggedIn)
        {
            continue;
        }

        sendTextMessage(
            client.first,
            message
        );
    }
}

void KungFuChessServerApplication::sendSelectionState(
    ConnectionHandle connection,
    Color playerColor)
{
    sendTextMessage(
        connection,
        JsonProtocol::createSelectionStateMessage(
            game.getSelectedPosition(
                playerColor
            )
        )
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

void KungFuChessServerApplication::sendLoginRejected(
    ConnectionHandle connection,
    const std::string& reason)
{
    sendTextMessage(
        connection,
        JsonProtocol::createLoginRejectedMessage(
            reason
        )
    );
}

void KungFuChessServerApplication::closeFullConnection(
    ConnectionHandle connection)
{
    sendTextMessage(
        connection,
        JsonProtocol::createGameFullMessage()
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
            << "full connection: "
            << error.message()
            << std::endl;
    }
}

void KungFuChessServerApplication::scheduleGameTick()
{
    server.set_timer(
        GAME_TICK_MS,
        [this](
            const websocketpp::lib::error_code& error)
        {
            if (error)
            {
                return;
            }

            auto removedPieces =
                game.advanceTime(
                    GAME_TICK_MS
                );

            // פרסום אירועי ניקוד
            for (const auto& removedPiece :
                 removedPieces)
            {
                pieceRemovedBus.publish(
                    removedPiece
                );
            }

            if (!removedPieces.empty())
            {
                std::cout
                    << "[SCORE] White: "
                    << scoreService.getWhiteScore()
                    << " | Black: "
                    << scoreService.getBlackScore()
                    << std::endl;
            }

            // פרסום אירוע סיום פעם אחת בלבד
            if (game.isGameOver() &&
                !gameEndEventPublished)
            {
                const auto winnerColor =
                    game.getWinnerColor();

                if (winnerColor.has_value())
                {
                    gameEndEventPublished =
                        true;

                    gameEndedBus.publish(
                        winnerColor.value()
                    );
                }
            }

            broadcastGameState();

            scheduleGameTick();
        }
    );
}