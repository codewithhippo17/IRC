#include "Server.hpp"
#include "Colors.hpp"
#include <iostream>


void Server::runServer()
{
	std::cout << CLR_CYAN << "[Server] Waiting for connections..."
			  << CLR_RESET << std::endl;

	while (_running)
	{
		for (size_t i = 0; i < _pollFds.size(); i++)
		{
			int fd = _pollFds[i].fd;
			if (fd != _listenFd && _clients.find(fd) != _clients.end())
			{
				if (_clients[fd]->hasPendingSend())
					_pollFds[i].events = POLLIN | POLLOUT;
				else
					_pollFds[i].events = POLLIN;
			}
		}

		int ready = poll(&_pollFds[0], _pollFds.size(), -1);

		if (ready < 0)
		{
			if (!_running)
				break;
			continue;
		}

		std::vector<struct pollfd> snapshot = _pollFds;

		for (size_t i = 0; i < snapshot.size(); i++)
		{
			if (snapshot[i].revents == 0)
				continue;

			int fd = snapshot[i].fd;
			short revents = snapshot[i].revents;

			if (revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				if (fd != _listenFd)
					_removeClient(fd);
				continue;
			}

			if (revents & POLLIN)
			{
				if (fd == _listenFd)
				{
					_acceptNewClient();
				}
				else
				{
					_handleClientData(fd);
					if (_clients.find(fd) == _clients.end())
						continue;
				}
			}

			if (revents & POLLOUT)
			{
				if (_clients.find(fd) != _clients.end())
					_handleClientWrite(fd);
			}
		}
	}
}
