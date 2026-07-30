#pragma once
#include <string>
#include <map>
#include <hiredis/hiredis.h>

class RedisManager {
private:
    redisContext* context;
    std::string host;
    int port;

public:
    RedisManager();
    ~RedisManager();

    bool connect();
    bool isConnected() const;

    // Session management
    std::string get(const std::string& key) const;

    // Matchmaking management commands using Redis Hash
    bool setWaitingPlayer(int userId, int rating);
    bool removeWaitingPlayer(int userId);
    
    // Read-only functions (defined as const)
    bool isPlayerWaiting(int userId) const;
    std::size_t getWaitingCount() const;
    std::map<int, int> getAllWaitingPlayers() const;
};