#pragma once

#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <vector>

#include "game/game_room.hpp"

// Holds the IDs of two matched players ready to start a game
struct MatchResult
{
    int firstUserId = 0;
    int secondUserId = 0;
};

class RoomManager
{
public:
    // Callback alias for operations performed on all active game rooms
    using RoomAction = std::function<void(GameRoom&)>;

    // Default constructor
    RoomManager();

    // Prevent copying the manager to avoid duplicating rooms and ID counters
    RoomManager(const RoomManager&) = delete;
    RoomManager& operator=(const RoomManager&) = delete;

    // Creates a new game room, stores it, and returns a reference to it
    GameRoom& createRoom();

    // Finds an active room by ID; returns nullptr if it does not exist
    GameRoom* findRoom(int roomId);
    const GameRoom* findRoom(int roomId) const;

    // Deletes a room from memory by ID; returns true if successful
    bool removeRoom(int roomId);

    // Executes a callback action across every active game room
    void forEachRoom(const RoomAction& action);

    // Returns the total number of active rooms
    std::size_t getRoomCount() const;

    // Attempts to match a waiting player within +-100 ELO range
    std::optional<MatchResult> addWaitingPlayer(int userId, int rating);

    // Removes a player from the waiting queue (e.g., if they click Cancel)
    void removeWaitingPlayer(int userId);

    // Checks if a specific user is currently in the waiting queue
    bool isWaiting(int userId) const;

    // Returns the total number of players waiting for a match
    std::size_t getWaitingCount() const;

private:
    // Fulfills the college requirement: Maximum allowed ELO difference for matchmaking
    static constexpr int MAX_RATING_DIFFERENCE = 100;

    // Internal structure representing a player in the matchmaking queue
    struct WaitingPlayer
    {
        int userId = 0;
        int rating = 0;
    };

    // Incremental counter to generate unique room IDs
    int nextRoomId = 1;

    // Map storing room IDs as keys and exclusive pointers to rooms as values
    std::map<int, std::unique_ptr<GameRoom>> rooms;

    // Queue of players currently searching for an opponent
    std::vector<WaitingPlayer> waitingPlayers;
};