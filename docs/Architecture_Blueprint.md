
---
## 🏛️ ft_irc Architecture Blueprint

### 1. The Golden Rules (Constraints Shape Architecture)

Before designing, internalize these hard constraints from the subject:

| Constraint                          | Architectural Impact                                                                     |
| ----------------------------------- | ---------------------------------------------------------------------------------------- |
| **C++98 only**                      | No `std::thread`, no `auto`, no range-for. Use `std::vector`, `std::map`, `std::string`. |
| **No fork, no threads**             | Single-process, single-threaded, event-driven.                                           |
| **Only 1 `poll()` (or equivalent)** | One central event loop. All I/O multiplexed through this single call.                    |
| **Non-blocking FDs**                | Every socket must be `O_NONBLOCK`. `recv`/`send` must never block.                       |
| **No external libs**                | Pure POSIX sockets + C++98 STL.                                                          |

---
### 2. Core Philosophy: The Reactor Pattern

The architecture follows the **Reactor Pattern**:
- One central demultiplexer (`poll()`)
- When an event fires, dispatch to the appropriate handler
- Handlers process quickly and return control to the loop

```
┌─────────────────────────────────────────┐
│           REACTOR (Server)              │
│  ┌─────────┐    ┌──────────────────┐   │
│  │  poll() │───▶│ Event Dispatch   │   │
│  │  Loop   │    │ (Accept/Read/Write)│ │
│  └─────────┘    └──────────────────┘   │
│                    │                    │
│         ┌────────┴────────┐           │
│         ▼                 ▼           │
│    ┌─────────┐      ┌──────────┐      │
│    │ Accept  │      │  Handle  │      │
│    │ Handler │      │  Client  │      │
│    │         │      │  Request │      │
│    └─────────┘      └────┬─────┘      │
│                          │             │
│              ┌───────────┼───────────┐ │
│              ▼           ▼           ▼ │
│         ┌────────┐  ┌─────────┐  ┌──────┐│
│         │Command │  │ Channel │  │Client││
│         │Router  │  │ Manager │  │State ││
│         └────────┘  └─────────┘  └──────┘│
└─────────────────────────────────────────┘
```

---

### 3. The Four Pillars: Class Architecture

Based on successful implementations , the design centers on **4 core classes**:

#### **Pillar 1: `Server` (The Orchestrator)**
*Single instance, owns everything.*

**Responsibilities:**
- Create/bind/listen on the server socket
- Own and manage the `pollfd` array
- Own the client registry (`std::map<int, Client>` — key is socket fd)
- Own the channel registry (`std::map<std::string, Channel>`)
- Run the main `poll()` loop
- Route commands to handlers

**Key Members:**
```cpp
class Server {
    int _listenSocket;                    // Server socket
    std::string _password;                // Connection password
    int _port;
    
    std::vector<struct pollfd> _pollFds; // For poll()
    std::map<int, Client> _clients;       // fd -> Client
    std::map<std::string, Channel> _channels; // #name -> Channel
    
    // Command dispatch table
    typedef void (Server::*CommandFunc)(Client&, const Command&);
    std::map<std::string, CommandFunc> _commandMap;
};
```

**Key Methods:**
- `runServer()` — Main loop
- `_acceptNewClient()` — New connection
- `_handleClientRequest(int fd)` — Data available
- `_processMessage(Client&, std::string&)` — Parse and dispatch
- `_removeClient(int fd)` — Cleanup

---

#### **Pillar 2: `Client` (Connection State)**
*One per connected socket. Tracks the full IRC user state.*

**Responsibilities:**
- Buffer incoming data (crucial for partial messages)
- Track authentication state (PASS → NICK → USER sequence)
- Store user info (nickname, username, hostname, realname)
- Track joined channels
- Track operator status

**Key Members:**
```cpp
class Client {
    int _socket;
    std::string _buffer;          // Read buffer (partial messages)
    std::string _sendBuffer;      // Write buffer (for non-blocking send)
    
    // Authentication flags (must be set in order)
    bool _passAuth;
    bool _nickAuth;
    bool _userAuth;
    bool _fullyRegistered;
    
    std::string _nickname;
    std::string _username;
    std::string _hostname;
    std::string _realname;
    
    std::vector<std::string> _joinedChannels;
    bool _isOperator;
    bool _isAway;
    std::string _awayMessage;
};
```

**Critical Design Decision — The Buffer:**
IRC messages end with `\r\n`. Since TCP is stream-based, you might receive `NICK user\r\nPASS` in one read. You **must** buffer until `\r\n` is found, then extract complete messages.

```
Buffer: "NICK use" → recv adds "r\r\nPASS pass" → 
Buffer: "NICK user\r\nPASS pass"
Extract: "NICK user\r\n" → process
Buffer left: "PASS pass"
```

