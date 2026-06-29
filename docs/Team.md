Got it — you're a team of **3 total**, and you're the maintainer. So it's **2 developers + you**.

Let me redesign the split for a leaner team.

---

## 🏗️ ft_irc Team: 3 Total (You + 2 Devs)

With only 2 developers, the split becomes **vertical slices** rather than horizontal layers. Each dev owns a **feature domain** end-to-end.

---

### 👤 Developer A — "The Core"
**Focus:** Server engine + Client lifecycle + Authentication

| Owns | Why |
|------|-----|
| `Server.hpp/cpp`, `ServerRun.cpp`, `ServerClient.cpp` | Socket setup, poll loop, accept, remove |
| `Client.hpp/cpp` | Buffer, auth state, getters/setters |
| `main.cpp`, `Makefile` | Entry point, build |
| `Command.hpp/cpp` | Parser (message → tokens) |
| `Pass.cpp`, `Nick.cpp`, `User.cpp`, `Quit.cpp` | Auth sequence |
| `Utils.cpp`, `Replies.cpp` | Helpers, numeric codes |

**Skills:** Socket programming, `poll()`, non-blocking I/O, buffer management, IRC protocol basics

**Test:** `nc localhost 6667` → PASS → NICK → USER → receive `001` welcome

---

### 👤 Developer B — "The World"
**Focus:** Channels + Messaging + Operator commands

| Owns | Why |
|------|-----|
| `Channel.hpp/cpp` | Channel state, membership, modes |
| `Join.cpp`, `Part.cpp` | Channel entry/exit |
| `Privmsg.cpp`, `Notice.cpp` | User-to-user and user-to-channel messaging |
| `Topic.cpp`, `Invite.cpp`, `Kick.cpp` | Operator commands |
| `Mode.cpp`, `iMode.cpp`, `tMode.cpp`, `kMode.cpp`, `oMode.cpp`, `lMode.cpp` | Mode parsing & handling |

**Skills:** Set operations, access control, state machines, broadcasting, IRC channel semantics

**Test:** Two irssi clients → join `#test` → chat → set `+k` → kick one user

---

### 👤 YOU — "The Maintainer"
**Focus:** Interfaces, integration, quality gate

| You Own            | You Do                                      |
| ------------------ | ------------------------------------------- |
| All `.hpp` files   | Define APIs before anyone starts            |
| `Replies.hpp`      | Centralize all IRC numeric codes            |
| `tests/` directory | Write shell scripts for integration testing |
| `README.md`        | Documentation                               |
| Merge control      | Only you merge to `develop`                 |

