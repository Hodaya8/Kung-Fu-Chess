#include "application/server_application.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

#include "protocol/json_protocol.hpp"

namespace
{
    //Converts internal authentication enum errors to readable protocol strings
    std::string authFailureReason(AuthStatus status)
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

//constructor
KungFuChessServerApplication::KungFuChessServerApplication(int port, const std::string& databasePath)
    : port(port),
      databaseManager(databasePath),
      userRepository(databaseManager),
      authService(userRepository)
{
    // Wire up internal event buses using lambdas
    findMatchRequestedBus.subscribe(
        [this](const FindMatchRequestedEvent& event)
        {
            handleFindMatchRequested(event);
        }
    );

    matchFoundBus.subscribe(
        [this](const MatchResult& match)
        {
            handleMatchFound(match);
        }
    );
}

void KungFuChessServerApplication::configureServer()
{
    // Silence verbose console logging from websocketpp
    server.clear_access_channels(websocketpp::log::alevel::all);
    server.init_asio();
    server.set_reuse_addr(true);

    // Bind network lifecycle events to our class methods
    server.set_open_handler([this](ConnectionHandle hdl) { onOpen(hdl); });
    server.set_close_handler([this](ConnectionHandle hdl) { onClose(hdl); });
    server.set_message_handler([this](ConnectionHandle hdl, Server::message_ptr msg) { onMessage(hdl, msg); });
}

int KungFuChessServerApplication::run()
{
    try
    {
        // 1. Initialize SQLite Database
        if (!databaseManager.init() || !userRepository.createTable())
        {
            std::cerr << "[SERVER ERROR] Failed to initialize database." << std::endl;
            return 1;
        }

        // 2. Configure and bind WebSocket server
        configureServer();
        server.listen(port);
        server.start_accept();

        // 3. Start the 20ms game engine clock
        scheduleGameTick();

        std::cout << "[SERVER] KungFu Chess Server listening on port " << port << std::endl;

        // 4. Block and run the asynchronous event loop
        server.run();
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "[SERVER FATAL] " << exception.what() << std::endl;
        return 1;
    }
}

void KungFuChessServerApplication::onOpen(ConnectionHandle connection)
{
    // Register new connection with empty/unauthenticated client state
    clients.emplace(connection, ClientState{});
    std::cout << "[SERVER] New client connected. Waiting for authentication." << std::endl;
}

void KungFuChessServerApplication::onClose(ConnectionHandle connection)
{
    const auto client = clients.find(connection);
    if (client == clients.end())
    {
        return;
    }

    const std::string username = client->second.username;

    // If player was queuing for a match, remove them immediately
    if (client->second.loggedIn)
    {
        roomManager.removeWaitingPlayer(client->second.userId);
    }

    clients.erase(client);

    std::cout << "[SERVER] Client disconnected" << (!username.empty() ? ": " + username : "") 
              << ". Active logged-in users: " << loggedInPlayerCount() << std::endl;
}

void KungFuChessServerApplication::onMessage(ConnectionHandle connection, Server::message_ptr message)
{
    try
    {
        const std::string payload = message->get_payload();
        const std::string messageType = JsonProtocol::getMessageType(payload);

        // Route incoming network message to the appropriate handler
        if (messageType == "LoginRequest")
        {
            handleLoginRequest(connection, payload);
        }
        else if (messageType == "FindMatchRequest")
        {
            handleFindMatchRequest(connection);
        }
        else if (messageType == "MoveRequest")
        {
            handleMoveRequest(connection, payload);
        }
        else if (messageType == "JumpRequest")
        {
            handleJumpRequest(connection, payload);
        }
        else
        {
            throw std::invalid_argument("Unknown messageType: " + messageType);
        }
    }
    catch (const std::exception& exception)
    {
        std::cerr << "[SERVER WARN] Malformed client message: " << exception.what() << std::endl;
    }
}

void KungFuChessServerApplication::handleLoginRequest(ConnectionHandle connection, const std::string& message)
{
    const auto client = clients.find(connection);
    if (client == clients.end())
    {
        return;
    }

    ClientState& clientState = client->second;

    if (clientState.loggedIn)
    {
        sendLoginRejected(connection, "AlreadyLoggedIn");
        return;
    }

    const JsonProtocol::LoginRequest request = JsonProtocol::parseLoginRequest(message);

    // Prevent double logins from the same account
    if (isUsernameConnected(request.username))
    {
        sendLoginRejected(connection, "AlreadyConnected");
        return;
    }

    const AuthResult authResult = authService.loginOrRegister(request.username, request.password);

    if (!authResult.isSuccess())
    {
        sendLoginRejected(connection, authFailureReason(authResult.status));
        return;
    }

    // Populate session state
    clientState.userId = authResult.user.id;
    clientState.username = authResult.user.username;
    clientState.rating = authResult.user.rating;
    clientState.roomId.reset();
    clientState.playerColor.reset();
    clientState.loggedIn = true;

    const bool registered = (authResult.status == AuthStatus::Registered);

    sendTextMessage(
        connection,
        JsonProtocol::createLoginAcceptedMessage(clientState.username, clientState.rating, registered)
    );

    std::cout << "[AUTH] User " << (registered ? "registered" : "logged in") 
              << ": " << clientState.username << " (ELO: " << clientState.rating << ")" << std::endl;
}

void KungFuChessServerApplication::handleFindMatchRequest(ConnectionHandle connection)
{
    const auto client = clients.find(connection);
    if (client == clients.end())
    {
        return;
    }

    const ClientState& clientState = client->second;

    // Ignore if unauthenticated or already playing in a room
    if (!clientState.loggedIn || clientState.isInRoom())
    {
        return;
    }

    if (roomManager.isWaiting(clientState.userId))
    {
        sendTextMessage(connection, JsonProtocol::createWaitingForMatchMessage());
        return;
    }

    // Publish async matchmaking request
    findMatchRequestedBus.publish({clientState.userId, clientState.rating});
}

void KungFuChessServerApplication::handleFindMatchRequested(const FindMatchRequestedEvent& event)
{
    // Attempt to match with existing waiting players (+-100 ELO)
    const auto match = roomManager.addWaitingPlayer(event.userId, event.rating);

    if (match.has_value())
    {
        matchFoundBus.publish(match.value());
        return;
    }

    const auto client = findClientByUserId(event.userId);
    if (client == clients.end())
    {
        return;
    }

    sendTextMessage(client->first, JsonProtocol::createWaitingForMatchMessage());
    std::cout << "[MATCHMAKING] " << client->second.username << " queued. Rating: " << client->second.rating << std::endl;
}

void KungFuChessServerApplication::handleMatchFound(const MatchResult& match)
{
    const auto firstClient = findClientByUserId(match.firstUserId);
    const auto secondClient = findClientByUserId(match.secondUserId);

    if (firstClient == clients.end() || secondClient == clients.end())
    {
        return;
    }

    // Security check: Verify both players are available
    if (!firstClient->second.loggedIn || !secondClient->second.loggedIn ||
        firstClient->second.isInRoom() || secondClient->second.isInRoom())
    {
        return;
    }

    // Create a new isolated game room
    GameRoom& room = roomManager.createRoom();
    const int roomId = room.getId();

    // Listen for game-over event in this room
    room.subscribeToGameEnded(
        [this, roomId](const Color& winnerColor)
        {
            handleGameEnded(roomId, winnerColor);
        }
    );

    // Assign colors and update client states
    firstClient->second.roomId = roomId;
    firstClient->second.playerColor = Color::WHITE;

    secondClient->second.roomId = roomId;
    secondClient->second.playerColor = Color::BLACK;

    sendTextMessage(firstClient->first, JsonProtocol::createMatchFoundMessage(roomId, Color::WHITE));
    sendTextMessage(secondClient->first, JsonProtocol::createMatchFoundMessage(roomId, Color::BLACK));

    std::cout << "[ROOM " << roomId << "] Created match: " << firstClient->second.username 
              << " (White) vs " << secondClient->second.username << " (Black)" << std::endl;

    broadcastGameState(roomId);
}

void KungFuChessServerApplication::handleMoveRequest(ConnectionHandle connection, const std::string& message)
{
    const auto client = clients.find(connection);
    if (client == clients.end() || !client->second.isPlayer())
    {
        return;
    }

    GameRoom* room = roomManager.findRoom(client->second.roomId.value());
    if (!room)
    {
        return;
    }

    const JsonProtocol::MoveRequest request = JsonProtocol::parseMoveRequest(message);
    room->requestMove(client->second.playerColor.value(), request.source, request.destination);
    broadcastGameState(room->getId());
}

void KungFuChessServerApplication::handleJumpRequest(ConnectionHandle connection, const std::string& message)
{
    const auto client = clients.find(connection);
    if (client == clients.end() || !client->second.isPlayer())
    {
        return;
    }

    GameRoom* room = roomManager.findRoom(client->second.roomId.value());
    if (!room)
    {
        return;
    }

    const JsonProtocol::JumpRequest request = JsonProtocol::parseJumpRequest(message);
    room->requestJump(client->second.playerColor.value(), request.cell);
    broadcastGameState(room->getId());
}

void KungFuChessServerApplication::handleGameEnded(int roomId, Color winnerColor)
{
    ClientState* whitePlayer = nullptr;
    ClientState* blackPlayer = nullptr;

    // Locate the two participating players in the client map
    for (auto& client : clients)
    {
        ClientState& clientState = client.second;
        if (!clientState.isPlayer() || clientState.roomId.value() != roomId)
        {
            continue;
        }

        if (clientState.playerColor.value() == Color::WHITE)
        {
            whitePlayer = &clientState;
        }
        else
        {
            blackPlayer = &clientState;
        }
    }

    if (!whitePlayer || !blackPlayer)
    {
        std::cerr << "[ROOM " << roomId << "] Error: Could not locate players to update ELO." << std::endl;
        return;
    }

    const int prevWhiteRating = whitePlayer->rating;
    const int prevBlackRating = blackPlayer->rating;

    // Calculate new ELO ratings mathematically
    const EloResult newRatings = eloService.calculateRatings(prevWhiteRating, prevBlackRating, winnerColor);

    // Persist new ratings to SQLite database
    const bool whiteSaved = userRepository.updateRating(whitePlayer->userId, newRatings.whiteRating);
    const bool blackSaved = userRepository.updateRating(blackPlayer->userId, newRatings.blackRating);

    if (!whiteSaved || !blackSaved)
    {
        std::cerr << "[ROOM " << roomId << "] Error: Failed to save updated ELO to database." << std::endl;
        return;
    }

    whitePlayer->rating = newRatings.whiteRating;
    blackPlayer->rating = newRatings.blackRating;

    std::cout << "[ROOM " << roomId << "] Game Ended. ELO Updates:\n"
              << "  -> White (" << whitePlayer->username << "): " << prevWhiteRating << " -> " << whitePlayer->rating << "\n"
              << "  -> Black (" << blackPlayer->username << "): " << prevBlackRating << " -> " << blackPlayer->rating << std::endl;
}

KungFuChessServerApplication::ClientMap::iterator KungFuChessServerApplication::findClientByUserId(int userId)
{
    for (auto it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second.loggedIn && it->second.userId == userId)
        {
            return it;
        }
    }
    return clients.end();
}

