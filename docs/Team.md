## ft_irc Team: Hippo + Mohammed + Noura

### Roles

| Person | Role | Branches | Owns |
|--------|------|----------|------|
| **Mohammed** | The Core | `feature/core-poll-loop` → `feature/core-auth` | Server engine, Client state, Parser, Auth commands (Pass/Nick/User/Quit), Utils, main.cpp |
| **Noura** | The World | `feature/channel-join` → `feature/channel-modes` | Channel, Join/Part, Privmsg, Operator commands (Kick/Invite/Topic/Mode) |
| **Hippo** | Maintainer | `develop` + `master` | All `.hpp` files, Replies.hpp, tests/, README, merge control |

### Branch Flow

```
master (protected) ← Hippo merges from develop
  └── develop ← PRs merge here
      ├── feature/core-poll-loop   ← Mohammed
      ├── feature/core-auth        ← Mohammed
      ├── feature/channel-join     ← Noura
      └── feature/channel-modes    ← Noura
```

### Workflow

**Mohammed & Noura — daily:**
```bash
git checkout develop && git pull origin develop
git checkout <feature-branch> && git merge develop
# code, commit, push
git push origin <feature-branch>
```

**PR when done:** base=`develop` → compare=`<feature-branch>` → assign **Hippo** → wait for merge.

**Hippo — maintainer:**
```bash
# Review PRs, then:
git checkout develop && git pull
make && ./ircserv                        # test build
valgrind --leak-check=full ./ircserv     # check leaks
git checkout master && git merge develop && git push origin master
```

### Golden Rules

- No direct pushes to `develop` or `master` — all code through PRs
- Mohammed & Noura never modify `.hpp` files — only Hippo
- Always pull `develop` before starting work
- Feature branches only — no work on `develop` or `master`
- Hippo merges everything

### Merge Point

The only place Mohammed and Noura's code touches is `Server::_dispatchCommand()`:

```cpp
void Server::_dispatchCommand(Client& client, const Command& cmd) {
    if (cmdName == "PASS" || "NICK" || "USER" || "QUIT")  // Mohammed
    if (cmdName == "JOIN" || "PART" || "PRIVMSG" || ...)  // Noura
}
```

Hippo writes the skeleton. Mohammed fills auth. Noura fills channels. Neither touches the other's block.
