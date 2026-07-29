# Distributed Game Server Architecture - Kung Fu Chess

## Overview
This document outlines the architectural design for the Kung Fu Chess backend. The system transitions from a monolithic C++ server to a distributed, containerized microservices architecture to support real-time multiplayer gameplay, scalability, and high availability.

The architecture strictly separates real-time game commands from administrative actions, ensuring the `GameEngine` remains the authoritative single source of truth.

## Core Architecture Components

### 1. Gateways (Client Entry Points)
*   **API Gateway (REST / HTTP):** Handles all non-real-time actions. Clients connect here for authentication (Login/Register), fetching match history, and querying active rooms.
*   **WebSocket Gateway (WS Gateways):** Manages persistent, live connections with clients. Utilizes asynchronous I/O (no thread per client) to stream real-time state updates and route player moves to the internal network. The Gateway contains zero game logic.

### 2. Internal Communication & State
*   **NATS Event Bus:** The high-performance internal messaging backbone. All microservices communicate purely via NATS (Publish/Subscribe), removing direct coupling between gateways and backend logic.
*   **Redis (Temporary State):** Used for fast, ephemeral data. Stores active sessions, connected users, matchmaking queues, and routing info.
*   **PostgreSQL (Durable State):** The permanent source of truth for persistent data, including user profiles, hashed passwords, ELO ratings, and historical match results.

### 3. Matchmaking & Orchestration
*   **Matchmaker:** A dedicated service that listens to matchmaking requests via NATS, groups players based on their ELO ratings using Redis, and emits a `MatchFound` event.
*   **Game Allocator:** Listens for matched pairs and assigns them to an available Game Server Shard.

### 4. Game Execution (The Single Source of Truth)
*   **Game Server Shards:** Standalone C++ instances executing the authoritative `GameEngine`. They maintain the strict game rules, cooldowns, and piece logic. They consume moves from NATS, validate them, and publish the updated `GameSnapshot` back to the bus for the WS Gateway to broadcast.

## Data Flow Examples

**Matchmaking Flow:**
1. Client sends a "Find Match" request via WS or API Gateway.
2. Gateway publishes `PlayerQueued` to NATS.
3. Matchmaker processes the queue in Redis and finds a suitable pair.
4. Matchmaker publishes `MatchFound` to NATS.
5. Game Allocator catches the event, reserves a Game Shard, and publishes the `RoomReady` assignment.
6. WS Gateway notifies both clients to start rendering the board.

**Gameplay Flow (Move Execution):**
1. Client attempts a move and sends JSON to the WS Gateway.
2. WS Gateway blindly publishes `MoveRequested` to NATS.
3. The assigned Game Shard receives the event and validates the move using the C++ `GameEngine`.
4. The Game Shard updates its internal state and publishes a `GameStateUpdated` event to NATS.
5. WS Gateway receives the update and pushes the new board state to the clients.

## Deployment Roadmap (Iterative Dockerization)

To ensure system stability, the architecture is built iteratively using `docker-compose`:
*   **Phase 1:** Dockerize the existing C++ monolith alongside a PostgreSQL container.
*   **Phase 2:** Introduce Redis to externalize in-memory queues and sessions.
*   **Phase 3:** Extract the API Gateway for HTTP REST endpoints (Authentication/Rooms).
*   **Phase 4:** Deploy NATS and extract the Matchmaker into a standalone service.
*   **Phase 5:** Extract the authoritative `GameEngine` into scalable Game Server Shards.

## Observability & Health
The cluster supports logs, metrics, and health checks to ensure reliability and prepare for large-scale deployments via Kubernetes/K3s.