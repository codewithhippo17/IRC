#include "Server.hpp"

void Server::_cmdPart(Client &client, const Command &cmd) {
  if (cmd.getParams().empty()) {
    client.sendMessage(":" SERVER_NAME " " ERR_NEEDMOREPARAMS " "
                       + client.getNickname() + " PART :Not enough parameters\r\n");
    return;
  }

  std::string channelName = cmd.getParams()[0];

  std::map<std::string, Channel>::iterator it = _channels.find(channelName);
  if (it == _channels.end()) {
    client.sendMessage(":" SERVER_NAME " " ERR_NOSUCHCHANNEL " "
                       + client.getNickname() + " " + channelName
                       + " :No such channel\r\n");
    return;
  }
  Channel &channel = it->second;

  if (!channel.isMember(&client)) {
    client.sendMessage(":" SERVER_NAME " " ERR_NOTONCHANNEL " "
                       + client.getNickname() + " " + channelName
                       + " :You're not on that channel\r\n");
    return;
  }

  std::string partMsg = ":" + client.getNickname() + " PART " + channelName;
  channel.broadcast(partMsg, 0);

  channel.removeMember(&client);
  client.removeChannel(channelName);

  if (channel.getMembers().empty())
    _channels.erase(channelName);
}
