#include "../../includes/Server.hpp"

void Server::_cmdPart(Client &client, const Command &cmd)
{
	std::string channelName = cmd.getParams()[0];

    Channel *channel = _channelManager.getChannel(channelName);
    if(!channel)
    {
        sendReply(client, ERR_NOSUCHCHANNEL);
        return;
    }

    if(!channel->isMember(&client))
    {
        sendReply(client, ERR_NOTONCHANNEL);
        return;
    }
    
    std::string partMsg = ":" + client.getNickname() + " PART " + channelName;
    channel->broadcast(partMsg, 0);

    channel->removeClient(&client);

    if(channel->getMembers().empty())
    {
        _channelManager.removeChannel(channelName);
    }
}