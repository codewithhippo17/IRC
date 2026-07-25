#include "../../includes/Server.hpp"

void Server::_cmdPrivmsg(Client &client, const Command &cmd)
{
	std::string target = cmd.getParams()[0];
	std::string message = cmd.hasTrailing() ? cmd.getTrailing() : "";

	if (target[0] == '#')
	{
		std::map<std::string, Channel>::iterator it = _channels.find(target);
		
		if(it == _channels.end())
		{
			client.sendMessage(":" SERVER_NAME " " ERR_NOSUCHCHANNEL " " + client.getNickname() + " " + target + " :No such Channel\r\n");
			return ;
		}
		Channel &channel = it->second;

		if (!channel.isMember(&client))
		{
			client.sendMessage(":" SERVER_NAME " " ERR_NOTONCHANNEL " " + client.getNickname() + " " + target + " :You're not on that channel\r\n");
			return;
		}
		std::string fullMsg = ":" + client.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n";
		channel.broadcast(fullMsg, &client);
	}
	else
	{
		Client *targetClient = _findClientByNick(target);
		if (!targetClient)
		{
			client.sendMessage(":" SERVER_NAME " " ERR_NOSUCHNICK " " + client.getNickname() + " " + target + " :No such nick/channel\r\n");
			return;
		}

		std::string fullMsg = ":" + client.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n";
		targetClient->sendMessage(fullMsg);
	}
}