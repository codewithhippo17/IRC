#include "../../includes/Server.hpp"

void Server::_cmdInvite(Client &client, const Command &cmd)
{
    std::string targetNick = cmd.getParams()[0];
    std::string channelName = cmd.getParams().size() > 1 ? cmd.getParams()[1] : "" ;

    std::map<std::string, Channel>::iterator it = _channels.find(channelName);

    if(it == _channels.end())
    {
        client.sendMessage(":" SERVER_NAME " " ERR_NOSUCHCHANNEL " " + client.getNickname() + " " + channelName + " :No such Channel\r\n");
        return;
    }
    Channel &channel = it->second;
    
    if (!channel.isOperator(&client))
    {
        client.sendMessage(":" SERVER_NAME " " ERR_CHANOPRIVSNEEDED " " + client.getNickname() + " " + channelName + " :You're not channel operator\r\n");
        return;
    }

    Client *target = _findClientByNick(targetNick);
    if (!target)
    {
        client.sendMessage(":" SERVER_NAME " " ERR_NOSUCHNICK " " + client.getNickname() + " " + targetNick + " :No such nick/channel\r\n");
        return;
    }

    if (channel.isMember(target))
    {
        client.sendMessage(":" SERVER_NAME " " ERR_USERONCHANNEL " " + client.getNickname() + " " + targetNick + " " + channelName + " :is already on channel\r\n");
        return;
    }

    channel.addInvite(target);

    std::string inviteMsg = ":" + client.getNickname() + " INVITE " + targetNick + " " + channelName + "\r\n";
    
    target->sendMessage(inviteMsg);

    client.sendMessage(":" SERVER_NAME " " RPL_INVITING " " + client.getNickname() + " " + targetNick + " " + channelName + "\r\n");
}