#include "Server.hpp"
#include "Colors.hpp"

bool Server::_running = true;

Server::Server(int port, const std::string &password)
    : _port(port), _password(password), _listenFd(-1) {
  _setupServer();
  _initCommandMap();
  std::cout << CLR_GREEN << "[Server] " << CLR_BOLD << "Started on port "
            << _port << CLR_RESET << std::endl;
}

Server::~Server() {

  for (std::map<int, Client *>::iterator it = _clients.begin();
       it != _clients.end(); ++it) {
    close(it->first);
    delete it->second;
  }
  _clients.clear();
  _channels.clear();

  if (_listenFd >= 0)
    close(_listenFd);

  std::cout << CLR_YELLOW << "[Server] Shut down cleanly" << CLR_RESET
            << std::endl;
}

void Server::_setupServer() {
  _listenFd = socket(AF_INET, SOCK_STREAM, 0);
  if (_listenFd < 0)
    throw std::runtime_error("socket() failed");

  int opt = 1;
  if (setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    close(_listenFd);
    throw std::runtime_error("setsockopt() failed");
  }

  struct sockaddr_in addr;
  std::memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(_port);

  if (bind(_listenFd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    close(_listenFd);
    throw std::runtime_error("bind() failed");
  }

  if (listen(_listenFd, SOMAXCONN) < 0) {
    close(_listenFd);
    throw std::runtime_error("listen() failed");
  }

  if (fcntl(_listenFd, F_SETFL, O_NONBLOCK) < 0) {
    close(_listenFd);
    throw std::runtime_error("fcntl() failed");
  }

  struct pollfd pfd;
  pfd.fd = _listenFd;
  pfd.events = POLLIN;
  pfd.revents = 0;
  _pollFds.push_back(pfd);
}

bool Server::isRunning() { return _running; }

void Server::signalHandler(int signum) {
  (void)signum;
  std::cout << std::endl
            << CLR_YELLOW << "[Server] Signal received, shutting down..."
            << CLR_RESET << std::endl;
  _running = false;
}
