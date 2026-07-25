# ft_irc — Complete File Reference

> Every file in this project, what it does, and how it connects to everything else.

---

## Project Overview

**ft_irc** is an IRC (Internet Relay Chat) server written in C++98. It uses the **Reactor pattern** — a single `poll()` event loop, single process, no threads. It supports 14 IRC commands, channel management, operator privileges, and multiple concurrent clients.

```
./ircserv <port> <password>
```

---

## Directory Structure

```
irc/
├── Makefile
├── README.md
├── .gitignore
├── structure.txt
├── docs/
│   ├── Architecture_Blueprint.md
│   ├── File_Reference.md          ← you are here
│   └── Team.md
├── includes/
│   ├── Server.hpp
│   ├── Client.hpp
│   ├── Channel.hpp
│   ├── Channelmanager.hpp
│   ├── Command.hpp
│   ├── Replies.hpp
│   ├── Utils.hpp
│   └── Colors.hpp
├── srcs/
│   ├── main.cpp
│   ├── Server/
│   │   ├── Server.cpp
│   │   ├── ServerRun.cpp
│   │   ├── ServerClient.cpp
│   │   ├── ServerChannels.cpp
│   │   └── ServerCommands.cpp
│   ├── Client/
│   │   ├── Client.cpp
│   │   └── ClientBuffer.cpp
│   ├── Channel/
│   │   ├── Channel.cpp
│   │   ├── ChannelManager.cpp
│   │   ├── ChannelModes.cpp       (stub — unused)
│   │   └── ChannelOps.cpp         (does not exist)
│   ├── Commands/
│   │   ├── Pass.cpp
│   │   ├── Nick.cpp
│   │   ├── User.cpp
│   │   ├── Join.cpp
│   │   ├── Part.cpp
│   │   ├── Privmsg.cpp
│   │   ├── Kick.cpp
│   │   ├── Invite.cpp
│   │   ├── Topic.cpp
│   │   ├── Mode.cpp
│   │   ├── Quit.cpp
│   │   ├── Ping.cpp
│   │   └── Whois.cpp
│   ├── Command/
│   │   └── Command.cpp
│   └── Utils/
│       ├── Utils.cpp
│       └── Replies.cpp            (does not exist — shared with utils)
└── ircserv                        (compiled binary, git-ignored)
```

---

# Root Files

---

### `Makefile`

**Purpose:** Build system. Compiles all `.cpp` files into the `ircserv` binary.

**Key details:**
- Compiler: `c++` with flags `-Wall -Wextra -Werror -std=c++98 -g`
- Include path: `-I includes/`
- Rules: `all`, `$(NAME)`, `clean`, `fclean`, `re`
- Auto-compiles all `.cpp` to `.o` via pattern rule `%.o: %.cpp`
- Links all `.o` files into the `ircserv` executable

**Notes:**
- Lines 7-32 contain a commented-out OLD source list (missing `ChannelManager.cpp`, `Ping.cpp`, `Whois.cpp`)
- Lines 34-59 contain the active source list

---

### `README.md`

**Purpose:** Project documentation for evaluators and visitors.

**Current state:** EMPTY (0 bytes).

**Subject requires:**
1. First line italicized: `*This project has been created as part of the 42 curriculum by <login1>[, <login2>[, <login3>[...]]].*`
2. **Description** section — what the project is and its goal
3. **Instructions** section — compilation, installation, execution
4. **Resources** section — documentation links + how AI was used

---

### `.gitignore`

**Purpose:** Prevents compiled artifacts and editor files from being tracked.

**Ignores:**
- `ircserv` — the compiled binary
- `ircserv.dSYM/` — debug symbol directory (macOS)
- `*.o`, `*.obj` — object files
- `*.out`, `*.exe`, `*.a`, `*.so`, `*.dylib`, `*.dll` — other build artifacts
- `.vscode/`, `.idea/`, `*.swp`, `*.swo`, `*~`, `.DS_Store`, `Thumbs.db` — editor files
- `*.log`, `vgcore.*` — debug output

---

### `structure.txt`

**Purpose:** Quick-reference directory tree for developers. Shows the intended file layout.

**Note:** Slightly outdated — lists `ChannelOps.cpp` which was never created, and doesn't list `ChannelManager.cpp`, `Ping.cpp`, or `Whois.cpp`.

---

# Documentation (`docs/`)

---

### `docs/Architecture_Blueprint.md`

**Purpose:** The architectural design document. Written BEFORE coding to plan the system.

**Contents:**
1. **Golden Rules** — The hard constraints from the subject (C++98, no threads, single poll(), non-blocking, no external libs)
2. **Reactor Pattern** — The central design pattern explained with an ASCII diagram
3. **Four Pillars** — Core classes (Server, Client, Channel, Command) with detailed member descriptions
4. **Event Loop** — Pseudocode for the main `poll()` loop
5. **Command Dispatch** — Function pointer map pattern
6. **File Structure** — Intended directory layout
7. **Key Lessons** — Pitfalls from other implementations (partial messages, blocking sends, etc.)
8. **Auth State Machine** — PASS → NICK → USER sequence
9. **IRC Reply System** — Numeric reply categories
10. **Testing Architecture** — How to test with nc, irssi, valgrind

---