---

#### **Pillar 3: `Channel` (The Chat Room)**
*Represents an IRC channel and its state.*

**Responsibilities:**
- Member list (regular users + operators)
- Channel modes (+i, +t, +k, +o, +l)
- Topic management
- Message broadcasting
- Invitation list (for +i mode)
- Ban list (optional, but good for completeness)

**Key Members:**
```cpp
class Channel {
    std::string _name;
    std::string _topic;
    std::string _key;             // Password for +k
    int _userLimit;               // For +l
    
    std::set<Client*> _members;
    std::set<Client*> _operators;
    std::set<Client*> _invited;   // For +i mode
    
    // Mode flags
    bool _inviteOnly;     // +i
    bool _topicRestricted; // +t (only ops can change topic)
    bool _hasKey;          // +k
    bool _hasLimit;        // +l
};
```

**Key Methods:**
- `broadcast(const std::string& msg, Client* exclude)` — Send to all except sender
- `addMember(Client*)`, `removeMember(Client*)`
- `isOperator(Client*)`, `addOperator(Client*)`
- `isInvited(Client*)`, `invite(Client*)`

---

#### **Pillar 4: `Command` (The Parser)**
*Parses raw IRC protocol messages into structured commands.*

**IRC Message Format:**
```
:[prefix] <command> [param1] [param2] ... [:trailing parameter]\r\n
```

Examples:
```
NICK alice\r\n
USER alice 0 * :Alice Smith\r\n
JOIN #general\r\n
PRIVMSG #general :Hello everyone!\r\n
```

**Key Members:**
```cpp
class Command {
    std::string _prefix;
    std::string _command;
    std::vector<std::string> _params;
    std::string _trailing;  // The part after :
};
```

---

### 4. The Event Loop: Heart of the System

This is the **only** place where blocking is allowed (`poll()` blocks until events).

```cpp
void Server::runServer() {
    // Setup: socket(), bind(), listen(), set O_NONBLOCK
    _setupServer();
    
    while (_running) {
        // THE SINGLE poll() CALL
        int ready = poll(&_pollFds[0], _pollFds.size(), -1); // -1 = block forever
        
        for (size_t i = 0; i < _pollFds.size(); i++) {
            if (_pollFds[i].revents & POLLIN) {
                if (_pollFds[i].fd == _listenSocket)
                    _acceptNewClient();      // New connection
                else
                    _handleClientRequest(_pollFds[i].fd); // Data from client
            }
            if (_pollFds[i].revents & POLLOUT) {
                _handleClientWrite(_pollFds[i].fd); // Ready to send buffered data
            }
            if (_pollFds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                _removeClient(_pollFds[i].fd); // Disconnect
            }
        }
    }
}
```

**Why `poll()` over `epoll`/`select`?**
- `poll()` is POSIX, simpler, and perfectly adequate for IRC-scale connections (dozens/hundreds, not thousands)
- `select()` has the 1024 fd limit and requires rebuilding fd_sets each loop
- `epoll()` is Linux-only and overkill here; the subject explicitly allows `poll()` as the default 

---

### 5. Command Dispatch Architecture

Use a **function pointer map** (Command Pattern) for clean routing:

```cpp
void Server::_initCommandMap() {
    _commandMap["PASS"] = &Server::_cmdPass;
    _commandMap["NICK"] = &Server::_cmdNick;
    _commandMap["USER"] = &Server::_cmdUser;
    _commandMap["JOIN"] = &Server::_cmdJoin;
    _commandMap["PRIVMSG"] = &Server::_cmdPrivmsg;
    _commandMap["KICK"] = &Server::_cmdKick;
    _commandMap["INVITE"] = &Server::_cmdInvite;
    _commandMap["TOPIC"] = &Server::_cmdTopic;
    _commandMap["MODE"] = &Server::_cmdMode;
    _commandMap["PART"] = &Server::_cmdPart;
    _commandMap["QUIT"] = &Server::_cmdQuit;
    // ... etc
}

void Server::_processCommand(Client& client, const Command& cmd) {
    // Registration sequence enforcement
    if (!client.isFullyRegistered()) {
        if (cmd.getCommand() == "PASS") 
            _cmdPass(client, cmd);
        else if (cmd.getCommand() == "NICK" && client.hasPass())
            _cmdNick(client, cmd);
        else if (cmd.getCommand() == "USER" && client.hasPass() && client.hasNick())
            _cmdUser(client, cmd);
        else
            sendReply(client, ERR_NOTREGISTERED);
        return;
    }
    
    // Registered users: full command access
    std::map<std::string, CommandFunc>::iterator it = _commandMap.find(cmd.getCommand());
    if (it != _commandMap.end())
        (this->*(it->second))(client, cmd);
    else
        sendReply(client, ERR_UNKNOWNCOMMAND);
}
```

