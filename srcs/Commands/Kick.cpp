#include "../../includes/Command.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/Replies.hpp"
#include "../../includes/Client.hpp"

void Server::_cmdKick(Client &client, const Command &cmd)
{
	std::string channelName = cmd.getParams()[0];

    Channel *channel = _channelManager.getChannel(channelName);
    if (!channel)
    {
        sendReply(client, ERR_NOSUCHCHANNEL);
        return;
    }

    if (!channel->isOperator(&client))
    {
        sendReply(client, ERR_CHANOPRIVSNEEDED);
        return;
    }

    std::string targetNick = cmd.getParams().size() > 1 ? cmd.getParams()[1] : "";
    Client *target = getClientByNickname(targetNick);
    if (!target)
    {
        sendReply(client, ERR_NOSUCHNICK);
        return;
    }

    if (!channel->isMember(target))
    {
        sendReply(client, ERR_USERNOTINCHANNEL);
        return;
    }

    std::string reason = cmd.getParams().size() > 2 ? cmd.getParams()[2] : client.getNickname();
    std::string kickMsg = ":" + client.getNickname() + " KICK " + channelName + " "
        + targetNick + " :" + reason;

    channel->broadcast(kickMsg, 0);

    channel->removeClient(target);

    if (channel->getMembers().empty())
    {
        _channelManager.removeChannel(channelName);
    }
}
