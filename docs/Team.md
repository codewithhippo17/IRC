## ft_irc Team: Hippo + Mohammed + Noura

### Roles

| Person | Role | Branches | Focus |
|--------|------|----------|-------|
| **Mohammed** | The Core | `feature/core-poll-loop` → `feature/core-auth` | Server engine, Client, Parser, Auth, Utils |
| **Noura** | The World | `feature/channel-join` → `feature/channel-modes` | Channel, Join/Part, Privmsg, Kick/Invite/Topic/Mode |
| **Hippo** | Maintainer | `develop` + `master` | All `.hpp` files, Replies.hpp, tests/, merge gate |

---

### File Ownership

| File | Owner |
|------|-------|
| `Server.hpp`, `Client.hpp`, `Channel.hpp`, `Command.hpp`, `Replies.hpp`, `Utils.hpp`, `Colors.hpp` | **Hippo** |
| `Server.cpp`, `ServerRun.cpp`, `ServerClient.cpp` | Mohammed |
| `Client.cpp`, `ClientBuffer.cpp` | Mohammed |
| `Command.cpp` | Mohammed |
| `Pass.cpp`, `Nick.cpp`, `User.cpp`, `Quit.cpp` | Mohammed |
| `Utils.cpp`, `Replies.cpp` | Mohammed |
| `main.cpp`, `Makefile` | Mohammed |
| `Channel.cpp` | Noura |
| `Join.cpp`, `Part.cpp` | Noura |
| `Privmsg.cpp` | Noura |
| `Kick.cpp`, `Invite.cpp`, `Topic.cpp`, `Mode.cpp` | Noura |
| `tests/*.sh` | **Hippo** |

**Rule:** Mohammed and Noura never modify `.hpp` files. Only Hippo touches headers.

---

### Branch Structure

```
master (protected) ← Hippo merges from develop
  └── develop ← all PRs merge here
      ├── feature/core-poll-loop   ← Mohammed
      ├── feature/core-auth        ← Mohammed
      ├── feature/channel-join     ← Noura
      └── feature/channel-modes    ← Noura
```

**master protection (GitHub):** Require PR + 1 approval + dismiss stale + linear history.

---

### Workflow

#### Mohammed & Noura

```bash
# Daily
git checkout develop && git pull origin develop
git checkout <feature-branch> && git merge develop
# code, compile, commit
git push origin <feature-branch>

# When feature is done → open PR
# base: develop ← compare: <feature-branch>
# assign Hippo as reviewer, wait for merge

# After PR merged
git checkout develop && git pull origin develop
git checkout -b <next-feature-branch> develop
```

#### Hippo (Maintainer)

```bash
# Review PRs → pull develop → test
git checkout develop && git pull
make && ./ircserv
valgrind --leak-check=full ./ircserv

# Merge to master when stable
git checkout master && git merge develop && git push origin master
```

---

### Merge Point: Where Mohammed and Noura Meet

The only place both touch is `Server::_dispatchCommand()`:

```cpp
void Server::_dispatchCommand(Client& client, const Command& cmd) {
    std::string cmdName = cmd.getCommand();

    // Mohammed — auth commands
    if (cmdName == "PASS") return _cmdPass(client, cmd);
    if (cmdName == "NICK") return _cmdNick(client, cmd);
    if (cmdName == "USER") return _cmdUser(client, cmd);
    if (cmdName == "QUIT") return _cmdQuit(client, cmd);

    if (!client.isAuthenticated())
        return sendReply(client, ERR_NOTREGISTERED);

    // Noura — channel commands
    if (cmdName == "JOIN") return _cmdJoin(client, cmd);
    if (cmdName == "PART") return _cmdPart(client, cmd);
    if (cmdName == "PRIVMSG") return _cmdPrivmsg(client, cmd);
    if (cmdName == "TOPIC") return _cmdTopic(client, cmd);
    if (cmdName == "INVITE") return _cmdInvite(client, cmd);
    if (cmdName == "KICK") return _cmdKick(client, cmd);
    if (cmdName == "MODE") return _cmdMode(client, cmd);

    sendReply(client, ERR_UNKNOWNCOMMAND);
}
```

Hippo writes the skeleton. Mohammed fills auth. Noura fills channels. Neither modifies the other's block.

---

### Testing

| Person | Test |
|--------|------|
| Mohammed | `nc localhost 6667` → PASS → NICK → USER → receive `001` welcome |
| Noura | Two clients → join `#test` → chat → set `+k` → kick one user |
| Hippo | `valgrind --leak-check=full ./ircserv` before each merge to master |

---

### Risks

| Risk | Mitigation |
|------|------------|
| Mohammed blocked waiting for channels | Mohammed creates a mock `#test` channel in Server for testing |
| Noura needs auth to test JOIN | Hippo provides `forceAuth()` debug method (removed before eval) |
| Merge conflicts in headers | Hippo owns headers. No one else modifies `.hpp` |
| Feature branches touch same file | Hippo handles the merge and resolves conflicts |
