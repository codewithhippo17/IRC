#include "../../includes/Server.hpp"


void Server::_cmdInvite(Client &client, const Command &cmd)
{
    std::string targetNick = cmd.getParams()[0];
    std::string channelName = cmd.getParams().size() > 1 ? cmd.getParams()[1] : "" ;

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

    Client *target = getClientByNickname(targetNick);
    if (!target)
    {
        sendReply(client, ERR_NOSUCHNICK);
        return;
    }
    if (channel->isMember(target))
    {
        sendReply(client, ERR_USERONCHANNEL);
        return;
    }

    channel->addInvite(target);

    std::string inviteMsg = ":" + client.getNickname() + " INVITE " + targetNick + " " + channelName;
    
    target->sendMessage(inviteMsg);

    sendReply(client, RPL_INVITING);
}