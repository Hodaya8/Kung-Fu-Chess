#pragma once

#include <map>
#include <memory>
#include <string>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "game/game_session.hpp"
#include "model/piece.hpp"

class KungFuChessServerApplication
{
private:
    using Server =
        websocketpp::server<
            websocketpp::config::asio
        >;

    using ConnectionHandle =
        websocketpp::connection_hdl;

    static constexpr int GAME_TICK_MS = 20;

    int port;
    Server server;
    GameSession game;

    std::map<
        ConnectionHandle,
        Color,
        std::owner_less<ConnectionHandle>
    > players;

    void configureServer();

    void onOpen(
        ConnectionHandle connection
    );

    void onClose(
        ConnectionHandle connection
    );

    void onMessage(
        ConnectionHandle connection,
        Server::message_ptr message
    );

    bool isSideAssigned(
        Color playerColor
    ) const;

    void broadcastGameState();

    void sendSelectionState(
        ConnectionHandle connection,
        Color playerColor
    );

    void sendTextMessage(
        ConnectionHandle connection,
        const std::string& message
    );

    void scheduleGameTick();

public:
    explicit KungFuChessServerApplication(
        int port
    );

    int run();

    KungFuChessServerApplication(
        const KungFuChessServerApplication&
    ) = delete;

    KungFuChessServerApplication& operator=(
        const KungFuChessServerApplication&
    ) = delete;
};