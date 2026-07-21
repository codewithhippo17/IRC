
#include "Server.hpp"
#include "Colors.hpp"
#include "Utils.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>


void Server::_acceptNewClient()
{
	struct sockaddr_in clientAddr;
	socklen_t addrLen = sizeof(clientAddr);

	int clientFd = accept(_listenFd, (struct sockaddr *)&clientAddr, &addrLen);
	if (clientFd < 0)
		return; 

	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0)
	{
		close(clientFd);
		return;
	}

	std::string hostname = inet_ntoa(clientAddr.sin_addr);

	struct pollfd pfd;
	pfd.fd = clientFd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pollFds.push_back(pfd);

	_clients[clientFd] = new Client(clientFd, hostname);

	std::cout << CLR_GREEN << "[+] New connection: fd=" << clientFd
			  << " from " << hostname << CLR_RESET << std::endl;
}


void Server::_removeClient(int fd)
{
	std::map<int, Client *>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return;

	std::cout << CLR_RED << "[-] Disconnected: "
			  << it->second->getNickname()
			  << " (fd=" << fd << ")" << CLR_RESET << std::endl;

	_removeClientFromAllChannels(fd);

	delete it->second;
	_clients.erase(it);

	close(fd);

	for (size_t i = 0; i < _pollFds.size(); i++)
	{
		if (_pollFds[i].fd == fd)
		{
			_pollFds.erase(_pollFds.begin() + i);
			break;
		}
	}
}


Client *Server::_findClientByNick(const std::string &nickname)
{
	std::string upper = toUpper(nickname);
	for (std::map<int, Client *>::iterator it = _clients.begin();
		 it != _clients.end(); ++it)
	{
		if (toUpper(it->second->getNickname()) == upper)
			return it->second;
	}
	return NULL;
}
