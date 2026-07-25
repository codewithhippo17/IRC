#include "Server.hpp"

void Server::_cmdJoin(Client &client, const Command &cmd) {
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
    _sendReply(client, ERR_INVITEONLYCHAN);
    return;
  }

  if (channel.hasKey()) {
    std::string providekey =
        cmd.getParams().size() > 1 ? cmd.getParams()[1] : "";
    if (providekey != channel.getKey()) {
      _sendReply(client, ERR_BADCHANNELKEY);
      return;
    }
  }

  if (channel.hasLimit() &&
      channel.getMembers().size() >= static_cast<size_t>(channel.getLimit())) {
    _sendReply(client, ERR_CHANNELISFULL);
    return;
  }

  channel.addMember(&client);
  client.addChannel(channelName);

  std::string joinMsg = ":" + client.getNickname() + " JOIN " + channelName;
  channel.broadcast(joinMsg, 0);
}
