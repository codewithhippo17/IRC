#include "../../includes/Server.hpp"

void Server::_cmdPart(Client &client, const Command &cmd)
{
    // zedna hadchi: check if parameters exist bach mayw9e3ch segfault
    if (cmd.getParams().empty())
    {
        _sendReply(client, ERR_NEEDMOREPARAMS);
        return;
    }

	std::string channelName = cmd.getParams()[0];

    std::map<std::string, Channel>::iterator it = _channels.find(channelName);
    
    if(it == _channels.end())
    {
        _sendReply(client, ERR_NOSUCHCHANNEL);
        return ;
    }
    Channel &channel = it->second;

    if(!channel.isMember(&client))
    {
        _sendReply(client, ERR_NOTONCHANNEL);
        return;
    }
    
    std::string partMsg = ":" + client.getNickname() + " PART " + channelName;
    channel.broadcast(partMsg, 0);

    channel.removeMember(&client);

    if (channel.getMembers().empty())
    {
        _channels.erase(channelName);
    }
}