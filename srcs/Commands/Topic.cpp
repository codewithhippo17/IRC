#include "../../includes/Server.hpp"

void Server::_cmdTopic(Client &client, const Command &cmd)
{
	std::string channelName = cmd.getParams()[0];
 
	Channel *channel = _channelManager.getChannel(channelName);
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

    if (cmd.getParams().size() < 2)
    {
        if (channel->getTopic().empty())
            sendReply(client, RPL_NOTOPIC);
        else
            sendReply(client, RPL_TOPIC);
        return ;
    }

    if (channel->isTopicRestricted() && !channel->isOperator(&client))
    {
        sendReply(client, ERR_CHANOPRIVSNEEDED);
        return;
    }

    std::string newTopic = cmd.getParams()[1];
    channel->setTopic(newTopic);

    std::string topicMsg = ":" + client.getNickname() + " TOPIC " + channelName + " :" + newTopic;
    channel->broadcast(topicMsg, 0);
}