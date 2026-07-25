#include "../../includes/Server.hpp"

void Server::_cmdKick(Client &client, const Command &cmd)
{
    // zedna hadchi: verify command parameters bach n7miw mn l'crash
    if (cmd.getParams().size() < 2)
    {
        _sendReply(client, ERR_NEEDMOREPARAMS);
        return;
    }

	std::string channelName = cmd.getParams()[0];

	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	if (it == _channels.end())
	{
		_sendReply(client, ERR_NOSUCHCHANNEL);
		return;
	}
	Channel &channel = it->second;

	if (!channel.isOperator(&client))
	{
		_sendReply(client, ERR_CHANOPRIVSNEEDED);
		return;
	}

	std::string targetNick = cmd.getParams().size() > 1 ? cmd.getParams()[1] : "";
	Client *target = _findClientByNick(targetNick);
	if (!target)
	{
		_sendReply(client, ERR_NOSUCHNICK);
		return;
	}

	if (!channel.isMember(target))
	{
		_sendReply(client, ERR_USERNOTINCHANNEL);
		return;
	}

	std::string reason = cmd.hasTrailing() ? cmd.getTrailing() : client.getNickname();
	std::string kickMsg = ":" + client.getNickname() + " KICK " + channelName + " "
		+ targetNick + " :" + reason;

	channel.broadcast(kickMsg, 0);

	channel.removeMember(target);

	if (channel.getMembers().empty())
	{
		_channels.erase(channelName);
	}
}