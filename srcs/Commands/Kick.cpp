#include "../../includes/Server.hpp"

void Server::_cmdKick(Client &client, const Command &cmd)
{
	std::string channelName = cmd.getParams()[0];

	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	if (it == _channels.end())
	{
		client.sendMessage(":" SERVER_NAME " " ERR_NOSUCHCHANNEL " " + client.getNickname() + " " + channelName + " :No such Channel\r\n");
		return;
	}
	Channel &channel = it->second;

	if (!channel.isOperator(&client))
	{
		client.sendMessage(":" SERVER_NAME " " ERR_CHANOPRIVSNEEDED " " + client.getNickname() + " " + channelName + " :You're not channel operator\r\n");
		return;
	}

	std::string targetNick = cmd.getParams().size() > 1 ? cmd.getParams()[1] : "";
	Client *target = _findClientByNick(targetNick);
	if (!target)
	{
		client.sendMessage(":" SERVER_NAME " " ERR_NOSUCHNICK " " + client.getNickname() + " " + targetNick + " :No such nick/channel\r\n");
		return;
	}

	if (!channel.isMember(target))
	{
		client.sendMessage(":" SERVER_NAME " " ERR_USERNOTINCHANNEL " " + client.getNickname() + " " + targetNick + " " + channelName + " :They aren't on that channel\r\n");
		return;
	}

	std::string reason = cmd.hasTrailing() ? cmd.getTrailing() : client.getNickname();
	std::string kickMsg = ":" + client.getNickname() + " KICK " + channelName + " "
		+ targetNick + " :" + reason;

	channel.broadcast(kickMsg, 0);

	channel.removeClient(target);

	if (channel.getMembers().empty())
	{
		_channels.erase(channelName);
	}
}