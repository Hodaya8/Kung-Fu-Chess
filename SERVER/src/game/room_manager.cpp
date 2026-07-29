#include "game/room_manager.hpp"

#include <algorithm>
#include <cstdlib>
#include <utility>

RoomManager::RoomManager()
    : nextRoomId(1)
{
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

    //Find a player within +-100 ELO range
    const auto matchingPlayer = std::find_if(
        waitingPlayers.begin(),
        waitingPlayers.end(),
        [rating](const WaitingPlayer& player)
        {
            const int difference = std::abs(player.rating - rating);
            return difference <= MAX_RATING_DIFFERENCE;
        }
    );

    // If nobody is compatible, join the waiting queue
    if (matchingPlayer == waitingPlayers.end())
    {
        waitingPlayers.push_back({userId, rating});
        return std::nullopt;
    }

    // A match was found- Pair them up and remove the opponent from the queue
    const MatchResult result{matchingPlayer->userId, userId};
    waitingPlayers.erase(matchingPlayer);

    return result;
}

void RoomManager::removeWaitingPlayer(int userId)
{
    auto it = std::remove_if(
        waitingPlayers.begin(),
        waitingPlayers.end(),
        [userId](const WaitingPlayer& player)
        {
            return player.userId == userId;
        }
    );

    waitingPlayers.erase(it, waitingPlayers.end());
}

bool RoomManager::isWaiting(int userId) const
{
    // Check if any player in the queue has this userId
    return std::any_of(
        waitingPlayers.begin(),
        waitingPlayers.end(),
        [userId](const WaitingPlayer& player)
        {
            return player.userId == userId;
        }
    );
}

std::size_t RoomManager::getWaitingCount() const
{
    return waitingPlayers.size();
}