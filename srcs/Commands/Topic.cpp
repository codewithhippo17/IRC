#include "Server.hpp"

void Server::_cmdTopic(Client &client, const Command &cmd) {
  if (cmd.getParams().empty()) {
    client.sendMessage(":" SERVER_NAME " " ERR_NEEDMOREPARAMS " "
                       + client.getNickname() + " TOPIC :Not enough parameters\r\n");
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

  if (!cmd.hasTrailing()) {
    if (channel.getTopic().empty())
      client.sendMessage(":" SERVER_NAME " " RPL_NOTOPIC " "
                         + client.getNickname() + " " + channelName
                         + " :No topic is set\r\n");
    else
      client.sendMessage(":" SERVER_NAME " " RPL_TOPIC " "
                         + client.getNickname() + " " + channelName + " :"
                         + channel.getTopic() + "\r\n");
    return;
  }

  if (channel.isTopicRestricted() && !channel.isOperator(&client)) {
    client.sendMessage(":" SERVER_NAME " " ERR_CHANOPRIVSNEEDED " "
                       + client.getNickname() + " " + channelName
                       + " :You're not channel operator\r\n");
    return;
  }

  std::string newTopic = cmd.getTrailing();
  channel.setTopic(newTopic);

  std::string topicMsg =
      ":" + client.getNickname() + " TOPIC " + channelName + " :" + newTopic;
  channel.broadcast(topicMsg, 0);
}