### `docs/Team.md`

**Purpose:** Team workflow document defining who owns what.

**Team:**
| Person | Role |
|---|---|
| **Hippo** (you) | Maintainer — owns all `.hpp` files, Makefile, tests, merges |
| **Mohammed** | Core — Server engine, Client, Parser, Auth commands, Utils |
| **Noura** | Channels — Channel class, Join/Part/Privmsg, Kick/Invite/Topic/Mode |

**Branch structure:** `master` ← `develop` ← `feature/*` branches
**Rule:** Mohammed and Noura never modify `.hpp` files. Only Hippo touches headers.

---

# Header Files (`includes/`)

---

### `includes/Server.hpp`

**Purpose:** The Server class declaration — the central orchestrator that owns everything.

**Key members:**
| Member | Type | Purpose |
|---|---|---|
| `_port` | `int` | Listening port number |
| `_password` | `std::string` | Server connection password |
| `_listenFd` | `int` | Server socket file descriptor |
| `_running` | `static bool` | Signal-controlled run flag |
| `_pollFds` | `vector<pollfd>` | All tracked file descriptors (server + clients) |
| `_clients` | `map<int, Client*>` | All connected clients, keyed by fd |
| `_channels` | `map<string, Channel>` | All channels, keyed by `#name` |
| `_cmdMap` | `map<string, CmdHandler>` | Command name → member function pointer |

**CmdHandler type:** `void (Server::*)(Client&, const Command&)` — function pointer to a command handler method.

**Key methods:**
- `runServer()` — Enter the main poll loop
- `signalHandler()` — Static, handles SIGINT/SIGQUIT, sets `_running = false`
- `_setupServer()` — `socket()` → `bind()` → `listen()` → `fcntl(O_NONBLOCK)`
- `_initCommandMap()` — Populate `_cmdMap` with all 13 commands
- `_acceptNewClient()` — Accept TCP connection, create Client
- `_removeClient(fd)` — Clean disconnect from all channels
- `_handleClientData(fd)` — Read data from client
- `_handleClientWrite(fd)` — Write buffered data to client
- `_findClientByNick(nick)` — Case-insensitive nickname lookup
- `_processMessage()` — Parse raw message → dispatch command
- `_dispatchCommand()` — Route command to handler (with auth gating)
- `_sendReply()` — Send numeric reply (note: BROKEN — incomplete format)
- `_removeClientFromAllChannels()` — Clean up channel memberships
- 13 `_cmd*()` methods — Individual command handlers

---

### `includes/Client.hpp`

**Purpose:** Represents one connected user. Tracks socket, buffers, auth state, identity, and channel membership.

**Key members:**
| Member | Type | Purpose |
|---|---|---|
| `_fd` | `int` | Client socket file descriptor |
| `_readBuffer` | `string` | Accumulates partial TCP data until `\r\n` found |
| `_sendBuffer` | `string` | Buffers outgoing data for non-blocking writes |
| `_passAuth` | `bool` | PASS completed |
| `_nickAuth` | `bool` | NICK completed |
| `_userAuth` | `bool` | USER completed |
| `_registered` | `bool` | Fully registered (PASS+NICK+USER done) |
| `_nickname` | `string` | Chosen nickname |
| `_username` | `string` | Username from USER command |
| `_hostname` | `string` | IP address from TCP connection |
| `_realname` | `string` | Real name from USER command |
| `_channels` | `vector<string>` | Channel names the client has joined |

**Key methods:**
- `appendToBuffer()` / `extractMessages()` — Read buffer management (TCP stream → complete messages)
- `sendMessage()` / `getSendBuffer()` / `clearSendBuffer()` / `hasPendingSend()` — Send buffer management
- `hasPassAuth()` / `hasNickAuth()` / `hasUserAuth()` / `isRegistered()` — Auth state queries
- `getPrefix()` — Returns `nick!user@host` format
- `addChannel()` / `removeChannel()` / `isInChannel()` / `getChannels()` — Channel tracking

**Current issues:**
- `sendMessage()` doesn't send — it buffers. Misleading name.
- `sendMessage()` and `appendToSendBuffer()` do the same thing (duplicate API)
- No buffer size limits (DoS vector for slow clients)
- Default constructor leaves `_hostname` uninitialized

---

### `includes/Channel.hpp`

**Purpose:** Represents one IRC channel (a chat room). Tracks members, operators, topic, and mode flags.

**Key members:**
| Member | Type | Purpose |
|---|---|---|
| `_name` | `string` | Channel name (e.g., `#general`) |
| `_topic` | `string` | Channel topic text |
| `_key` | `string` | Channel password (for `+k` mode) |
| `_userLimit` | `int` | Max users (for `+l` mode) |
| `_members` | `set<Client*>` | Users currently in the channel |
| `_operators` | `set<Client*>` | Users with operator privileges |
| `_invited` | `set<Client*>` | Invited users (for `+i` mode) |
| `_inviteOnly` | `bool` | `+i` flag |
| `_topicRestricted` | `bool` | `+t` flag (only ops can change topic) |
| `_hasKey` | `bool` | `+k` flag (password protected) |
| `_hasLimit` | `bool` | `+l` flag (user limit) |

