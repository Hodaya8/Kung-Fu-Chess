#pragma once

#include <atomic>
#include <functional>
#include <string>
#include <thread>

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

class WebSocketClient
{
public:
    using MessageHandler =
        std::function<void(const std::string&)>;

    explicit WebSocketClient(
        MessageHandler messageHandler
    );

    ~WebSocketClient();

    WebSocketClient(
        const WebSocketClient&
    ) = delete;

    WebSocketClient& operator=(
        const WebSocketClient&
    ) = delete;

    bool connect(const std::string& uri);
    bool send(const std::string& message);

    void disconnect();

    bool isConnected() const;

private:
    using Client =
        websocketpp::client<
            websocketpp::config::asio_client
        >;

    Client client;

    websocketpp::connection_hdl connection;

    std::thread networkThread;

    std::atomic<bool> connected;

    MessageHandler messageHandler;
};