---

### 6. File Structure & Compilation

Based on the most successful project structures :

```
ft_irc/
├── Makefile
├── README.md
├── includes/
│   ├── Server.hpp
│   ├── Client.hpp
│   ├── Channel.hpp
│   ├── Command.hpp
│   ├── Replies.hpp          // IRC numeric reply codes (001, 401, etc.)
│   ├── Utils.hpp            // String helpers, time, etc.
│   └── Colors.hpp           // Optional: terminal colors for debug
└── srcs/
    ├── main.cpp
    ├── Server/
    │   ├── Server.cpp         // Constructor, basic methods
    │   ├── ServerRun.cpp      // runServer(), poll loop
    │   ├── ServerClient.cpp   // accept, remove, find clients
    │   ├── ServerChannels.cpp // channel management helpers
    │   └── ServerCommands.cpp // command dispatch
    ├── Client/
    │   ├── Client.cpp         // Constructor, getters, setters
    │   └── ClientBuffer.cpp   // Buffer management
    ├── Channel/
    │   ├── Channel.cpp        // Constructor, members
    │   ├── ChannelModes.cpp   // MODE command implementation
    │   └── ChannelOps.cpp     // KICK, INVITE, TOPIC
    ├── Commands/
    │   ├── Pass.cpp
    │   ├── Nick.cpp
    │   ├── User.cpp
    │   ├── Join.cpp
    │   ├── Privmsg.cpp
    │   ├── Kick.cpp
    │   ├── Invite.cpp
    │   ├── Topic.cpp
    │   ├── Mode.cpp           // Delegates to i,t,k,o,l handlers
    │   ├── Part.cpp
    │   └── Quit.cpp
    ├── Command/
    │   └── Command.cpp        // Parser implementation
    └── Utils/
        ├── Utils.cpp          // split, trim, toUpper, etc.
        └── Replies.cpp        // sendReply helpers
```

---

### 7. Key Lessons from Failed Attempts

Based on common pitfalls from peer reviews and GitHub analyses:

| Pitfall | Solution |
|---|---|
| **Partial message hell** | Always buffer client reads. Only process when `\r\n` found. |
| **Blocking send()** | Use non-blocking sockets + output buffer. If `send()` returns `EAGAIN`, buffer the rest and wait for `POLLOUT`. |
| **Nickname collisions** | Check uniqueness on `NICK` command. Case-insensitive in IRC. |
| **MODE parsing complexity** | MODE takes flags like `+i-t+k key`. Parse as a string of flag characters, each potentially consuming a parameter. |
| **Memory leaks on disconnect** | Always remove client from all channels before erasing from `_clients` map. |
| **Signal interruption** | Handle `SIGINT` gracefully — close all sockets and free memory. |

---

### 8. The Authentication State Machine

Clients must register in this exact order:

```
CONNECT → [PASS] → [NICK] → [USER] → FULLY REGISTERED
         password  nickname  username
                   (unique)   hostname
                              realname
```

Only after `USER` is the client fully registered and can use other commands.

---

### 9. IRC Reply System

The server must speak IRC protocol. Key reply categories:

| Type | Example | Meaning |
|---|---|---|
| Welcome | `:server 001 nick :Welcome...` | Registration complete |
| Error | `:server 401 nick :No such nick` | Target not found |
| Channel | `:server 332 nick #chan :Topic` | Topic reply |
| Broadcast | `:nick!user@host PRIVMSG #chan :msg` | Message to channel |

Create a `Replies.hpp` with all numeric codes defined as constants.

---

### 10. Testing Architecture

Before coding, plan how to test:

1. **netcat (`nc`)** — Raw TCP, test partial messages with `ctrl+D`
2. **irssi** — Real IRC client, test full compatibility
3. **Multiple nc instances** — Test concurrency and broadcasting
4. **Valgrind** — Memory leak detection
5. **flood test** — Rapid connect/disconnect to stress the poll loop

---

## Next Steps

This architecture gives you the skeleton. The code will flow naturally because:

1. **Start with `Server`**: Get `socket() → bind() → listen() → poll() → accept()` working
2. **Add `Client`**: Buffer reads, extract `\r\n`-terminated messages
3. **Add `Command`**: Parse the message format
4. **Add registration commands**: `PASS`, `NICK`, `USER` with state machine
5. **Add `Channel`**: `JOIN`, `PART`, `PRIVMSG` to channels
6. **Add operator commands**: `KICK`, `INVITE`, `TOPIC`, `MODE`
7. **Polish**: Error replies, edge cases, memory safety

Want me to elaborate on any specific pillar (e.g., the MODE parser, the buffer management algorithm, or the reply code system)?