**Key methods:**
- `addMember()` / `removeMember()` / `isMember()` / `getMemberCount()` / `getMembers()` — Member management
- `addOperator()` / `removeOperator()` / `isOperator()` — Operator management
- `addInvited()` / `removeInvited()` / `isInvited()` — Invitation management
- `setTopic()` / `getTopic()` — Topic management
- `setInviteOnly()` / `setTopicRestricted()` / `setKey()` / `setLimit()` / etc. — Mode flags
- `broadcast(message, exclude)` — Send message to all members (optionally excluding sender)
- `getModeString()` — Returns something like `"+itk"` for RPL_CHANNELMODEIS

**Cleanup note:** `removeMember()` also removes from `_operators` and `_invited` — prevents stale pointers.

---

### `includes/Channelmanager.hpp`

**Purpose:** A wrapper class around `map<string, Channel*>` for create/get/remove operations.

**Key methods:**
- `getChannel(name)` — Find by name, returns NULL if not found
- `createChannel(name)` — Creates a new Channel on the heap
- `removeChannel(name)` — Deletes and removes

**Current status:** UNUSED. The `Server` class manages channels directly via its own `_channels` map. This class is dead code.

---

### `includes/Command.hpp`

**Purpose:** Parses raw IRC protocol messages into structured data. No business logic — pure parsing.

**IRC message format:**
```
:[prefix] COMMAND [param1] [param2] ... [:trailing data]\r\n
```

**Key members:**
| Member | Type | Purpose |
|---|---|---|
| `_prefix` | `string` | Optional `:nick!user@host` origin |
| `_command` | `string` | The command name (e.g., "JOIN", "PRIVMSG") |
| `_params` | `vector<string>` | Space-separated parameters |
| `_trailing` | `string` | Everything after the final `:` |
| `_hasTrailing` | `bool` | Whether a trailing part exists |

**Key methods:**
- `static parse(raw)` — The parser. Splits prefix, command, params, and trailing.
- `getPrefix()` / `getCommand()` / `getParams()` / `getTrailing()` / `hasTrailing()` — Accessors
- `getParam(index)` / `paramCount()` — Convenience accessors

---

### `includes/Replies.hpp`

**Purpose:** Defines all IRC numeric reply codes as string constants, plus the server name.

**Server name:** `#define SERVER_NAME "ircserv"`

**Defined codes:**

| Group | Codes |
|---|---|
| **Welcome** (001-004) | `RPL_WELCOME`, `RPL_YOURHOST`, `RPL_CREATED`, `RPL_MYINFO` |
| **Channel** (324-366) | `RPL_CHANNELMODEIS`, `RPL_NOTOPIC`, `RPL_TOPIC`, `RPL_INVITING`, `RPL_NAMREPLY`, `RPL_ENDOFNAMES` |
| **Errors** (401-482) | `ERR_NOSUCHNICK`, `ERR_NOSUCHCHANNEL`, `ERR_CANNOTSENDTOCHAN`, `ERR_TOOMANYCHANNELS`, `ERR_NORECIPIENT`, `ERR_NOTEXTTOSEND`, `ERR_UNKNOWNCOMMAND`, `ERR_NONICKNAMEGIVEN`, `ERR_ERRONEUSNICKNAME`, `ERR_NICKNAMEINUSE`, `ERR_USERNOTINCHANNEL`, `ERR_NOTONCHANNEL`, `ERR_USERONCHANNEL`, `ERR_NOTREGISTERED`, `ERR_NEEDMOREPARAMS`, `ERR_ALREADYREGISTERED`, `ERR_PASSWDMISMATCH`, `ERR_CHANNELISFULL`, `ERR_INVITEONLYCHAN`, `ERR_BADCHANNELKEY`, `ERR_CHANOPRIVSNEEDED` |
| **Misc** (221-409) | `RPL_UMODEIS`, `RPL_WHOISUSER`, `RPL_ENDOFWHOIS`, `ERR_NOORIGIN` |

**Note:** The `_sendReply()` method in `ServerCommands.cpp` is BROKEN — it sends numeric codes without the required parameters (channel names, target nicks, etc.).

---

### `includes/Utils.hpp`

**Purpose:** Declares small utility functions shared across the codebase.

**Functions:**
- `toUpper(str)` — Convert string to uppercase (used for case-insensitive nickname comparisons)
- `trim(str)` — Remove leading/trailing whitespace
- `split(str, delimiter)` — Split string on delimiter (e.g., channel lists with commas)
- `sendToClient(fd, message)` — Intended to send data over socket (currently an empty stub)

---

### `includes/Colors.hpp`

**Purpose:** ANSI terminal color codes for debug logging. Used in `Server.cpp` and `ServerRun.cpp` to colorize console output.

**Defined colors:** RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, RESET, BOLD

**Usage in code:**
- `CLR_GREEN` — New connections, server start
- `CLR_RED` — Disconnections, errors
- `CLR_CYAN` — Incoming messages (`[<<]` log)
- `CLR_YELLOW` — Shutdown
- `CLR_BOLD` — Emphasized text

---

# Source Files (`srcs/`)

---

## Entry Point

### `srcs/main.cpp`

**Purpose:** Program entry point. Validates arguments, sets up signal handlers, creates and runs the server.

