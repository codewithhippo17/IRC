# IRC Server — Evaluation Testing Cheatsheet

Complete test matrix for 42 ft_irc defense.

---

## 🔧 Setup — 4 Terminals Before Each Session

```bash
# Terminal 1: Server with valgrind
valgrind --leak-check=full --track-fds=yes ./ircserv 6667 1234

# Terminal 2: netcat for raw protocol tests
nc localhost 6667

# Terminal 3 & 4: irssi clients
irssi --home=/tmp/irc-alice --nick=alice -c localhost -p 6667 -w 1234
irssi --home=/tmp/irc-bob --nick=bob -c localhost -p 6667 -w 1234
```

Optional extra clients:
```bash
irssi --home=/tmp/irc-slaissam --nick=slaissam -c localhost -p 6667 -w 1234
irssi --home=/tmp/irc-hippo --nick=hippo -c localhost -p 6667 -w 1234
```

---

## 🟢 Authentication

| # | Test | Command (nc) | Expected Reply |
|---|------|-------------|----------------|
| 1 | Wrong password | `PASS wrong` → `NICK t` → `USER t t t :t` | `464 ERR_PASSWDMISMATCH` |
| 2 | No PASS at all | `NICK t` → `USER t t t :t` | `451 ERR_NOTREGISTERED` |
| 3 | NICK before PASS | `NICK t` (no PASS sent) | `451 :You must send PASS first` |
| 4 | Duplicate nick | Alice already connected, then `NICK alice` from nc | `433 ERR_NICKNAMEINUSE` |
| 5 | Invalid nick `#bad` | `PASS 1234` → `NICK #bad` | `432 ERR_ERRONEUSNICKNAME` |
| 6 | Empty nick | `PASS 1234` → `NICK` (no argument) | `431 ERR_NONICKNAMEGIVEN` |
| 7 | Re-register | `USER x x x :x` after already registered | `462 ERR_ALREADYREGISTERED` |

---

## 🟡 Channel Join / Part

| # | Test | Alice | Bob / nc | Expected |
|---|------|-------|-----------|----------|
| 8 | Basic join | `/join #test` | `/join #test` | Both in, Alice gets `@` |
| 9 | NAMES on join | — | `/join #test` | Nicklist populated immediately |
| 10 | Topic synced on join | Alice: `/topic #test hello` | Bob: `/join #test` | Bob sees topic on entry |
| 11 | Part channel | — | `/part #test` | Bob leaves, Alice sees PART |
| 12 | Last one out | Alice: `/part #test` (after bob) | — | Channel destroyed |

---

## 🟠 Channel Modes

Test each mode independently. Create a fresh channel each time.

### +i — Invite Only

| # | Test | Command | Expected |
|---|------|---------|----------|
| 13 | Set +i | Alice: `/mode #x +i` | Channel invite-only |
| 14 | Non-invited blocked | Bob: `/join #x` | `473 ERR_INVITEONLYCHAN` |
| 15 | Invite bypasses +i | Alice: `/invite bob #x` → Bob: `/join #x` | Bob joins |
| 16 | Invite consumed after join | Bob parts, rejoins without invite | Blocked again |

### +t — Topic Lock

| # | Test | Command | Expected |
|---|------|---------|----------|
| 17 | Set +t | Alice: `/mode #x +t` | Topic restricted |
| 18 | Non-op can't change | Bob: `/topic #x new` | `482 ERR_CHANOPRIVSNEEDED` |
| 19 | Non-op can view | Bob: `/topic #x` | Shows topic (no error) |
| 20 | Op can change | Alice: `/topic #x hello` | Topic updates |

### +k — Channel Key

| # | Test | Command | Expected |
|---|------|---------|----------|
| 21 | Set +k | Alice: `/mode #x +k secret` | Channel keyed |
| 22 | Join with key | Bob: `/join #x secret` | Joins |
| 23 | Join without key | slaissam: `/join #x` | `475 ERR_BADCHANNELKEY` |
| 24 | Join with wrong key | slaissam: `/join #x wrong` | `475` |
| 25 | Remove key | Alice: `/mode #x -k` | Anyone can join |

### +l — User Limit

| # | Test | Command | Expected |
|---|------|---------|----------|
| 26 | Set +l 2 | Alice: `/mode #x +l 2` | Max 2 users |
| 27 | Limit hit | Bob + slaissam join → hippo tries | `471 ERR_CHANNELISFULL` |
| 28 | Invited bypasses +l | Alice: `/invite hippo #x` → hippo joins | Hippo joins despite limit |
| 29 | Remove limit | Alice: `/mode #x -l` | No limit |

