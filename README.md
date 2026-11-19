# KungFu Chess - Real-Time Multiplayer System

## Overview
KungFu Chess is an advanced, real-time multiplayer chess platform built as a graduation project in Software Engineering. 

Unlike traditional turn-based chess, this system operates on a real-time game engine tick and implements a modern **Microservices Architecture**. The backend is fully containerized using Docker, demonstrating high scalability, separation of concerns, and stateless session management.

## System Architecture & Microservices

The backend infrastructure is divided into decoupled services communicating over a private Docker network:

1. **API Gateway (Python / FastAPI):**
   * Acts as the entry point for client HTTP requests.
   * Handles user authentication, registration, and secure token generation.
   * Interacts exclusively with the persistent PostgreSQL database.

2. **Real-Time Game Server (C++17 / WebSocket++):**
   * A high-performance, asynchronous WebSocket server.
   * Runs the core game logic engine at a strict 20ms tick rate.
   * Completely isolated from the SQL database. Authenticates users strictly via fast Redis token lookups.
   * Handles live matchmaking, player movements, and real-time game state broadcasting.

3. **Session & Matchmaking Cache (Redis):**
   * Acts as the high-speed bridge between the API Gateway and the Game Server.
   * Stores temporary session tokens and manages the live matchmaking queue.

4. **Persistent Storage (PostgreSQL):**
   * Securely stores user credentials, profiles, and ELO ratings.

## Key Features
* **Microservices Design:** Complete separation between RESTful API services and real-time WebSocket gameplay.
* **Stateless Authentication:** Secure session management using Redis-based tokens.
* **Live Matchmaking:** ELO-based matchmaking system.
* **Real-Time Engine:** Asynchronous C++ game loop capable of handling simultaneous game rooms.
* **Containerization:** Fully automated build and deployment pipeline using Docker Compose.

## Tech Stack
* **Game Server:** C++17, CMake, Asio, WebSocket++, nlohmann/json.
* **API Gateway:** Python 3.11, FastAPI, Uvicorn, psycopg2.
* **Databases:** PostgreSQL (Relational), Redis (In-Memory Data Structure Store).
* **DevOps:** Docker, Docker Compose.

## Getting Started

### Prerequisites
* Docker
* Docker Desktop (or Docker Engine + Docker Compose)

### Running the System
Starting the entire microservices environment requires only a single command. Open your terminal in the root directory and run:

```bash
docker-compose up --build -d
```

This command will automatically:
1. Pull required images (Ubuntu, Python, Postgres, Redis).
2. Compile the C++ Game Server and Logic Library using CMake.
3. Install Python dependencies for the API Gateway.
4. Launch and link all 4 containers in the correct order.

The services will be available at:
* **API Gateway (HTTP):** `http://localhost:8000`
* **Game Server (WebSocket):** `ws://localhost:9000`

### Checking Logs
To view live logs from the game server or API:
```bash
docker logs -f kungfu_server
docker logs -f kungfu_api_gateway
```

## 📁 Repository Structure
* `/API_GATEWAY` - Python FastAPI REST server and database initialization.
* `/SERVER` - C++ WebSocket server, Redis integration, and Room Management.
* `/LOGIC` - Core chess rules, piece movement algorithms, and ELO calculations.
* `docker-compose.yml` - Container orchestration and network configuration.