**Flow:**
1. Check `argc == 3`
2. Validate port is numeric and in range [1024, 65535]
3. Validate password is non-empty
4. Set signal handlers:
   - `SIGINT` → `Server::signalHandler` (Ctrl+C)
   - `SIGQUIT` → `Server::signalHandler` (Ctrl+\)
   - `SIGPIPE` → `SIG_IGN` (ignore broken pipes)
5. Create `Server` object → calls `_setupServer()` + `_initCommandMap()`
6. Call `server.runServer()` — blocks until shutdown
7. On exception, print fatal error and return 1

---

## Server Module (`srcs/Server/`)

---

### `srcs/Server/Server.cpp`

**Purpose:** Server constructor, destructor, socket setup, signal handling.

**Functions:**

| Function | What it does |
|---|---|
| `Server(port, password)` | Constructor. Stores port/password, calls `_setupServer()`, calls `_initCommandMap()` |
| `~Server()` | Destructor. Closes all client sockets, deletes all Client objects, clears channels, closes listen socket |
| `_setupServer()` | Creates TCP socket → `setsockopt(SO_REUSEADDR)` → `bind()` to port → `listen()` → `fcntl(O_NONBLOCK)` → adds to `_pollFds` |
| `isRunning()` | Static. Returns `_running` flag |
| `signalHandler(signum)` | Static. Sets `_running = false` to trigger graceful shutdown |

**Error handling:** Each socket operation throws `std::runtime_error` on failure, with cleanup of previously opened resources.

**Static `_running`:** Initialized to `true` at file scope. Set to `false` by signal handler. The main loop checks this to decide when to exit.

---

### `srcs/Server/ServerRun.cpp`

**Purpose:** The heart of the system. Contains the main `poll()` event loop.

**The loop (simplified):**
```
while (_running):
    1. For each client: if hasPendingSend(), set events = POLLIN|POLLOUT
    2. poll() all fds, block until events
    3. Take a snapshot of _pollFds (to handle removals during iteration)
    4. For each fd with events:
       - POLLERR|POLLHUP|POLLNVAL → remove client
       - POLLIN on listenFd → accept new client
       - POLLIN on client fd → read data, process messages
       - POLLOUT on client fd → flush send buffer
```

**Key design decisions:**
- Takes a `snapshot` of `_pollFds` before iterating — safe against vector modifications during callbacks
- Sets `POLLOUT` dynamically based on `hasPendingSend()` — avoids busy-looping on writable sockets
- Blocks indefinitely (`-1` timeout) — no busy-waiting

**Functions:**

| Function | What it does |
|---|---|
| `runServer()` | The main event loop. Contains all the logic described above. |

---

### `srcs/Server/ServerClient.cpp`

**Purpose:** Client lifecycle management — accept, remove, and find clients.

**Functions:**

| Function | What it does |
|---|---|
| `_acceptNewClient()` | `accept()` new TCP connection → `fcntl(O_NONBLOCK)` → get IP address via `inet_ntoa` → create `pollfd` → create `Client` object → add to `_pollFds` and `_clients` |
| `_removeClient(fd)` | Find client by fd → remove from all channels → delete Client object → close socket → remove from `_pollFds` |
| `_findClientByNick(nickname)` | Case-insensitive search through `_clients` map using `toUpper()` comparison. Returns `NULL` if not found |

**Accept flow:** Creates `Client` before the client has authenticated. The client starts with all auth flags `false` and must complete PASS → NICK → USER.

**Remove flow:** Always removes from channels FIRST (to broadcast quit messages while the client still exists), then deletes the client and closes the socket.

---

### `srcs/Server/ServerChannels.cpp`

**Purpose:** Channel cleanup when a client disconnects.

**Functions:**

| Function | What it does |
|---|---|
| `_removeClientFromAllChannels(fd)` | Gets the client's channel list → for each channel: broadcast QUIT → remove member → remove operator → remove invite → if channel empty, delete it |

**Why this is separate:** Channel removal can trigger channel deletion (empty channels are removed). This logic is complex enough to warrant its own file.

---

### `srcs/Server/ServerCommands.cpp`

**Purpose:** Command dispatch infrastructure. Routes parsed commands to the correct handler function.

**Functions:**

| Function | What it does |
|---|---|
| `_initCommandMap()` | Fills `_cmdMap` with all 13 commands mapped to their handler methods |
| `_handleClientData(fd)` | `recv()` data from client → `appendToBuffer()` → `extractMessages()` → for each message: `_processMessage()` |
| `_handleClientWrite(fd)` | `getSendBuffer()` → `send()` data in non-blocking way → if partial send, re-buffer remainder → if fully sent, remove POLLOUT from events |
| `_processMessage(client, message)` | Log the message → `Command::parse()` → if command is non-empty, `_dispatchCommand()` |
| `_dispatchCommand(client, cmd)` | **THE GATEKEEPER.** If not registered: only PASS, NICK (+PASS), USER (+PASS), QUIT, CAP are allowed. If registered: look up command in `_cmdMap` and call it, or send `ERR_UNKNOWNCOMMAND` |
| `_sendReply(client, code)` | **BROKEN.** Sends `:server code nick\r\n` but most numeric replies need additional parameters (channel name, target nick, etc.) |