bool KungFuChessServerApplication::isUsernameConnected(const std::string& username) const
{
    for (const auto& client : clients)
    {
        if (client.second.loggedIn && client.second.username == username)
        {
            return true;
        }
    }
    return false;
}

std::size_t KungFuChessServerApplication::loggedInPlayerCount() const
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

void KungFuChessServerApplication::broadcastGameState(int roomId)
{
    GameRoom* room = roomManager.findRoom(roomId);
    if (!room)
    {
        return;
    }

    const GameSnapshot snapshot = room->getSnapshot();
    const std::string message = JsonProtocol::createGameStateMessage(
        snapshot, room->isGameOver(), room->getWhiteScore(), room->getBlackScore()
    );

    // Broadcast snapshot to all players and spectators viewing this room
    for (const auto& client : clients)
    {
        const ClientState& clientState = client.second;
        if (clientState.loggedIn && clientState.roomId.has_value() && clientState.roomId.value() == roomId)
        {
            sendTextMessage(client.first, message);
        }
    }
}

void KungFuChessServerApplication::sendTextMessage(ConnectionHandle connection, const std::string& message)
{
    websocketpp::lib::error_code error;
    server.send(connection, message, websocketpp::frame::opcode::text, error);

    if (error)
    {
        std::cerr << "[SERVER WARN] Send failed: " << error.message() << std::endl;
    }
}

void KungFuChessServerApplication::sendLoginRejected(ConnectionHandle connection, const std::string& reason)
{
    sendTextMessage(connection, JsonProtocol::createLoginRejectedMessage(reason));
}

void KungFuChessServerApplication::scheduleGameTick()
{
    // Set an asynchronous timer to fire after GAME_TICK_MS (20 milliseconds)
    server.set_timer(
        GAME_TICK_MS,
        [this](const websocketpp::lib::error_code& error)
        {
            if (error)
            {
                return;
            }

            // Advance time for all active game rooms
            roomManager.forEachRoom(
                [this](GameRoom& room)
                {
                    const auto removedPieces = room.advanceTime(GAME_TICK_MS);
                    
                    if (!removedPieces.empty())
                    {
                        std::cout << "[ROOM " << room.getId() << "] Capture! Score -> White: " 
                                  << room.getWhiteScore() << " | Black: " << room.getBlackScore() << std::endl;
                    }

                    broadcastGameState(room.getId());
                }
            );

            // Reschedule the loop for endless real-time execution
            scheduleGameTick();
        }
    );
}