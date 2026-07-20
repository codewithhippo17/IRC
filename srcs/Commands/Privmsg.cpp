#include "../../includes/Server.hpp"

void Server::_cmdPrivmsg(Client &client, const Command &cmd)
{
	std::string target = cmd.getParams()[0];
	std::string message = cmd.getParams().size() > 1 ? cmd.getParams()[1] : "";

	if (target[0] == '#')
	{
		Channel *channel = _channelManager.getChannel(target);
		if (!channel)
		{
			sendReply(client, ERR_NOSUCHCHANNEL);
			return;
		}

		if (!channel->isMember(&client))
		{
			sendReply(client, ERR_NOTONCHANNEL);
			return;
		}
		std::string fullMsg = ":" + client.getNickname() + " PRIVMSG " + target + " :" + message;
		channel->broadcast(fullMsg, &client);
	}
	else
	{
		Client *targetClient = getClientByNickname(target);
		if (!targetClient)
		{
			sendReply(client, ERR_NOSUCHNICK);
			return;
		}

		std::string fullMsg = ":" + client.getNickname() + " PRIVMSG " + target + " :" + message;
		targetClient->sendMessage(fullMsg);
	}
}