#include "../../includes/Server.hpp"


void Server::_cmdJoin(Client &client, const Command &cmd)
{
    std::string channelName = cmd.getParams()[0];
    
    Channel *channel = _channelManager.getChannel(channelName);
	if (!channel)
	{
		channel = _channelManager.createChannel(channelName);
		channel->addOperator(&client);
    }

    if(channel->isMember(&client))
        return;

    if(channel->isInviteOnly() && !channel->isInvited(&client))
    {
        sendReply(client, ERR_INVITEONLYCHAN);
		return;
    }

    if(channel->hasKey())
    {
        std::string providekey = cmd.getParams().size() > 1 ? cmd.getParams()[1] : "" ;
        if(providekey != channel->getKey())
        {
            sendReply(client, ERR_BADCHANNELKEY);
            return;
        }
    }

    if(channel->hasUserLimit() && channel-> getMembers().size() >= channel->getUserLimit())
    {
        sendReply(client, ERR_CHANNELISFULL);
	    return;
    }

    channel->addClient(&client);

    std::string joinMsg = ":" + client.getNickname() + " JOIN " + channelName;
    channel->broadcast(joinMsg, 0);

}