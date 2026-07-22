#include "network/websocket_client.hpp"

#include <iostream>
#include <utility>

WebSocketClient::WebSocketClient(
    MessageHandler messageHandler)
    : connected(false),
      messageHandler(std::move(messageHandler))
{
    client.clear_access_channels(
        websocketpp::log::alevel::all
    );

    client.init_asio();

    client.set_open_handler(
        [this](
            websocketpp::connection_hdl handle)
        {
            connection = handle;
            connected = true;

            std::cout
                << "[UI] Connected to server."
                << std::endl;
        }
    );

    client.set_message_handler(
        [this](
            websocketpp::connection_hdl,
            Client::message_ptr message)
        {
            if (this->messageHandler)
            {
                this->messageHandler(
                    message->get_payload()
                );
            }
        }
    );

    client.set_close_handler(
        [this](
            websocketpp::connection_hdl)
        {
            connected = false;

            std::cout
                << "[UI] Disconnected from server."
                << std::endl;
        }
    );

    client.set_fail_handler(
        [this](
            websocketpp::connection_hdl handle)
        {
            connected = false;

            websocketpp::lib::error_code error;

            auto connectionPointer =
                client.get_con_from_hdl(
                    handle,
                    error
                );

            std::cerr
                << "[UI] Failed to connect to server";

            if (!error && connectionPointer)
            {
                std::cerr
                    << ": "
                    << connectionPointer
                           ->get_ec()
                           .message();
            }

            std::cerr << std::endl;
        }
    );
}

WebSocketClient::~WebSocketClient()
{
    disconnect();
}

bool WebSocketClient::connect(
    const std::string& uri)
{
    if (networkThread.joinable())
    {
        std::cerr
            << "[UI] WebSocket client is already running."
            << std::endl;

        return false;
    }

    websocketpp::lib::error_code error;

    Client::connection_ptr connectionPointer =
        client.get_connection(
            uri,
            error
        );

    if (error)
    {
        std::cerr
            << "[UI] Could not create connection: "
            << error.message()
            << std::endl;

        return false;
    }

    connection =
        connectionPointer->get_handle();

    client.connect(connectionPointer);

    networkThread =
        std::thread(
            [this]()
            {
                try
                {
                    client.run();
                }
                catch (
                    const std::exception& exception)
                {
                    std::cerr
                        << "[UI] WebSocket error: "
                        << exception.what()
                        << std::endl;
                }
            }
        );

    return true;
}

bool WebSocketClient::send(
    const std::string& message)
{
    if (!connected)
    {
        std::cerr
            << "[UI] Cannot send: not connected."
            << std::endl;

        return false;
    }

    websocketpp::lib::error_code error;

    client.send(
        connection,
        message,
        websocketpp::frame::opcode::text,
        error
    );

    if (error)
    {
        std::cerr
            << "[UI] Failed to send message: "
            << error.message()
            << std::endl;

        return false;
    }

    return true;
}

void WebSocketClient::disconnect()
{
    if (connected)
    {
        websocketpp::lib::error_code error;

        client.close(
            connection,
            websocketpp::close::status::normal,
            "UI closed",
            error
        );

        if (error)
        {
            std::cerr
                << "[UI] Failed to close connection: "
                << error.message()
                << std::endl;
        }
    }

    connected = false;

    // גורם ללולאת הרשת להסתיים
    client.stop();

    if (networkThread.joinable())
    {
        networkThread.join();
    }
}

bool WebSocketClient::isConnected() const
{
    return connected;
}