**Your daily workflow:**
1. Review PRs from both devs
2. Pull `develop`, test compilation
3. Fix header conflicts (the #1 team killer)
4. Run `valgrind --leak-check=full`
5. Update task board

---

## 🌿 Git Strategy (3-Person)

```
main (protected) ◄── develop (protected) ◄── feature/*
     ▲                      ▲
     │                      │
   [YOU]                [YOU MERGE]
   - Tag releases       - After review
   - Final eval prep    - Integration test
```

**Branch naming:**
- `feature/core-poll-loop` (Dev A)
- `feature/core-auth` (Dev A)
- `feature/channel-join` (Dev B)
- `feature/channel-modes` (Dev B)
- `fix/buffer-crash` (whoever)

**Rule:** Both devs push feature branches. You review, you merge. No direct pushes to `develop`.

---

## 📋 Development Phases (3-Person Sprint)

| Phase | Days | What | Who |
|-------|------|------|-----|
| **1. Interfaces** | 1-2 | You write all `.hpp` files. Both devs review. | YOU |
| **2. Core Engine** | 3-5 | Dev A: socket, poll, accept, buffer, Client state | Dev A |
| **3. Auth** | 6-8 | Dev A: PASS, NICK, USER, welcome reply | Dev A |
| **4. Channels** | 6-10 | Dev B: Channel class, JOIN, PART | Dev B |
| **5. Messaging** | 11-13 | Dev B: PRIVMSG, NOTICE + Dev A integrates broadcast | Dev B + You |
| **6. Ops** | 14-17 | Dev B: MODE, KICK, INVITE, TOPIC | Dev B |
| **7. Polish** | 18-20 | Bug fixes, valgrind, tests, edge cases | ALL |

**Overlap note:** Phases 2-4 and 4-6 run in parallel. Dev A works on auth while Dev B works on channels. They only meet at the `JOIN` command boundary.

---

## 🔗 The Critical Interface: Where A and B Meet

The **only** place Dev A and Dev B touch is in `Server::_dispatchCommand()`:

```cpp
// Server.cpp — Dev A owns this function body
void Server::_dispatchCommand(Client& client, const Command& cmd) {
    std::string cmdName = cmd.getCommand();
    
    // Auth commands — Dev A implemented
    if (cmdName == "PASS") return _cmdPass(client, cmd);
    if (cmdName == "NICK") return _cmdNick(client, cmd);
    if (cmdName == "USER") return _cmdUser(client, cmd);
    
    // Block non-authenticated users
    if (!client.isAuthenticated())
        return sendReply(client, ERR_NOTREGISTERED);
    
    // Channel commands — Dev B implemented
    if (cmdName == "JOIN") return _cmdJoin(client, cmd);
    if (cmdName == "PART") return _cmdPart(client, cmd);
    if (cmdName == "PRIVMSG") return _cmdPrivmsg(client, cmd);
    if (cmdName == "NOTICE") return _cmdNotice(client, cmd);
    if (cmdName == "TOPIC") return _cmdTopic(client, cmd);
    if (cmdName == "INVITE") return _cmdInvite(client, cmd);
    if (cmdName == "KICK") return _cmdKick(client, cmd);
    if (cmdName == "MODE") return _cmdMode(client, cmd);
    
    // Dev A implemented
    if (cmdName == "QUIT") return _cmdQuit(client, cmd);
    
    sendReply(client, ERR_UNKNOWNCOMMAND);
}
```

**This is your merge point.** You write this skeleton. Dev A fills auth commands. Dev B fills channel commands. Neither touches the other's block.

---

## 📁 File Ownership (3-Person)

| File | Owner | Notes |
|------|-------|-------|
| `Server.hpp` | **YOU** | Interface contract |
| `Client.hpp` | **YOU** | Dev A implements |
| `Channel.hpp` | **YOU** | Dev B implements |
| `Command.hpp` | **YOU** | Dev A implements parser |
| `Replies.hpp` | **YOU** | Centralized codes |
| `Server.cpp`, `ServerRun.cpp`, `ServerClient.cpp` | Dev A | Core engine |
| `Client.cpp` | Dev A | Buffer, auth state |
| `main.cpp`, `Makefile` | Dev A | Entry, build |
| `Command.cpp` | Dev A | Parser |
| `Pass.cpp`, `Nick.cpp`, `User.cpp`, `Quit.cpp` | Dev A | Auth |
| `Utils.cpp`, `Replies.cpp` | Dev A | Helpers |
| `Channel.cpp` | Dev B | Membership, modes |
| `Join.cpp`, `Part.cpp` | Dev B | Channel entry/exit |
| `Privmsg.cpp`, `Notice.cpp` | Dev B | Messaging |
| `Topic.cpp`, `Invite.cpp`, `Kick.cpp` | Dev B | Operator commands |
| `Mode.cpp`, `*Mode.cpp` | Dev B | Mode handlers |
| `tests/*.sh` | **YOU** | Integration tests |

---

## 🚨 3-Person Specific Rules

| Risk | Mitigation |
|------|------------|
| **Dev A blocked waiting for Dev B's channel to test PRIVMSG** | Dev A creates a mock `#test` channel in `Server` for testing messaging |
| **Dev B needs client auth state to test JOIN** | You provide a `forceAuth()` debug method in `Client.hpp` (removed before eval) |
| **Only 2 devs = bus factor** | You understand both domains enough to step in |
| **Merge conflicts** | You own headers. Devs never modify `.hpp` without your approval |

---

## Your First Commit

```bash
# Create repo structure
mkdir -p ft_irc/{includes,srcs/{server,client,channel,commands,utils},tests}
cd ft_irc
git init
git checkout -b develop

# Write all headers (your job)
# includes/Server.hpp, Client.hpp, Channel.hpp, Command.hpp, Replies.hpp

# Skeleton Makefile
# make compiles empty .cpp files → ircserv binary

git add .
git commit -m "init: interfaces and skeleton"
git push -u origin develop
```

Then tell Dev A and Dev B: *"Pull `develop`. Your headers are ready. Implement the body. Don't touch the header."*

Want me to generate the actual header files now?
