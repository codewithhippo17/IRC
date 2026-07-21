#include "../../includes/Server.hpp"

void Server::_cmdPart(Client &client, const Command &cmd)
{
	std::string channelName = cmd.getParams()[0];

    std::map<std::string, Channel>::iterator it = _channels.find(channelName);
    
    if(it == _channels.end())
    {
        sendReply(client, ERR_NOSUCHCHANNEL);
        return ;
    }
    Channel &channel = it->second;

    if(!channel.isMember(&client))
    {
        sendReply(client, ERR_NOTONCHANNEL);
        return;
    }
    
    std::string partMsg = ":" + client.getNickname() + " PART " + channelName;
    channel.broadcast(partMsg, 0);

    channel.removeClient(&client);

    if (channel.getMembers().empty())
    {
        _channels.erase(channelName);
    }
}