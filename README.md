# KungFu Chess - Real-Time Multiplayer System

## Overview
KungFu Chess is a real-time multiplayer chess game built as part of the KamaTech Bootcamp.

Unlike traditional chess, there are no turns: both players move pieces at the same time, with a cooldown after each move. The game runs on a real-time engine tick and uses a **Microservices Architecture**: the backend runs in Docker, with the real-time game server kept separate from user authentication. Players use a C++ game client (UI) that runs on their own computer and connects to both services.

## System Architecture & Microservices

The backend infrastructure is divided into decoupled services communicating over a private Docker network:

1. **API Gateway (Python / FastAPI):**
   * Acts as the entry point for client HTTP requests.
   * Handles user authentication, registration, and secure token generation.
   * Stores users in PostgreSQL and session tokens in Redis.

2. **Real-Time Game Server (C++17 / WebSocket++):**
   * A high-performance, asynchronous WebSocket server.
   * Runs the core game logic engine at a strict 20ms tick rate.
   * Completely isolated from the SQL database. Authenticates users strictly via fast Redis token lookups.
   * Handles live matchmaking, player movements, and real-time game state broadcasting.

3. **Session & Matchmaking Cache (Redis):**
   * Acts as the high-speed bridge between the API Gateway and the Game Server.
   * Stores temporary session tokens and manages the live matchmaking queue.

4. **Persistent Storage (PostgreSQL):**
   * Stores user credentials and ELO ratings.

### Game Client (C++ / OpenCV)
   * Runs on the player's computer, outside Docker.
   * Renders the board with OpenCV and sends the player's moves to the Game Server.

### Login Flow
   1. The player enters a username and password in the game client.
   2. The client sends them to the API Gateway. If the user does not exist, the client registers it first.
   3. The API Gateway returns a session token and stores it in Redis.
   4. The client connects to the Game Server and sends the token.
   5. The Game Server checks the token in Redis. The password never reaches the Game Server.

## Key Features
* **Microservices Design:** Complete separation between RESTful API services and real-time WebSocket gameplay.
* **Stateless Authentication:** Secure session management using Redis-based tokens.
* **Live Matchmaking:** ELO-based matchmaking system.
* **Real-Time Engine:** Asynchronous C++ game loop capable of handling simultaneous game rooms.
* **Containerization:** All backend services are built and started with a single Docker Compose command.

## Tech Stack
* **Game Server:** C++17, CMake, Asio, WebSocket++, nlohmann/json.
* **API Gateway:** Python 3.11, FastAPI, Uvicorn, psycopg2.
* **Game Client:** C++17, CMake, OpenCV, Asio, WebSocket++.
* **Databases:** PostgreSQL (Relational), Redis (In-Memory Data Structure Store).
* **DevOps:** Docker, Docker Compose.

## Getting Started

### Prerequisites
* Docker Desktop (or Docker Engine + Docker Compose)
* To build the game client (Windows):
   * Visual Studio with the C++ build tools
   * CMake 3.16 or later
   * The game client uses OpenCV 4.5.1, which is not included in this repository. Download the [OpenCV 4.5.1](https://opencv.org/opencv-4-5-1/) Windows pack and run it. It creates a folder named `opencv`.
   * In the project root, create a folder named `lib` with two subfolders, `include` and `bin`, and copy these files into them:

     | Copy from the `opencv` folder | To |
     |---|---|
     | Everything inside `build/include/` | `lib/include/` |
     | `build/x64/vc15/lib/opencv_world451.lib` and `opencv_world451d.lib` | `lib/bin/` |
     | `build/x64/vc15/bin/opencv_world451.dll` and `opencv_world451d.dll` | `lib/bin/` |

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
   *  Open `http://localhost:8000/docs` in a browser to see and try the API.
* **Game Server (WebSocket):** `ws://localhost:9000`
   * The game client connects to this address automatically. It cannot be opened in a browser.

### Building the Game Client
Open a terminal in the `UI` folder and run:

```bash
cmake -S . -B build
cmake --build build --config Debug
```

### Playing
A game starts when two players are connected, so run the client twice, in two separate terminals. Run it from the `UI` folder:

```bash
.\build\Debug\KungFuChess.exe
```

Enter a username and password. If the username does not exist yet, a new account is created automatically. Use a different username in each window.

### Checking Logs
To view live logs from the game server or API:
```bash
docker logs -f kungfu_server
docker logs -f kungfu_api_gateway
```

## 📁 Repository Structure
* `/API_GATEWAY` - Python FastAPI REST server and database initialization.
* `/SERVER` - C++ WebSocket server, Redis integration, Room Management, and ELO calculations.
* `/LOGIC` - Core chess rules and piece movement algorithms.
* `/UI` - C++ game client: rendering, mouse input, and server communication.
* `docker-compose.yml` - Container orchestration and network configuration.
