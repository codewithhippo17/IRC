## 🏗️ ft_irc Team: Hippo + Mohammed + Noura

### 👤 Mohammed — "The Core"
**Focus:** Server engine + Client lifecycle + Authentication

Branches: `feature/core-poll-loop` → `feature/core-auth`

| Owns | Files |
|------|-------|
| Server engine | `Server.cpp`, `ServerRun.cpp`, `ServerClient.cpp` |
| Client state | `Client.cpp`, `ClientBuffer.cpp` |
| Parser | `Command.cpp` |
| Auth commands | `Pass.cpp`, `Nick.cpp`, `User.cpp`, `Quit.cpp` |
| Build & entry | `main.cpp`, `Makefile` |
| Helpers | `Utils.cpp`, `Replies.cpp` |

**Test:** `nc localhost 6667` → PASS → NICK → USER → receive `001` welcome

---

### 👤 Noura — "The World"
**Focus:** Channels + Messaging + Operator commands

Branches: `feature/channel-join` → `feature/channel-modes`

| Owns | Files |
|------|-------|
| Channel state | `Channel.cpp` |
| Entry/exit | `Join.cpp`, `Part.cpp` |
| Messaging | `Privmsg.cpp` |
| Operator commands | `Kick.cpp`, `Invite.cpp`, `Topic.cpp` |
| Mode handling | `Mode.cpp` |

**Test:** Two irssi clients → join `#test` → chat → set `+k` → kick one user

---

### 👤 Hippo (You) — "The Maintainer"
**Focus:** Interfaces, integration, quality gate

| You Own | You Do |
|---------|--------|
| All `.hpp` files | Define APIs before anyone starts |
| `Replies.hpp` | Centralize all IRC numeric codes |
| `tests/` directory | Write shell scripts for integration testing |
| `README.md` | Documentation |
| Merge control | Only you merge to `develop` and `master` |

---

## 🌿 Branch Structure

```
master (protected)          ← only you merge from develop
  └── develop               ← integration branch, PRs merge here
      ├── feature/core-poll-loop   ← Mohammed
      ├── feature/core-auth        ← Mohammed
      ├── feature/channel-join     ← Noura
      └── feature/channel-modes    ← Noura
```

**Branch protection on `master` (GitHub):**
- Require pull request before merging
- Require 1 approval
- Dismiss stale approvals
- Require linear history

---

## 📋 Workflow for Mohammed & Noura

### First time setup
```bash
git clone git@github.com:codewithhippo17/IRC.git
cd IRC

# Mohammed
git checkout -b feature/core-poll-loop origin/feature/core-poll-loop

# Noura
git checkout -b feature/channel-join origin/feature/channel-join
```

### Daily
```bash
# 1. Get latest from develop
git checkout develop
git pull origin develop
git checkout <your-feature-branch>
git merge develop

# 2. Code, compile, commit
# ...

# 3. Push your feature branch
git push origin <your-feature-branch>
```

### Open a PR (when feature is done)
1. Go to https://github.com/codewithhippo17/IRC
2. **base:** `develop` ← **compare:** `your-feature-branch`
3. Title: `feat: what you implemented`
4. Assign reviewer: **Hippo**
5. Wait for review — Hippo merges

### After PR is merged
```bash
git checkout develop
git pull origin develop
git checkout -b <next-feature-branch> develop
```

---

## 📋 Your Workflow (Hippo — Maintainer)

```bash
# 1. Review open PRs from Mohammed and Noura
# 2. Pull develop and test
git checkout develop
git pull origin develop
make && ./ircserv
# 3. Run valgrind
valgrind --leak-check=full ./ircserv
# 4. Merge to master when stable
git checkout master
git merge develop
git push origin master
```

---

## ⚠️ Golden Rules

| Rule | Why |
|------|-----|
| **Never push directly to `develop` or `master`** | All code goes through PRs |
| **Never modify `.hpp` files** (Mohammed & Noura) | Only Hippo touches headers |
| **Always pull `develop` before starting work** | Avoid merge conflicts |
| **Feature branches only** | No work on `develop` or `master` |
| **Hippo merges everything** | Single gate for quality control |

---

## 🔗 The Critical Interface: Where Mohammed and Noura Meet

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

## 📁 File Ownership

| File | Owner | Notes |
|------|-------|-------|
| `Server.hpp` | **Hippo** | Interface contract |
| `Client.hpp` | **Hippo** | Mohammed implements |
| `Channel.hpp` | **Hippo** | Noura implements |
| `Command.hpp` | **Hippo** | Mohammed implements parser |
| `Replies.hpp` | **Hippo** | Centralized codes |
| `Server.cpp`, `ServerRun.cpp`, `ServerClient.cpp` | Mohammed | Core engine |
| `Client.cpp`, `ClientBuffer.cpp` | Mohammed | Buffer, auth state |
| `main.cpp`, `Makefile` | Mohammed | Entry, build |
| `Command.cpp` | Mohammed | Parser |
| `Pass.cpp`, `Nick.cpp`, `User.cpp`, `Quit.cpp` | Mohammed | Auth |
| `Utils.cpp`, `Replies.cpp` | Mohammed | Helpers |
| `Channel.cpp` | Noura | Membership, modes |
| `Join.cpp`, `Part.cpp` | Noura | Channel entry/exit |
| `Privmsg.cpp` | Noura | Messaging |
| `Kick.cpp`, `Invite.cpp`, `Topic.cpp` | Noura | Operator commands |
| `Mode.cpp` | Noura | Mode handler |
| `tests/*.sh` | **Hippo** | Integration tests |

---

## 🚨 Team Rules

| Risk | Mitigation |
|------|------------|
| **Mohammed blocked waiting for Noura's channel to test PRIVMSG** | Mohammed creates a mock `#test` channel in `Server` for testing messaging |
| **Noura needs client auth state to test JOIN** | Hippo provides a `forceAuth()` debug method in `Client.hpp` (removed before eval) |
| **Merge conflicts** | Hippo owns headers. Mohammed and Noura never modify `.hpp` without approval |
| **Different feature branches touching same file** | Hippo handles the merge and resolves conflicts |