**Command dispatch table:**
```
PASS    → _cmdPass      (Mohammed)
NICK    → _cmdNick      (Mohammed)
USER    → _cmdUser      (Mohammed)
QUIT    → _cmdQuit      (Mohammed)
PING    → _cmdPing      (Mohammed)
WHOIS   → _cmdWhois     (Mohammed)
JOIN    → _cmdJoin      (Noura)
PART    → _cmdPart      (Noura)
PRIVMSG → _cmdPrivmsg   (Noura)
KICK    → _cmdKick      (Noura)
INVITE  → _cmdInvite    (Noura)
TOPIC   → _cmdTopic     (Noura)
MODE    → _cmdMode      (Noura)
```

**The `_handleClientWrite` send algorithm:**
```
send(fd, buf, len, 0) = sent
if sent > 0:
    remaining = buf[sent..end]
    clear buffer
    if remaining not empty:
        add remaining back to buffer
if buffer is now empty:
    remove POLLOUT from event mask
```

---

## Client Module (`srcs/Client/`)

---

### `srcs/Client/Client.cpp`

**Purpose:** Client construction, identity management, channel tracking.

**Functions:**

| Function | What it does |
|---|---|
| `Client()` | Default constructor (private, unused). Sets fd=-1, all auth flags false |
| `Client(fd, hostname)` | Main constructor. Sets fd, stores IP as hostname, all auth flags false |
| `~Client()` | Empty destructor (no dynamic resources owned by Client) |
| `getFd()` | Returns socket fd |
| `hasPassAuth()` / `hasNickAuth()` / `hasUserAuth()` / `isRegistered()` | Auth state getters |
| `setPassAuth(val)` / `setNickAuth(val)` / `setUserAuth(val)` / `setRegistered(val)` | Auth state setters |
| `getNickname()` / `getUsername()` / `getHostname()` / `getRealname()` | Identity getters |
| `setNickname(nick)` / `setUsername(user)` / `setHostname(host)` / `setRealname(real)` | Identity setters |
| `getPrefix()` | Returns `"nick!user@host"` format |
| `addChannel(channel)` | Add channel to list (with duplicate check) |
| `removeChannel(channel)` | Remove channel from list via `std::find` + `vector::erase` |
| `isInChannel(channel)` | Check if channel name is in list |
| `getChannels()` | Returns const reference to channel list vector |

---

### `srcs/Client/ClientBuffer.cpp`

**Purpose:** Read and send buffer management.

**Functions:**

