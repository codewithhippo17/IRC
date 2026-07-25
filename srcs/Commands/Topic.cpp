#include "../../includes/Server.hpp"

void Server::_cmdTopic(Client &client, const Command &cmd) {
  std::string channelName = cmd.getParams()[0];

  std::map<std::string, Channel>::iterator it = _channels.find(channelName);

  if (it == _channels.end()) {
    _sendReply(client, ERR_NOSUCHCHANNEL);
    return;
  }
  Channel &channel = it->second;

  if (!channel.isMember(&client)) {
    _sendReply(client, ERR_NOTONCHANNEL);
    return;
  }

  if (!cmd.hasTrailing()) {
    if (channel.getTopic().empty())
      _sendReply(client, RPL_NOTOPIC);
    else
      _sendReply(client, RPL_TOPIC);
    return;
  }

  if (channel.isTopicRestricted() && !channel.isOperator(&client)) {
    _sendReply(client, ERR_CHANOPRIVSNEEDED);
    return;
  }

  std::string newTopic = cmd.getTrailing();
  channel.setTopic(newTopic);

  std::string topicMsg =
      ":" + client.getNickname() + " TOPIC " + channelName + " :" + newTopic;
  channel.broadcast(topicMsg, 0);
}
