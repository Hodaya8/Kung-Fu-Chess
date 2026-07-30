#pragma once

#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <functional> // For std::hash

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "application/client_state.hpp"
#include "database/redis_manager.hpp"
#include "events/event_bus.hpp"
#include "game/room_manager.hpp"
#include "rating/elo_service.hpp"

class KungFuChessServerApplication
{
private:
    // WebSocket++ type aliases for clean reading
    using Server = websocketpp::server<websocketpp::config::asio>;
    using ConnectionHandle = websocketpp::connection_hdl;

    // Maps a raw socket connection to our internal client session state
    using ClientMap = std::map<ConnectionHandle, ClientState, std::owner_less<ConnectionHandle>>;

    // Internal event fired when a user requests to enter the matchmaking queue
    struct FindMatchRequestedEvent
    {
        int userId = 0;
        int rating = 0;
    };

    // Fulfills college requirement: Real-time game loop runs every 20ms (50 FPS)
    static constexpr int GAME_TICK_MS = 20;

    int port = 9000;

    // Core infrastructure services
    RedisManager redisManager;
    Server server;
    RoomManager roomManager;

    // Event buses for async matchmaking notifications
    EventBus<FindMatchRequestedEvent> findMatchRequestedBus;
    EventBus<MatchResult> matchFoundBus;

    // Active connected clients repository
    ClientMap clients;

    // Server configuration and networking lifecycle
    void configureServer();
    void onOpen(ConnectionHandle connection);
    void onClose(ConnectionHandle connection);
    void onMessage(ConnectionHandle connection, Server::message_ptr message);

    // Incoming network request handlers
    void handleLoginRequest(ConnectionHandle connection, const std::string& message);
    void handleFindMatchRequest(ConnectionHandle connection);
    void handleMoveRequest(ConnectionHandle connection, const std::string& message);
    void handleJumpRequest(ConnectionHandle connection, const std::string& message);

    // Internal event bus handlers
    void handleFindMatchRequested(const FindMatchRequestedEvent& event);
    void handleMatchFound(const MatchResult& match);
    void handleGameEnded(int roomId, Color winnerColor);

    // Lookup helpers and telemetry
    ClientMap::iterator findClientByUserId(int userId);
    bool isUsernameConnected(const std::string& username) const;
    std::size_t loggedInPlayerCount() const;

    // Outgoing communication helpers
    void broadcastGameState(int roomId);
    void sendTextMessage(ConnectionHandle connection, const std::string& message);
    void sendLoginRejected(ConnectionHandle connection, const std::string& reason);

    // Real-time engine clock
    void scheduleGameTick();

public:
    // Constructor updated to remove database path parameter
    KungFuChessServerApplication(int port);

    // Starts Redis, binds to the network port, and blocks running the server loop
    int run();

    // Prevent copying the entire server application
    KungFuChessServerApplication(const KungFuChessServerApplication&) = delete;
    KungFuChessServerApplication& operator=(const KungFuChessServerApplication&) = delete;
};