#include "../../includes/Server.hpp"

void Server::_cmdInvite(Client &client, const Command &cmd)
{
    std::string targetNick = cmd.getParams()[0];
    std::string channelName = cmd.getParams().size() > 1 ? cmd.getParams()[1] : "" ;

    std::map<std::string, Channel>::iterator it = _channels.find(channelName);

    if(it == _channels.end())
    {
        sendReply(client, ERR_NOSUCHCHANNEL);
        return;
    }
    Channel &channel = it->second;
    
    if (!channel.isOperator(&client))
    {
        sendReply(client, ERR_CHANOPRIVSNEEDED);
        return;
    }

    Client *target = _findClientByNick(targetNick);
    if (!target)
    {
        sendReply(client, ERR_NOSUCHNICK);
        return;
    }
    if (channel.isMember(target))
    {
        sendReply(client, ERR_USERONCHANNEL);
        return;
    }

    channel.addInvite(target);

    std::string inviteMsg = ":" + client.getNickname() + " INVITE " + targetNick + " " + channelName;
    
    target->sendMessage(inviteMsg);

    sendReply(client, RPL_INVITING);
}