| Function | What it does |
|---|---|
| `appendToBuffer(data)` | Appends raw TCP data to `_readBuffer` |
| `extractMessages()` | **The TCP streaming handler.** Finds `\r\n` or `\n` delimiters, extracts complete messages, leaves partial data in buffer. Handles mixed line endings correctly |
| `getSendBuffer()` | Returns const reference to `_sendBuffer` (for `send()` call) |
| `appendToSendBuffer(data)` | Appends data to send buffer |
| `clearSendBuffer()` | Empties `_sendBuffer` |
| `hasPendingSend()` | Returns true if `_sendBuffer` is not empty |
| `sendMessage(msg)` | Appends message to `_sendBuffer` (NOTE: doesn't actually send!) |

**Buffer algorithm (extractMessages):**
```
while buffer contains \r\n or \n:
    extract text before delimiter
    remove delimiter from buffer
    if next char is \n (from \r\n case), remove it too
    if extracted text is non-empty, add to result list
```

**Important:** `sendMessage()` does NOT send data over the socket. It buffers it. The actual `send()` syscall happens later in `Server::_handleClientWrite()` when `POLLOUT` fires. This is correct for non-blocking I/O but the name is misleading.

---

## Channel Module (`srcs/Channel/`)

---

### `srcs/Channel/Channel.cpp`

**Purpose:** Channel construction, member/operator/invite management, topic, mode flags, broadcasting, mode string generation.

**Functions:**

| Function | What it does |
|---|---|
| `Channel()` | Default constructor. All flags false, limit 0 |
| `Channel(name)` | Constructor with channel name |
| `~Channel()` | Empty destructor (channels don't own clients — clients are owned by Server) |
| `getName()` | Returns channel name |
| `addMember(client)` | Adds client pointer to `_members` set |
| `removeMember(client)` | Removes from `_members`, `_operators`, AND `_invited` sets (cleanup) |
| `isMember(client)` | Checks if client is in `_members` (uses `set::find`) |
| `getMemberCount()` | Returns `_members.size()` |
| `getMembers()` | Returns reference to `_members` set |
| `addOperator(client)` / `removeOperator(client)` / `isOperator(client)` | Operator management |
| `addInvited(client)` / `removeInvited(client)` / `isInvited(client)` | Invitation management |
| `getTopic()` / `setTopic(topic)` | Topic management |
| Mode flag getters/setters: | `isInviteOnly()` / `setInviteOnly()`, `isTopicRestricted()` / `setTopicRestricted()`, `hasKey()` / `setKey()` / `getKey()` / `removeKey()`, `hasLimit()` / `setLimit()` / `getLimit()` / `removeLimit()` |
| `broadcast(message, exclude)` | Sends message to every member EXCEPT `exclude` (pass 0/NULL to send to everyone). Appends `\r\n` to each message |
| `getModeString()` | Returns active mode flags as string e.g. `"+itk"` |

---

### `srcs/Channel/ChannelManager.cpp`

**Purpose:** A wrapper around `map<string, Channel*>` with create/get/remove operations.

**Functions:**

| Function | What it does |
|---|---|
| `ChannelManager()` | Empty constructor |
| `~ChannelManager()` | Deletes all Channel objects in the map, then clears it |
| `getChannel(name)` | Returns pointer or NULL |
| `createChannel(name)` | `new Channel(name)`, inserts into map, returns pointer |
| `removeChannel(name)` | Deletes the Channel object and removes from map |

**Current status:** DEAD CODE. The `Server` class channels are managed directly through `Server::_channels` (a `map<string, Channel>` by value, not pointer). This file is compiled into the binary but never instantiated.

---

### `srcs/Channel/ChannelModes.cpp`

**Purpose:** Was intended to hold MODE command implementation.

**Current contents:**
```cpp
// Channel mode flag implementations live in Channel.cpp
```

**Current status:** STUB. All mode logic is implemented directly in `srcs/Commands/Mode.cpp` and the flag setters in `Channel.cpp`. This file is dead code.

---

## Command Module (`srcs/Commands/`)

Each file implements one IRC command as a method on the `Server` class.

---

### `srcs/Commands/Pass.cpp`

**Command:** `PASS <password>`

**Handler:** `Server::_cmdPass()`

**Logic:**
1. If already registered → `ERR_ALREADYREGISTERED`
2. If no password given → `ERR_NEEDMOREPARAMS`
3. Get password from params or trailing
4. If password doesn't match `_password` → `ERR_PASSWDMISMATCH`
5. ✅ → `client.setPassAuth(true)`

---

### `srcs/Commands/Nick.cpp`

**Command:** `NICK <nickname>`

**Handler:** `Server::_cmdNick()`

**Logic:**
1. If no nickname given → `ERR_NONICKNAMEGIVEN`
2. Validate nickname with `isValidNick()`:
   - Max 9 characters
   - First char must be alpha or special: `[\]`_^{|}`
   - Rest must be alphanumeric or special or `-`
   - Invalid → `ERR_ERRONEUSNICKNAME`
3. Check if nickname is already in use (case-insensitive via `toUpper()`) → `ERR_NICKNAMEINUSE`
4. If already registered (nick change): broadcast NICK change to all channels the client is in
5. Set new nickname → `client.setNickAuth(true)`
6. If both NICK and USER are complete: register client, send welcome (001-004)

**Nickname validation rules:**
- Length 1-9 characters
- Allowed first chars: letters + `[\]`_^{|}`
- Allowed other chars: letters + digits + `[\]`_^{|}-`

---

### `srcs/Commands/User.cpp`

**Command:** `USER <username> <hostname> <servername> :<realname>`

**Handler:** `Server::_cmdUser()`

**Logic:**
1. If already registered → `ERR_ALREADYREGISTERED`
2. If fewer than 3 params or no trailing → `ERR_NEEDMOREPARAMS`
3. Set username from `cmd.getParam(0)`
4. Set realname from `cmd.getTrailing()`
5. `client.setUserAuth(true)`
6. If NICK was already done: register client, send welcome (001-004)

**Note:** The `hostname` and `servername` params from the IRC protocol are accepted but ignored (they're traditionally placeholders).

---

### `srcs/Commands/Join.cpp`

**Command:** `JOIN <channel> [<key>]`

**Handler:** `Server::_cmdJoin()`

**Logic:**
1. ⚠️ **CRASH BUG:** Accesses `cmd.getParams()[0]` without checking if params is empty
2. Look up channel in `_channels` map
3. If channel doesn't exist: create it, make the joiner an operator
4. If already a member → return silently
5. If invite-only (`+i`) and not invited → `ERR_INVITEONLYCHAN`
6. If key required (`+k`) and wrong key provided → `ERR_BADCHANNELKEY`
7. If user limit (`+l`) reached → `ERR_CHANNELISFULL`
8. Add member → broadcast `:nick JOIN #channel`

**Missing features:**
- No NAMES reply (RPL_NAMREPLY 353 + RPL_ENDOFNAMES 366) after joining
- JOIN broadcast uses bare `client.getNickname()` instead of full prefix `client.getPrefix()`
- All error replies use the broken `_sendReply()` — missing channel name

---

### `srcs/Commands/Part.cpp`

**Command:** `PART <channel>`

**Handler:** `Server::_cmdPart()`

**Logic:**
1. ⚠️ **CRASH BUG:** Accesses `cmd.getParams()[0]` without checking if params is empty
2. Look up channel → if not found → `ERR_NOSUCHCHANNEL`
3. If not a member → `ERR_NOTONCHANNEL`
4. Broadcast `:nick PART #channel` to all members
5. Remove member from channel
6. If channel is empty → delete it from `_channels`

---

### `srcs/Commands/Privmsg.cpp`

**Command:** `PRIVMSG <target> :<message>`

**Handler:** `Server::_cmdPrivmsg()`

**Logic:**
1. ⚠️ **CRASH BUG:** Accesses `cmd.getParams()[0]` without checking if params is empty
2. Get target and message
3. If target starts with `#`:
   - Look up channel → if not found → `ERR_NOSUCHCHANNEL`
   - If sender not in channel → `ERR_NOTONCHANNEL`
   - Broadcast `:nick PRIVMSG #channel :message` to all members EXCEPT sender
4. Otherwise (private message):
   - Look up user by nickname → if not found → `ERR_NOSUCHNICK`
   - Send `:nick PRIVMSG target :message` directly to target client

**Note:** Broadcast uses `client.getNickname()` prefix instead of `client.getPrefix()` — missing `!user@host`.

---

### `srcs/Commands/Kick.cpp`

**Command:** `KICK <channel> <user> [:<reason>]`

**Handler:** `Server::_cmdKick()`

**Logic:**
1. ⚠️ **CRASH BUG:** Accesses `cmd.getParams()[0]` without checking if params is empty
2. Look up channel → if not found → `ERR_NOSUCHCHANNEL`
3. If kicker is not channel operator → `ERR_CHANOPRIVSNEEDED`
4. Look up target user → if not found → `ERR_NOSUCHNICK`
5. If target not in channel → `ERR_USERNOTINCHANNEL`
6. Default reason is kicker's nickname
7. Broadcast `:kicker KICK #channel target :reason` to all members
8. Remove target from channel
9. If channel empty → delete it

---

### `srcs/Commands/Invite.cpp`

**Command:** `INVITE <user> <channel>`

**Handler:** `Server::_cmdInvite()`

**Logic:**
1. ⚠️ **CRASH BUG:** Accesses `cmd.getParams()[0]` without checking if params is empty
2. Look up channel → if not found → `ERR_NOSUCHCHANNEL`
3. If inviter is not channel operator → `ERR_CHANOPRIVSNEEDED`
4. Look up target user → if not found → `ERR_NOSUCHNICK`
5. If target already in channel → `ERR_USERONCHANNEL`
6. `channel.addInvited(target)` — marks them as invited
7. Send `:inviter INVITE target #channel` to the TARGET user
8. Send confirmation to inviter via `_sendReply(RPL_INVITING)` — **BROKEN** (missing params)

---

### `srcs/Commands/Topic.cpp`

**Command:** `TOPIC <channel> [:<topic>]`

**Handler:** `Server::_cmdTopic()`

**Logic:**
1. ⚠️ **CRASH BUG:** Accesses `cmd.getParams()[0]` without checking if params is empty
2. Look up channel → if not found → `ERR_NOSUCHCHANNEL`
3. If not a member → `ERR_NOTONCHANNEL`
4. If no trailing (view topic):
   - If topic is empty → `RPL_NOTOPIC`
   - Otherwise → `RPL_TOPIC`
5. If trailing exists (set topic):
   - If topic is restricted (`+t`) and not operator → `ERR_CHANOPRIVSNEEDED`
   - Set topic → broadcast `:nick TOPIC #channel :new topic`

**Error replies broken:** Uses `_sendReply()` which sends incomplete format.

---

### `srcs/Commands/Mode.cpp`

**Command:** `MODE <channel> <flags> [<args>]`

**Handler:** `Server::_cmdMode()`

**Parses flag strings like:** `+i-t+k key o nick`

**Logic:**
1. ⚠️ **CRASH BUG:** Accesses `cmd.getParams()[0]` without checking if params is empty
2. Look up channel → if not found → `ERR_NOSUCHCHANNEL`
3. If not operator → `ERR_CHANOPRIVSNEEDED`
4. Iterate through mode string character by character:
   - `+` → start adding modes
   - `-` → start removing modes
   - `i` → set/unset invite-only
   - `t` → set/unset topic restriction
   - `k` → set/remove channel key (needs argument when adding)
   - `l` → set/remove user limit (needs argument when adding)
   - `o` → give/take operator privilege (needs nickname argument)

**Missing:** Does NOT broadcast mode changes to the channel. Other users don't see mode changes.

---

### `srcs/Commands/Quit.cpp`

**Command:** `QUIT [:<reason>]`

**Handler:** `Server::_cmdQuit()`

**Logic:**
1. Get reason from trailing (default: "Client Quit")
2. For each channel the client is in: broadcast `:prefix QUIT :reason`
3. Send `ERROR :Closing link...` message to the quitting client
4. `_removeClient(fd)` — cleans up client from everything

**Why send `ERROR`:** The client gets one last message before their connection is closed. The `ERROR` message is the IRC standard way to inform a client they're being disconnected.

---

### `srcs/Commands/Ping.cpp`

**Command:** `PING <token>`

**Handler:** `Server::_cmdPing()`

**Logic:**
1. If no params → `ERR_NOORIGIN`
2. Respond: `:server PONG server :<token>`

**Purpose:** Keepalive mechanism. Clients send PING to verify the connection is alive. Server must respond with PONG.

---

### `srcs/Commands/Whois.cpp`

**Command:** `WHOIS <nickname>`

**Handler:** `Server::_cmdWhois()`

**Logic:**
1. If no params → `ERR_NONICKNAMEGIVEN`
2. Look up user by nickname:
   - Not found → `ERR_NOSUCHNICK`
   - Found → `RPL_WHOISUSER` (311) with username, hostname, realname
3. Always → `RPL_ENDOFWHOIS` (318)

**RPL_WHOISUSER format:**
```
:server 311 requester target username hostname * :realname
```

---

## Parser Module (`srcs/Command/`)

---

### `srcs/Command/Command.cpp`

**Purpose:** The IRC protocol parser. Converts raw text into structured `Command` objects.

**Functions:**

| Function | What it does |
|---|---|
| `Command()` | Default constructor, sets `_hasTrailing = false` |
| `~Command()` | Empty destructor |
| `static parse(raw)` | **THE PARSER.** Strips trailing `\r\n`, extracts prefix if present, extracts command name (uppercased), splits params, captures trailing after `:` |
| `getPrefix()` / `getCommand()` / `getParams()` / `getTrailing()` / `hasTrailing()` | Accessors |
| `getParam(index)` / `paramCount()` | Convenience accessors |

**Parse algorithm:**
```
1. Strip trailing \r\n
2. If starts with ':': extract prefix up to first space
3. Extract command (up to next space, uppercase it)
4. Extract params (space-separated tokens)
5. If ':' found: everything after is the trailing parameter
```

**Examples:**
```
"NICK alice\r\n"
    → command: "NICK", params: ["alice"]

":nick!user@host PRIVMSG #test :Hello\r\n"
    → prefix: "nick!user@host", command: "PRIVMSG", params: ["#test"], trailing: "Hello"

"JOIN #test\r\n"
    → command: "JOIN", params: ["#test"]
```

---

## Utilities (`srcs/Utils/`)

---

### `srcs/Utils/Utils.cpp`

**Purpose:** Small utility functions used across the codebase.

**Functions:**

| Function | What it does |
|---|---|
| `toUpper(str)` | Returns a new string with all characters uppercased via `std::toupper()`. Used for case-insensitive nickname comparison in IRC |
| `trim(str)` | Removes leading and trailing whitespace |
| `split(str, delimiter)` | Splits a string on a delimiter character. Handles consecutive delimiters gracefully (doesn't produce empty tokens) |
| `sendToClient(fd, message)` | **STUB.** Empty function body. Never called anywhere. Dead code |

**Where `toUpper()` is used:**
- `_findClientByNick()` — case-insensitive nickname lookup
- `Command::parse()` — command names are uppercased for dispatch

---

# Data Flow Summary

## How a message travels through the system:

```
1. Client sends: "JOIN #test\r\n"
2. OS puts data in TCP buffer
3. poll() returns POLLIN for this fd
4. Server::_handleClientData(fd)
   → recv() reads raw bytes
   → Client::appendToBuffer(data)
   → Client::extractMessages() → ["JOIN #test"]
5. Server::_processMessage(client, "JOIN #test")
   → Command::parse("JOIN #test")
     → command: "JOIN", params: ["#test"]
   → Server::_dispatchCommand(client, cmd)
6. Auth gate check: is registered? YES (or handles pre-auth)
7. _cmdMap lookup: "JOIN" → &Server::_cmdJoin
8. Server::_cmdJoin(client, cmd)
   → finds or creates Channel
   → checks +i, +k, +l restrictions
   → Channel::addMember(client)
   → Channel::broadcast(":nick JOIN #test")
     → for each member: Client::sendMessage(msg)
     → appends to each client's _sendBuffer
9. Next poll() iteration:
   → hasPendingSend() is true for member clients
   → poll() events includes POLLOUT
10. Server::_handleClientWrite(fd)
    → Client::getSendBuffer()
    → send(fd, ...) — actual TCP send
    → if partial: buffer remaining
    → if done: remove POLLOUT
```

---

# Known Issues Summary

| Severity | Issue                                                                | Files                                              |
| -------- | -------------------------------------------------------------------- | -------------------------------------------------- |
| 🔴 P0    | 7 commands access `[0]` without empty check                          | Join, Part, Privmsg, Kick, Invite, Topic, Mode     |
| 🔴 P0    | `_sendReply()` missing required parameters                           | ServerCommands.cpp (affects all commands using it) |
| 🟠 P1    | No NAMES reply on JOIN                                               | Join.cpp                                           |
| 🟠 P1    | No MODE change broadcast                                             | Mode.cpp                                           |
| 🟠 P1    | No MOTD after registration                                           | Nick.cpp, User.cpp                                 |
| 🟠 P1    | JOIN broadcast uses bare nick not full prefix                        | Join.cpp                                           |
| 🟡 P2    | INVITE reply missing target+channel                                  | Invite.cpp                                         |
| 🟢 P3    | README.md empty                                                      | README.md                                          |
| 🟢 P3    | Dead code: ChannelManager.cpp, ChannelModes.cpp                      | Multiple                                           |
| 🟢 P3    | `sendToClient()` unused stub                                         | Utils.cpp                                          |
| 🟢 P3    | Duplicate API: `sendMessage()` / `appendToSendBuffer()`              | ClientBuffer.cpp                                   |
| 🟢 P3    | No buffer size limits                                                | ClientBuffer.cpp                                   |
| 🔵 Info  | Architecture Blueprint mentions `ChannelOps.cpp` which doesn't exist | docs/                                              |
| 🔵 Info  | `structure.txt` outdated (missing new files)                         | structure.txt                                      |

---

*Generated from reading all 25+ source files, headers, Makefile, docs, and git history.*
