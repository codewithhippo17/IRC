#include "Server.hpp"
#include "Replies.hpp"
#include "Colors.hpp"
#include "Utils.hpp"
#include <iostream>
#include <sys/socket.h>


void Server::_initCommandMap()
{
	_cmdMap["PASS"] = &Server::_cmdPass;
	_cmdMap["NICK"] = &Server::_cmdNick;
	_cmdMap["USER"] = &Server::_cmdUser;
	_cmdMap["QUIT"] = &Server::_cmdQuit;
	_cmdMap["PING"] = &Server::_cmdPing;
	_cmdMap["WHOIS"] = &Server::_cmdWhois;

	_cmdMap["JOIN"] = &Server::_cmdJoin;
	_cmdMap["PART"] = &Server::_cmdPart;
	_cmdMap["PRIVMSG"] = &Server::_cmdPrivmsg;
	_cmdMap["KICK"] = &Server::_cmdKick;
	_cmdMap["INVITE"] = &Server::_cmdInvite;
	_cmdMap["TOPIC"] = &Server::_cmdTopic;
	_cmdMap["MODE"] = &Server::_cmdMode;
}


void Server::_handleClientData(int fd)
{
	char buf[512];
	int bytes = recv(fd, buf, sizeof(buf) - 1, 0);

	if (bytes <= 0)
	{
		_removeClient(fd); 
		return;
	}

	Client *client = _clients[fd];
	client->appendToBuffer(std::string(buf, bytes));//NOTE: append received data to client's buffer

	std::vector<std::string> messages = client->extractMessages();//NOTE: extract messages from buffer
	for (size_t i = 0; i < messages.size(); i++)
	{
		_processMessage(*client, messages[i]);
		if (_clients.find(fd) == _clients.end())
			return;
	}
}


void Server::_handleClientWrite(int fd)
{
	std::map<int, Client *>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return;

	Client *client = it->second;
	if (!client->hasPendingSend())//NOTE: if there is no data to send, return
		return;

	const std::string &buf = client->getSendBuffer();//NOTE: get data to send
	int sent = send(fd, buf.c_str(), buf.size(), 0);

	if (sent > 0)
	{
		std::string remaining = buf.substr(sent);
		client->clearSendBuffer();//NOTE: clear sent data from buffer
		if (!remaining.empty())
			client->appendToSendBuffer(remaining);//NOTE: append remaining data to buffer
	}

	if (!client->hasPendingSend())//NOTE: if there is no data to send, set POLLIN
	{
		for (size_t i = 0; i < _pollFds.size(); i++)
		{
			if (_pollFds[i].fd == fd)
			{
				_pollFds[i].events = POLLIN;
				break;
			}
		}
	}
}


void Server::_processMessage(Client &client, const std::string &message)
{
	std::cout << CLR_CYAN << "[<<] fd=" << client.getFd()
			  << ": " << message << CLR_RESET << std::endl;

	Command cmd = Command::parse(message);
	if (cmd.getCommand().empty())
		return;

	_dispatchCommand(client, cmd);
}


void Server::_dispatchCommand(Client &client, const Command &cmd)
{
	std::string cmdName = cmd.getCommand();

	if (!client.isRegistered())
	{
		if (cmdName == "PASS")
		{
			_cmdPass(client, cmd);
		}
		else if (cmdName == "NICK" && client.hasPassAuth())
		{
			_cmdNick(client, cmd);
		}
		else if (cmdName == "USER" && client.hasPassAuth())
		{
			_cmdUser(client, cmd);
		}
		else if (cmdName == "QUIT")
		{
			_cmdQuit(client, cmd);
		}
		else if (cmdName == "CAP")
		{
			if (!cmd.getParams().empty() && cmd.getParams()[0] == "LS")
				client.sendMessage(":" SERVER_NAME " CAP * LS :\r\n");
		}
		else if (cmdName == "NICK" && !client.hasPassAuth())
		{
			client.sendMessage(":" SERVER_NAME " " ERR_NOTREGISTERED
							   " * :You must send PASS first\r\n");
		}
		else
		{
			client.sendMessage(":" SERVER_NAME " " ERR_NOTREGISTERED
							   " * :You have not registered\r\n");
		}
		return;
	}

	std::map<std::string, CmdHandler>::iterator it = _cmdMap.find(cmdName);
	if (it != _cmdMap.end())
	{
		(this->*(it->second))(client, cmd);
	}
	else
	{
		client.sendMessage(":" SERVER_NAME " " ERR_UNKNOWNCOMMAND " " + client.getNickname() + " " + cmdName + " :Unknown command\r\n");
	}
}

/* ── Send IRC numeric reply ──────────────────────────────────────────── */
void Server::_sendReply(Client &client, const std::string &code) const
{
	client.sendMessage(":" SERVER_NAME " " + code + " " + client.getNickname() + "\r\n");
}
