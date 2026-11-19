#include "network/auth_client.hpp"

#include <array>
#include <sstream>
#include <stdexcept>
#include <system_error>
#include <utility>

#include <asio.hpp>
#include <nlohmann/json.hpp>

using Json = nlohmann::json;

namespace
{
    const int HTTP_OK = 200;
    const int HTTP_BAD_REQUEST = 400;
    const int HTTP_UNAUTHORIZED = 401;

    // Returns the "detail" field from an error response,
    // or the raw body if it is not valid JSON.
    std::string extractErrorDetail(
        const std::string& body)
    {
        try
        {
            const Json json =
                Json::parse(body);

            return json.value(
                "detail",
                body
            );
        }
        catch (const std::exception&)
        {
            return body;
        }
    }
}

AuthClient::AuthClient(
    std::string host,
    std::string port)
    : host(std::move(host)),
      port(std::move(port))
{
}

AuthClient::HttpResponse AuthClient::post(
    const std::string& path,
    const std::string& jsonBody) const
{
    asio::io_context ioContext;

    asio::ip::tcp::resolver resolver(
        ioContext
    );

    asio::ip::tcp::socket socket(
        ioContext
    );

    asio::connect(
        socket,
        resolver.resolve(host, port)
    );

    // Build a minimal HTTP/1.1 POST request.
    std::ostringstream request;

    request
        << "POST " << path << " HTTP/1.1\r\n"
        << "Host: " << host << ":" << port << "\r\n"
        << "Content-Type: application/json\r\n"
        << "Content-Length: " << jsonBody.size() << "\r\n"
        << "Connection: close\r\n"
        << "\r\n"
        << jsonBody;

    asio::write(
        socket,
        asio::buffer(request.str())
    );

    // Read until the server closes the connection.
    std::string response;
    std::array<char, 1024> chunk{};
    asio::error_code error;

    while (true)
    {
        const std::size_t bytesRead =
            socket.read_some(
                asio::buffer(chunk),
                error
            );

        response.append(
            chunk.data(),
            bytesRead
        );

        if (error == asio::error::eof)
        {
            break;
        }

        if (error)
        {
            throw std::system_error(error);
        }
    }

    // Status line looks like: "HTTP/1.1 200 OK"
    const std::size_t firstSpace =
        response.find(' ');

    if (firstSpace == std::string::npos)
    {
        throw std::runtime_error(
            "Invalid HTTP response from API Gateway."
        );
    }

    const int statusCode =
        std::stoi(
            response.substr(firstSpace + 1, 3)
        );

    // The body starts after the first empty line.
    const std::size_t bodyStart =
        response.find("\r\n\r\n");

    std::string body;

    if (bodyStart != std::string::npos)
    {
        body = response.substr(bodyStart + 4);
    }

    return { statusCode, body };
}

AuthResult AuthClient::loginOrRegister(
    const std::string& username,
    const std::string& password) const
{
    const std::string credentials =
        Json{
            { "username", username },
            { "password", password }
        }.dump();

    HttpResponse response =
        post("/api/login", credentials);

    // Unknown user or wrong password:
    // try to register. If the username is taken,
    // the password was wrong.
    if (response.statusCode == HTTP_UNAUTHORIZED)
    {
        const HttpResponse registerResponse =
            post("/api/register", credentials);

        if (registerResponse.statusCode == HTTP_BAD_REQUEST)
        {
            return { false, "", "Incorrect password." };
        }

        if (registerResponse.statusCode != HTTP_OK)
        {
            return {
                false,
                "",
                extractErrorDetail(registerResponse.body)
            };
        }

        response =
            post("/api/login", credentials);
    }

    if (response.statusCode != HTTP_OK)
    {
        return {
            false,
            "",
            extractErrorDetail(response.body)
        };
    }

    const Json json =
        Json::parse(response.body);

    return {
        true,
        json.at("token").get<std::string>(),
        ""
    };
}
