#include "Server.hpp"

void Server::_cmdJoin(Client &client, const Command &cmd) {
  if (cmd.getParams().empty()) {
    client.sendMessage(":" SERVER_NAME " " ERR_NEEDMOREPARAMS " "
                       + client.getNickname() + " JOIN :Not enough parameters\r\n");
    return;
  }

  std::string channelName = cmd.getParams()[0];

  std::map<std::string, Channel>::iterator it = _channels.find(channelName);
  bool isNewChannel = (it == _channels.end());

  if (isNewChannel)
    it = _channels.insert(std::make_pair(channelName, Channel(channelName)))
             .first;

  Channel &channel = it->second;

  if (isNewChannel)
    channel.addOperator(&client);

  if (channel.isMember(&client))
    return;

  if (channel.isInviteOnly() && !channel.isInvited(&client)) {
    client.sendMessage(":" SERVER_NAME " " ERR_INVITEONLYCHAN " "
                       + client.getNickname() + " " + channelName
                       + " :Cannot join channel (+i)\r\n");
    return;
  }

  if (channel.hasKey()) {
    std::string providekey =
        cmd.getParams().size() > 1 ? cmd.getParams()[1] : "";
    if (providekey != channel.getKey()) {
      client.sendMessage(":" SERVER_NAME " " ERR_BADCHANNELKEY " "
                         + client.getNickname() + " " + channelName
                         + " :Cannot join channel (+k)\r\n");
      return;
    }
  }

  if (channel.hasLimit() &&
      channel.getMembers().size() >= static_cast<size_t>(channel.getLimit())) {
    client.sendMessage(":" SERVER_NAME " " ERR_CHANNELISFULL " "
                       + client.getNickname() + " " + channelName
                       + " :Cannot join channel (+l)\r\n");
    return;
  }

  channel.addMember(&client);
  client.addChannel(channelName);

  std::string joinMsg = ":" + client.getNickname() + " JOIN " + channelName;
  channel.broadcast(joinMsg, 0);
}
