#include "database/redis_manager.hpp"
#include <cstdlib>
#include <iostream>

RedisManager::RedisManager() : context(nullptr) {
    const char* envHost = std::getenv("REDIS_HOST");
    const char* envPort = std::getenv("REDIS_PORT");
    
    host = envHost ? envHost : "localhost";
    port = envPort ? std::stoi(envPort) : 6379;
}

RedisManager::~RedisManager() {
    if (context) {
        redisFree(context);
    }
}

bool RedisManager::connect() {
    context = redisConnect(host.c_str(), port);
    
    if (context == nullptr || context->err) {
        if (context) {
            std::cerr << "[REDIS ERROR] Connection error: " << context->errstr << std::endl;
            redisFree(context);
            context = nullptr;
        } else {
            std::cerr << "[REDIS ERROR] Connection error: can't allocate redis context" << std::endl;
        }
        return false;
    }
    
    std::cout << "[REDIS] Successfully connected to Redis at " << host << ":" << port << "!" << std::endl;
    return true;
}

bool RedisManager::isConnected() const {
    return context != nullptr && !context->err;
}

bool RedisManager::setWaitingPlayer(int userId, int rating) {
    if (!isConnected()) return false;
    redisReply* reply = (redisReply*)redisCommand(context, "HSET matchmaking_queue %d %d", userId, rating);
    bool success = (reply != nullptr && reply->type != REDIS_REPLY_ERROR);
    if (reply) freeReplyObject(reply);
    return success;
}

bool RedisManager::removeWaitingPlayer(int userId) {
    if (!isConnected()) return false;
    redisReply* reply = (redisReply*)redisCommand(context, "HDEL matchmaking_queue %d", userId);
    bool success = (reply != nullptr && reply->type != REDIS_REPLY_ERROR);
    if (reply) freeReplyObject(reply);
    return success;
}

bool RedisManager::isPlayerWaiting(int userId) const {
    if (!isConnected()) return false;
    redisReply* reply = (redisReply*)redisCommand(context, "HEXISTS matchmaking_queue %d", userId);
    bool exists = (reply != nullptr && reply->type == REDIS_REPLY_INTEGER && reply->integer == 1);
    if (reply) freeReplyObject(reply);
    return exists;
}

std::size_t RedisManager::getWaitingCount() const {
    if (!isConnected()) return 0;
    redisReply* reply = (redisReply*)redisCommand(context, "HLEN matchmaking_queue");
    std::size_t count = 0;
    if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER) {
        count = reply->integer;
    }
    if (reply) freeReplyObject(reply);
    return count;
}

std::map<int, int> RedisManager::getAllWaitingPlayers() const {
    std::map<int, int> players;
    if (!isConnected()) return players;

    // HGETALL returns all keys and values in the Hash as a flat array
    redisReply* reply = (redisReply*)redisCommand(context, "HGETALL matchmaking_queue");
    if (reply != nullptr && reply->type == REDIS_REPLY_ARRAY) {
        for (size_t i = 0; i < reply->elements; i += 2) {
            if (reply->element[i]->type == REDIS_REPLY_STRING && reply->element[i+1]->type == REDIS_REPLY_STRING) {
                int id = std::stoi(reply->element[i]->str);
                int r = std::stoi(reply->element[i+1]->str);
                players[id] = r;
            }
        }
    }
    if (reply) freeReplyObject(reply);
    return players;
}

std::string RedisManager::get(const std::string& key) const
{
    if (!context)
    {
        return "";
    }

    redisReply* reply = (redisReply*)redisCommand(context, "GET %s", key.c_str());
    std::string result = "";

    if (reply)
    {
        if (reply->type == REDIS_REPLY_STRING)
        {
            result = reply->str;
        }
        freeReplyObject(reply);
    }

    return result;
}