#include "Server.hpp"

static bool isValidNick(const std::string &nick) {
  if (nick.empty() || nick.length() > 9)
    return false;

  char c = nick[0];
  if (!std::isalpha(static_cast<unsigned char>(c)) && c != '[' && c != ']' &&
      c != '\\' && c != '`' && c != '_' && c != '^' && c != '{' && c != '|' &&
      c != '}')
    return false;

  for (size_t i = 1; i < nick.length(); i++) {
    c = nick[i];
    if (!std::isalnum(static_cast<unsigned char>(c)) && c != '[' && c != ']' &&
        c != '\\' && c != '`' && c != '_' && c != '^' && c != '{' && c != '|' &&
        c != '}' && c != '-')
      return false;
  }
  return true;
}

void Server::_cmdNick(Client &client, const Command &cmd) {
  if (cmd.paramCount() == 0 && !cmd.hasTrailing()) {
    std::string name =
        client.isRegistered() ? client.getNickname() : std::string("*");
    client.sendMessage(":" SERVER_NAME " " ERR_NONICKNAMEGIVEN " " + name +
                       " :No nickname given\r\n");
    return;
  }

  std::string newNick =
      cmd.paramCount() > 0 ? cmd.getParam(0) : cmd.getTrailing();

  if (!isValidNick(newNick)) {
    std::string name =
        client.isRegistered() ? client.getNickname() : std::string("*");
    client.sendMessage(":" SERVER_NAME " " ERR_ERRONEUSNICKNAME " " + name +
                       " " + newNick + " :Erroneous nickname\r\n");
    return;
  }

  Client *existing = _findClientByNick(newNick);
  if (existing && existing->getFd() != client.getFd()) {
    std::string name =
        client.isRegistered() ? client.getNickname() : std::string("*");
    client.sendMessage(":" SERVER_NAME " " ERR_NICKNAMEINUSE " " + name + " " +
                       newNick + " :Nickname is already in use\r\n");
    return;
  }

  if (client.isRegistered()) {
    std::string oldPrefix = client.getPrefix();
    std::string changeMsg = ":" + oldPrefix + " NICK " + newNick + "\r\n";

    client.sendMessage(changeMsg);

    const std::vector<std::string> &channels = client.getChannels();
    for (size_t i = 0; i < channels.size(); i++) {
      std::map<std::string, Channel>::iterator it = _channels.find(channels[i]);
      if (it != _channels.end())
        it->second.broadcast(changeMsg, &client);
    }
  }

  client.setNickname(newNick);
  client.setNickAuth(true);

  if (client.hasUserAuth() && !client.isRegistered()) {
    client.setRegistered(true);

    std::string nick = client.getNickname();
    std::string prefix = client.getPrefix();

    client.sendMessage(":" SERVER_NAME " " RPL_WELCOME " " + nick +
                       " :Welcome to the IRC network, " + prefix + "\r\n");

    client.sendMessage(":" SERVER_NAME " " RPL_YOURHOST " " + nick +
                       " :Your host is " SERVER_NAME
                       ", running version 1.0\r\n");

    client.sendMessage(":" SERVER_NAME " " RPL_CREATED " " + nick +
                       " :This server was created today\r\n");

    client.sendMessage(":" SERVER_NAME " " RPL_MYINFO " " + nick +
                       " " SERVER_NAME " 1.0 o itkol\r\n");
  }
}
