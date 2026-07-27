# Cloud Server Architecture for KungFu Chess - Load & Scale Design
**Cloud System Design Document**
---

## 1. Introduction & System Objectives
This document details the architectural transition from a single-process server based on direct WebSocket connections and local SQLite storage[cite: 1], to a distributed cloud microservices architecture.

The system is designed to meet the following high-scale requirements:
* Support **100 million registered users**.
* Support **10 million concurrent active players** (representing 5 million simultaneous active rooms).
* Manage real-time, low-latency data flow with zero freezing or lag.
* Ensure high fault tolerance and automated self-healing capabilities.

---

## 2. Microservices Architecture & Traffic Flow (Docker Containers)
To support 10 million concurrent players without memory or CPU exhaustion, the backend is decoupled into 5 specialized service types, orchestrated via Kubernetes or K3s. 

The following architecture diagram illustrates how the components interact, separating real-time client communication from game logic and persistent storage:

```mermaid
graph TD
    %% Clients Layer
    subgraph Clients [Clients Layer - 10M Concurrent Players]
        C1[Player 1 - White]
        C2[Player 2 - Black]
        C3[Viewers / Spectators]
    end

    %% Load Balancer
    LB[Cloud Load Balancer]
    C1 -- WebSocket --> LB
    C2 -- WebSocket --> LB
    C3 -- WebSocket --> LB

    %% Gateways Layer
    subgraph Gateways [Stateless Gateways Layer - Dozens/Hundreds of Pods]
        GW1[WS Gateway Node 1]
        GW2[WS Gateway Node 2]
        GW3[WS Gateway Node N]
    end
    LB ---> GW1 & GW2 & GW3

    %% Middle Tier - Redis Bus & Cache
    subgraph MiddleTier [Middle Tier - Redis In-Memory Cluster]
        REDIS[(Redis Cache - Live Room States)]
        BUS[[Pub/Sub Message Bus]]
    end

    %% Gateway Connections to Middle Tier
    GW1 <== Pub/Sub ==> BUS
    GW2 <== Pub/Sub ==> BUS
    GW3 <== Pub/Sub ==> BUS
    GW1 & GW2 & GW3 --- REDIS

    %% Backend Services Layer
    subgraph LogicLayer [Backend Services - Thousands of Pods]
        MM[Matchmaker Service]
        LOGIC1[Logic Server 1 - Rooms 1 to 5K]
        LOGIC2[Logic Server 2 - Rooms 5K to 10K]
        WORKER[Background Workers]
    end

    %% Service Connections
    MM <-->|Poll Waiting Queue| REDIS
    LOGIC1 <== Pub/Sub ==> BUS
    LOGIC2 <== Pub/Sub ==> BUS
    LOGIC1 & LOGIC2 <-->|Read/Write Board State| REDIS
    LOGIC1 & LOGIC2 -.->|Emit GameEnd Event| WORKER

    %% Persistent Storage
    subgraph Storage [Persistent Storage Layer]
        DB[(PostgreSQL / MongoDB - 100M Users)]
        S3[Cloud Object Storage - Game Logs]
    end

    WORKER -->|Update ELO & Users| DB
    WORKER -->|Archive Logs| S3
    
To support 10 million concurrent players without memory or CPU exhaustion, the backend is decoupled into 5 specialized service types, orchestrated via Kubernetes or K3s:

| Container Type | Role & Responsibilities | Scaling & Memory Characteristics |
| :--- | :--- | :--- |
| **WS Gateways (Communication)** | Maintain open WebSocket connections with clients, receive commands (e.g., `WQe2e5`), and broadcast live board states to players and viewers[cite: 1]. | **100% Stateless.** Deployed in dozens/hundreds of replicas behind a Load Balancer. |
| **Matchmakers** | Manage the "Play" button: match players within a $\pm 100$ ELO range, wait up to 1 minute, generate a new Room ID, and assign game sessions[cite: 1]. | Lightweight and fast services polling queues from cache. Deployed in a small number of replicas. |
| **Logic Servers** | Validate move legality, enforce piece cooldowns, manage the 20-second auto-resign disconnection countdown, and determine game outcomes[cite: 1]. | Hold logical room state strictly in memory. Deployed in thousands of concurrent replicas, each managing several thousand rooms. |
| **Background Workers** | Process game-end events, update ELO ratings in the persistent database, and archive game logs[cite: 1]. | Operate asynchronously in the background to avoid blocking critical real-time game logic. |
| **Redis Cluster (In-Memory)** | **Core Infrastructure Layer:** Hold active room states in RAM, manage matchmaking queues, and act as the Pub/Sub message broker[cite: 1]. | Dedicated, RAM-heavy nodes running completely independent of logic and gateway services. |

---

## 3. Data Management for 100M Users (Polyglot Persistence)
 the architecture implements a dual-database approach:
1. **Persistent Database (PostgreSQL / MongoDB):** Stores static and long-term records: registered user profiles, hashed passwords, historical game logs, and permanent ELO ratings[cite: 1]. Configured with Sharding and Primary-Replica replication for high performance and redundancy.
2. **High-Speed In-Memory Cache (Redis Cluster):** Manages active user sessions, live room states, and matchmaking queues. RAM access is orders of magnitude faster than disk access, preventing bottlenecks and offloading read/write pressure from the primary database.

---

## 4. Inter-Server Communication & Room Management (Pub/Sub Bus)
To ensure players connected to different Gateway instances can play together seamlessly and join any room ("everyone can play with everyone"), the backend utilizes a centralized message broker (**Pub/Sub Bus** via Redis Pub/Sub or Apache Kafka)[cite: 1]:

* **Middle-Tier Architecture:** Redis does not reside inside Gateway or Logic containers; it serves as an independent, centralized middle-tier data layer. All Gateways and Logic servers communicate with the Redis Cluster over the cloud provider's high-speed internal network. This ensures all instances share an identical, real-time state, preventing data loss if a logic container crashes.
* **Dedicated Room Channels:** Every active game room is assigned a dedicated Pub/Sub channel[cite: 1].
* **Bidirectional Event Flow:**
  * When a player submits a move, the assigned WS Gateway publishes the event to the room's Pub/Sub channel[cite: 1].
  * The assigned Logic Server, along with Gateways handling the opponent and any spectators (viewers), subscribe to this channel[cite: 1].
  * Score updates, move logs, audio triggers, and start/end animations are routed through this bus, reaching all relevant clients instantly[cite: 1].

---

## 5. Network Bandwidth Analysis
Calculations are based on 10 million concurrent players, representing 5 million active rooms consisting of White and Black players[cite: 1]:
* **Input Command Rate:** A player makes a move on average every 2 seconds, generating 0.5 commands/sec per player.
* **Total Inbound Traffic:** 10,000,000 players $\times$ 0.5 = 5,000,000 messages/sec.
* **Total Outbound Traffic (Minimum):** Every move is broadcast to at least the opponent = 5,000,000 messages/sec.
* **Total Message Throughput:** 10,000,000 messages/sec (excluding spectators).

**Volume Calculation:**
Assuming an average WebSocket payload (including headers) of 250 Bytes:
* **Data Volume:** 10,000,000 msgs/sec $\times$ 250 Bytes = 2,500,000,000 Bytes/sec ($\approx 2.5$ GB/sec).
* **Required Bandwidth:** In networking terms, this equals approximately **20 Gbps** (Gigabits per second).

**Load Evaluation:** While 20 Gbps would overwhelm a single physical server, **it is a standard, manageable volume for distributed cloud infrastructure.** Distributing this traffic across 100 WS Gateway containers behind Cloud Load Balancers reduces the load to a lightweight 200–400 Mbps per container, which modern servers handle effortlessly.

---

## 6. Adapting to Short Game Durations (30–90 Seconds)
Short game sessions result in an extremely high churn rate (approx. 100,000 rooms created and destroyed every second). The architecture accommodates this through:
* **Stateless Gateways:** Gateways retain no game rules or board memory. Upon game completion, the Gateway simply unsubscribes from the room's channel and immediately frees up system memory[cite: 1].
* **Asynchronous Cleanup:** When a game ends, the Logic Server emits a `GameEnd` event to the bus and immediately becomes available to host a new room[cite: 1]. Heavy DB writes (ELO calculations and log archiving) are offloaded entirely to Background Workers[cite: 1].
* **Dynamic Auto-Scaling:** Kubernetes monitors traffic spikes and drops, automatically spinning Gateway and Logic pods up or down within seconds to optimize resource usage and reduce cloud hosting costs.

---

## 7. Fault Tolerance & Edge Cases

### A. What happens if a server (container) crashes?
* **Automated Self-Healing:** The Kubernetes control plane detects unresponsive pods (Gateways or Logic servers) and immediately terminates and replaces them with healthy instances within seconds.
* **Seamless Continuity:** Because WS Gateways are completely stateless, a disconnected client is instantly routed to a healthy Gateway by the Load Balancer. The new Gateway fetches the current board state from Redis, allowing the user to resume playing with zero perceived data loss.

### B. What happens if the database crashes?
* **Automated Failover & Replication:** The primary database (PostgreSQL/MongoDB) runs in a High-Availability (HA) Primary-Replica configuration. All writes are replicated in real-time. If the Primary node fails, the Replica is automatically promoted to Primary with minimal downtime.
* **Cache Buffer Resilience:** During brief DB failover windows, active game sessions continue running smoothly because live states are read and written directly to the Redis cache cluster. Data is synchronized back to the persistent DB once it recovers.

### C. What happens if disk space runs out?
* **Decoupled Cloud Object Storage:** Containers do not store logs locally. All client and server activity logs[cite: 1] are streamed directly to external, scalable object storage (e.g., AWS S3 or Google Cloud Storage), which provides virtually limitless capacity.
* **Log Rotation & Retention Policies:** Automated lifecycle policies compress and archive old game logs, automatically deleting records older than 30 days.
* **Proactive Monitoring & Alerts:** Infrastructure monitoring tools (Prometheus & Grafana) trigger automated alerts to the engineering team when database disk volumes reach 75% capacity, allowing for proactive volume expansion before storage is exhausted.