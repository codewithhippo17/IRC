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
			_sendReply(client, ERR_NOSUCHCHANNEL);
			return ;
		}
		Channel &channel = it->second;

		if (!channel.isMember(&client))
		{
			_sendReply(client, ERR_NOTONCHANNEL);
			return;
		}
		std::string fullMsg = ":" + client.getNickname() + " PRIVMSG " + target + " :" + message;
		channel.broadcast(fullMsg, &client);
	}
	else
	{
		Client *targetClient = _findClientByNick(target);
		if (!targetClient)
		{
			_sendReply(client, ERR_NOSUCHNICK);
			return;
		}

		std::string fullMsg = ":" + client.getNickname() + " PRIVMSG " + target + " :" + message;
		targetClient->sendMessage(fullMsg);
	}
}