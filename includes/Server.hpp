#ifndef SERVER_HPP
#define SERVER_HPP

#include "Channel.hpp"
#include "Client.hpp"
#include "Command.hpp"
#include "Replies.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <poll.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

class Server {
public:
  Server(int port, const std::string &password);
  ~Server();

  void runServer();

  /* Signal handling */
  static bool isRunning();
  static void signalHandler(int signum);

private:
  /* ── Server setup ───────────────────────────────────────────────────── */
  void _setupServer();
  void _initCommandMap();

  /* ── Client lifecycle ───────────────────────────────────────────────── */
  void _acceptNewClient();
  void _removeClient(int fd);
  void _handleClientData(int fd);
  void _handleClientWrite(int fd);
  Client *_findClientByNick(const std::string &nickname);

  /* ── Message processing ─────────────────────────────────────────────── */
  void _processMessage(Client &client, const std::string &message);
  void _dispatchCommand(Client &client, const Command &cmd);

  /* ── Channel helpers ────────────────────────────────────────────────── */
  void _removeClientFromAllChannels(int fd);

  /* ══ Command handlers — Person A ════════════════════════════════════ */
  void _cmdPass(Client &client, const Command &cmd);
  void _cmdNick(Client &client, const Command &cmd);
  void _cmdUser(Client &client, const Command &cmd);
  void _cmdQuit(Client &client, const Command &cmd);
  void _cmdPing(Client &client, const Command &cmd);
  void _cmdWhois(Client &client, const Command &cmd);

  /* ══ Command handlers — Person B (stubs until implemented) ══════════ */
  void _cmdJoin(Client &client, const Command &cmd);
  void _cmdPart(Client &client, const Command &cmd);
  void _cmdPrivmsg(Client &client, const Command &cmd);
  void _cmdKick(Client &client, const Command &cmd);
  void _cmdInvite(Client &client, const Command &cmd);
  void _cmdTopic(Client &client, const Command &cmd);
  void _cmdMode(Client &client, const Command &cmd);

  /* ── Server state ───────────────────────────────────────────────────── */
  int _port;
  std::string _password;
  int _listenFd;
  static bool _running;

  std::vector<struct pollfd> _pollFds;
  std::map<int, Client *> _clients;         /* fd -> Client*  */
  std::map<std::string, Channel> _channels; /* #name -> Channel */

  /* ── Command dispatch table ─────────────────────────────────────────── */
  typedef void (Server::*CmdHandler)(Client &, const Command &);
  std::map<std::string, CmdHandler> _cmdMap;

  /* ── Orthodox Canonical Form — prevent copy ─────────────────────────── */
  Server();
  Server(const Server &other);
  Server &operator=(const Server &other);
};

#endif
