*This project has been created as part of the 42 curriculum by mlaidi.*

## Description
This project consists of writing our own IRC server in C++98. The IRC server must allow clients to connect, authenticate, set a nickname, join channels, and communicate using private messages and channel broadcasts. It supports basic channel operator commands like KICK, INVITE, TOPIC, and MODE.

## Instructions

### Compilation
To compile the project, run:
```bash
make
```

### Execution
Run the server by providing a port and a password:
```bash
./ircserv <port> <password>
```

### Usage
Once the server is running, you can connect using a standard IRC client (e.g., irssi, WeeChat) or via netcat:
```bash
nc 127.0.0.1 <port>
```
Example sequence using netcat:
```text
PASS <password>
NICK <nickname>
USER <username> 0 * :<realname>
```

## Resources
- [RFC 1459: Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
- [RFC 2812: Internet Relay Chat: Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
- **AI Usage**: AI was used primarily to assist in reviewing and debugging the parameter validations across command handling classes, ensuring robust crash prevention. AI also generated this README based on the subject's requirements.
