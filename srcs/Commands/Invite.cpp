#include "../../includes/Server.hpp"

void Server::_cmdInvite(Client &client, const Command &cmd)
{
    // zedna hadchi: check if we have both user and channel
    if (cmd.getParams().size() < 2)
    {
        _sendReply(client, ERR_NEEDMOREPARAMS);
        return;
    }

    std::string targetNick = cmd.getParams()[0];
    std::string channelName = cmd.getParams().size() > 1 ? cmd.getParams()[1] : "" ;

    std::map<std::string, Channel>::iterator it = _channels.find(channelName);

    if(it == _channels.end())
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

    Client *target = _findClientByNick(targetNick);
    if (!target)
    {
        _sendReply(client, ERR_NOSUCHNICK);
        return;
    }
    if (channel.isMember(target))
    {
        _sendReply(client, ERR_USERONCHANNEL);
        return;
    }

    channel.addInvited(target);

    std::string inviteMsg = ":" + client.getNickname() + " INVITE " + targetNick + " " + channelName;
    
    target->sendMessage(inviteMsg);

    _sendReply(client, RPL_INVITING);
}