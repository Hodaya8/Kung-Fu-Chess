#include "game/room_manager.hpp"

#include <algorithm>
#include <cstdlib>
#include <utility>

RoomManager::RoomManager()
    : nextRoomId(1)
{
    // Connect to Redis as soon as the manager is created in the server
    redis.connect();
}

GameRoom& RoomManager::createRoom()
{
    // Generate ID and increment for the next room
    const int roomId = nextRoomId++;

    // Create room securely on the heap using a smart pointer
    auto room = std::make_unique<GameRoom>(roomId);

    // Keep a reference before moving ownership into the map
    GameRoom& createdRoom = *room;
    
    //inserting the roon into the the rooms map
    rooms.emplace(roomId, std::move(room));

    return createdRoom;
}

GameRoom* RoomManager::findRoom(int roomId)
{
    const auto room = rooms.find(roomId);

    if (room == rooms.end())
    {
        return nullptr;
    }

    // .get() returns the raw pointer without transferring ownership
    return room->second.get();
}

const GameRoom* RoomManager::findRoom(int roomId) const
{
    const auto room = rooms.find(roomId);

    if (room == rooms.end())
    {
        return nullptr;
    }

    return room->second.get();
}

bool RoomManager::removeRoom(int roomId)
{
    // erase() returns the number of removed elements (1 if found, 0 if not)
    return rooms.erase(roomId) > 0;
}

void RoomManager::forEachRoom(const RoomAction& action)
{
    if (!action)
    {
        return;
    }

    // Execute the callback action on every active room
    for (auto& room : rooms)
    {
        action(*room.second);
    }
}

std::size_t RoomManager::getRoomCount() const
{
    return rooms.size();
}

std::optional<MatchResult> RoomManager::addWaitingPlayer(int userId, int rating)
{
    // Prevent the same user from queuing multiple times
    if (isWaiting(userId))
    {
        return std::nullopt;
    }

    // Fetch all waiting players from Redis
    auto waitingPlayers = redis.getAllWaitingPlayers();

    // Search for an ELO match from the retrieved data
    const auto matchingPlayer = std::find_if(
        waitingPlayers.begin(),
        waitingPlayers.end(),
        [rating](const std::pair<const int, int>& player)
        {
            const int difference = std::abs(player.second - rating);
            return difference <= MAX_RATING_DIFFERENCE;
        }
    );

    // No match found - the player is pushed to Redis
    if (matchingPlayer == waitingPlayers.end())
    {
        redis.setWaitingPlayer(userId, rating);
        return std::nullopt;
    }

    // Match found! Remove the opponent from Redis and return the pair
    const MatchResult result{matchingPlayer->first, userId};
    redis.removeWaitingPlayer(matchingPlayer->first);

    return result;
}

void RoomManager::removeWaitingPlayer(int userId)
{
    // Direct deletion from the Hash
    redis.removeWaitingPlayer(userId);
}

bool RoomManager::isWaiting(int userId) const
{
    // Direct check against the Hash
    return redis.isPlayerWaiting(userId);
}

std::size_t RoomManager::getWaitingCount() const
{
    return redis.getWaitingCount();
}