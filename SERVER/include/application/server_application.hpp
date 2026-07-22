#pragma once

#include <cstddef>
#include <map>
#include <memory>
#include <string>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "application/client_state.hpp"
#include "auth/auth_service.hpp"
#include "database/databaseManager.hpp"
#include "events/event_bus.hpp"
#include "game/game_session.hpp"
#include "model/piece_removed_info.hpp"
#include "repositories/userRepository.hpp"
#include "scoring/score_service.hpp"

class KungFuChessServerApplication {
private:
    using Server =
        websocketpp::server<
            websocketpp::config::asio
        >;

    using ConnectionHandle =
        websocketpp::connection_hdl;

    static constexpr int GAME_TICK_MS = 20;

    int port;

    DatabaseManager databaseManager;
    UserRepository userRepository;
    AuthService authService;

    Server server;
    GameSession game;

    EventBus<PieceRemovedInfo> pieceRemovedBus;
    ScoreService scoreService;

    std::map<
        ConnectionHandle,
        ClientState,
        std::owner_less<ConnectionHandle>
    > clients;

    void configureServer();

    void onOpen(ConnectionHandle connection);
    void onClose(ConnectionHandle connection);

    void onMessage(
        ConnectionHandle connection,
        Server::message_ptr message
    );

    void handleLoginRequest(
        ConnectionHandle connection,
        const std::string& message
    );

    void handleClickRequest(
        ConnectionHandle connection,
        const std::string& message
    );

    bool isSideAssigned(Color playerColor) const;

    bool isUsernameConnected(
        const std::string& username
    ) const;

    std::size_t loggedInPlayerCount() const;

    void broadcastGameState();

    void sendSelectionState(
        ConnectionHandle connection,
        Color playerColor
    );

    void sendTextMessage(
        ConnectionHandle connection,
        const std::string& message
    );

    void sendLoginRejected(
        ConnectionHandle connection,
        const std::string& reason
    );

    void closeFullConnection(
        ConnectionHandle connection
    );

    void scheduleGameTick();

public:
    KungFuChessServerApplication(
        int port,
        const std::string& databasePath
    );

    int run();

    KungFuChessServerApplication(
        const KungFuChessServerApplication&
    ) = delete;

    KungFuChessServerApplication& operator=(
        const KungFuChessServerApplication&
    ) = delete;
};