### +o — Give/Take Operator

| # | Test | Command | Expected |
|---|------|---------|----------|
| 30 | Give op | Alice: `/mode #x +o bob` | Bob gets `@` |
| 31 | New op kicks | Bob: `/kick #x slaissam` | Works (bob is now op) |
| 32 | Remove op | Alice: `/mode #x -o bob` | Bob loses `@` |
| 33 | Ex-op can't kick | Bob: `/kick #x alice` | `482` (no longer op) |

---

## 🔴 Channel Operator Commands

| # | Test | Command | Expected |
|---|------|---------|----------|
| 34 | Kick | Alice: `/kick #x bob reason` | Bob removed, sees kick msg |
| 35 | Kick non-op | Bob: `/kick #x alice` | `482 ERR_CHANOPRIVSNEEDED` |
| 36 | Kick non-member | Alice: `/kick #x nobody` | `441 ERR_USERNOTINCHANNEL` |
| 37 | Kick nonexistent nick | Alice: `/kick #x ghost` | `401 ERR_NOSUCHNICK` |
| 38 | Invite | Alice: `/invite bob #x` | Bob sees invite, can join |
| 39 | Invite non-op | Bob: `/invite slaissam #x` | `482` |
| 40 | Invite nonexistent nick | Alice: `/invite ghost #x` | `401` |

---

## 🔵 Messaging

| # | Test | Command | Expected |
|---|------|---------|----------|
| 41 | Channel message | Alice: `hello world` in #channel | Everyone in channel sees it |
| 42 | Private message | Alice: `/msg bob hey` | Only bob sees it |
| 43 | PM nonexistent nick | Alice: `/msg ghost hi` | `401 ERR_NOSUCHNICK` |
| 44 | PM to channel not in | From nc after auth | `442 ERR_NOTONCHANNEL` |

---

## 🟣 Nick & WHOIS

| # | Test | Command | Expected |
|---|------|---------|----------|
| 45 | Nick change | Alice: `/nick alice2` | All channels see NICK change |
| 46 | Nick to existing | Bob: `/nick alice2` | `433` (alice2 taken) |
| 47 | WHOIS | Alice: `/whois bob` | Shows bob info (`311` → `318`) |
| 48 | WHOIS nonexistent | Alice: `/whois ghost` | `401` |

---

## ⚫ Protocol / Edge Cases

| # | Test | How | Watch For |
|---|------|-----|-----------|
| 49 | Partial command | nc: type `JOIN` → wait 5s → ` #test\r\n` | Server buffers, doesn't crash |
| 50 | Ctrl+C a client | Kill bob's terminal | Alice sees QUIT, server stable |
| 51 | Ctrl+D client | nc: press Ctrl+D | Server cleans up fd, no crash |
| 52 | Kill server | Ctrl+C server during active chat | No leaks in valgrind |
| 53 | Flood test | nc: send 50+ messages rapidly | Server stable, no dropped clients |
| 54 | Empty command | nc: send `\r\n` | Server ignores, no crash |
| 55 | Long message | nc: 500-char PRIVMSG | Delivered or truncated gracefully |
| 56 | Multiple channels | Alice joins #a, #b, #c simultaneously | All three work independently |
| 57 | Mode on non-existent chan | nc: `MODE #ghost +i` | `403 ERR_NOSUCHCHANNEL` |

---

## 💀 Valgrind Final Check

After running ALL tests:

```
==XXXXX== HEAP SUMMARY:
==XXXXX==     definitely lost: 0 bytes in 0 blocks
==XXXXX==     indirectly lost: 0 bytes in 0 blocks
==XXXXX==       possibly lost: 0 bytes in 0 blocks
==XXXXX== FILE DESCRIPTORS: 4 open at exit
             (3 std + 1 listen socket = OK)
```

---

## ✅ Evaluator Will Ask For

- [ ] Client connecting, authenticating, joining a channel
- [ ] Two clients chatting through a channel
- [ ] KICK as operator
- [ ] INVITE into +i channel
- [ ] TOPIC set and view
- [ ] MODE +i, +t, +k, +o, +l
- [ ] Non-op getting `482` on restricted commands
- [ ] Nick change propagating to all channel members
- [ ] Client disconnecting and server not crashing
- [ ] Partial commands don't crash the server
- [ ] **Valgrind: zero leaks**
