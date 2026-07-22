#pragma once

#include <memory>
#include <set>
#include <string>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "game/game_session.hpp"

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

    std::set<
        ConnectionHandle,
        std::owner_less<ConnectionHandle>
    > connections;

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

    void broadcastGameState();

    void sendSelectionState(
        ConnectionHandle connection
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