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

  if (channel.hasLimit() && !channel.isInvited(&client) &&
      channel.getMembers().size() >= static_cast<size_t>(channel.getLimit())) {
    client.sendMessage(":" SERVER_NAME " " ERR_CHANNELISFULL " "
                       + client.getNickname() + " " + channelName
                       + " :Cannot join channel (+l)\r\n");
    return;
  }

  channel.addMember(&client);
  client.addChannel(channelName);

  if (channel.isInvited(&client))
    channel.removeInvited(&client);

  std::string joinMsg = ":" + client.getPrefix() + " JOIN :" + channelName;
  channel.broadcast(joinMsg, &client);

  // Send NAMES list to the joining client
  std::string namesList;
  const std::set<Client *> &members = channel.getMembers();
  for (std::set<Client *>::const_iterator mit = members.begin();
       mit != members.end(); ++mit) {
    if (mit != members.begin())
      namesList += " ";
    if (channel.isOperator(*mit))
      namesList += "@";
    namesList += (*mit)->getNickname();
  }
  client.sendMessage(":" SERVER_NAME " " RPL_NAMREPLY " " +
                     client.getNickname() + " = " + channelName + " :" +
                     namesList + "\r\n");
  client.sendMessage(":" SERVER_NAME " " RPL_ENDOFNAMES " " +
                     client.getNickname() + " " + channelName +
                     " :End of /NAMES list\r\n");
}
