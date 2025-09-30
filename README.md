# ✅ Server (C++)

A lightweight C++ server built with standalone Asio and nlohmann/json.
It allows multiple clients to manage a shared to-do list in real time:
* 📌 Add new tasks
* 🔄 Toggle task status
* 📋 Retrieve the full list
* 📡 Broadcast updates to all connected clients

# 📦 Dependencies

* C++17 or newer
* [Asio (standalone)](https://github.com/chriskohlhoff/asio)
* [nlohmann/json](https://github.com/nlohmann/json)

Both libraries are header-only, no .lib or .dll required.

# ⚙️ Build Instructions (CMake)
1. Clone the repository
2. Download dependencies (if you didn't download from repository)
* Asio (standalone). Copy everything from include folder to %projectDir%/include
* nlohmann/json. Copy json.hpp to %projectDir%/include
3. Build:
* mkdir build && cd build
* cmake ..
* cmake --build .

# 🚀 Run

./Server [<port>]

Example:

* ./Server

* ./Server 9000

The server listens on all interfaces (0.0.0.0).

Clients can connect both from localhost and from other machines in the network.

# 📡 API (JSON over TCP)
Communication is done over plain TCP.

Each message is a JSON object terminated with \n.

Here some examples:

## ➕ Add Item
Client → Server
```
{
    "action": "add",
    "description": "Buy milk"
}
```
Server → Client
```
{
    "id": 1, "description":
    "Buy milk", "status": "Pending"
}
```

## 🔄 Toggle Status

Client → Server
```
{ "action": "toggle", "id": 1 }
```

Server → Client
```
{
  "id": 1,
  "description": "Buy milk",
  "status": "Completed"
}
```

## 📋 Get List
Client → Server
```
{ "action": "list" }
```
Server → Client
```
{
    "items": [
      { "id": 1, "description": "Buy milk", "status": "Completed" },
      { "id": 2, "description": "Write code", "status": "Pending" } 
    ] 
}
```

# 🔔 Real-Time Notifications
When one client adds or toggles a task, the server broadcasts the update to all connected clients in the same JSON format.

# 🛡️ Notes
* Make sure the chosen port is open in your firewall.
* For remote access, you may need port forwarding if behind NAT.
* The to-do list is stored in memory only — data is lost when the server restarts.