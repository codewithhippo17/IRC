#include "Server.hpp"

void Server::_cmdMode(Client &client, const Command &cmd) {
  if (cmd.getParams().empty()) {
    client.sendMessage(":" SERVER_NAME " " ERR_NEEDMOREPARAMS " "
                       + client.getNickname() + " MODE :Not enough parameters\r\n");
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

  // Mode query (no mode flags) — any member can see channel modes
  if (cmd.getParams().size() < 2) {
    std::string modeStr = "+";
    if (channel.isInviteOnly()) modeStr += "i";
    if (channel.isTopicRestricted()) modeStr += "t";
    if (channel.hasKey()) modeStr += "k";
    if (channel.hasLimit()) modeStr += "l";
    std::string modeParams;
    if (channel.hasKey()) modeParams += " " + channel.getKey();
    if (channel.hasLimit()) {
      std::ostringstream oss;
      oss << channel.getLimit();
      modeParams += " " + oss.str();
    }
    client.sendMessage(":" SERVER_NAME " " RPL_CHANNELMODEIS " " +
                       client.getNickname() + " " + channelName + " " +
                       modeStr + modeParams + "\r\n");
    return;
  }

  // Irssi automatically queries the ban list ('b'). We don't support bans.
  // Send RPL_ENDOFBANLIST (368) so Irssi doesn't error out.
  if (cmd.getParams().size() >= 2 && cmd.getParams()[1] == "b") {
    client.sendMessage(":" SERVER_NAME " 368 " + client.getNickname() + " " +
                       channelName + " :End of channel ban list\r\n");
    return;
  }

  if (!channel.isOperator(&client)) {
    client.sendMessage(":" SERVER_NAME " " ERR_CHANOPRIVSNEEDED " "
                       + client.getNickname() + " " + channelName
                       + " :You're not channel operator\r\n");
    return;
  }

  std::string modes = cmd.getParams()[1];
  size_t argIndex = 2;
  bool adding = true;

  for (size_t i = 0; i < modes.size(); i++) {
    char c = modes[i];
    if (c == '+') { adding = true; continue; }
    if (c == '-') { adding = false; continue; }

    if (c == 'i') { channel.setInviteOnly(adding); continue; }
    if (c == 't') { channel.setTopicRestricted(adding); continue; }

    if (c == 'k') {
      if (adding) {
        if (argIndex >= cmd.getParams().size()) {
          client.sendMessage(":" SERVER_NAME " " ERR_NEEDMOREPARAMS " "
                             + client.getNickname() + " MODE :Not enough parameters\r\n");
          continue;
        }
        channel.setKey(cmd.getParams()[argIndex++]);
      } else channel.removeKey();
      continue;
    }

    if (c == 'l') {
      if (adding) {
        if (argIndex >= cmd.getParams().size()) {
          client.sendMessage(":" SERVER_NAME " " ERR_NEEDMOREPARAMS " "
                             + client.getNickname() + " MODE :Not enough parameters\r\n");
          continue;
        }
        std::istringstream ss(cmd.getParams()[argIndex++]);
        int limit; ss >> limit;
        channel.setLimit(limit);
      } else channel.removeLimit();
      continue;
    }

    if (c == 'o') {
      if (argIndex >= cmd.getParams().size()) {
        client.sendMessage(":" SERVER_NAME " " ERR_NEEDMOREPARAMS " "
                           + client.getNickname() + " MODE :Not enough parameters\r\n");
        continue;
      }
      std::string targetNick = cmd.getParams()[argIndex++];
      Client *target = _findClientByNick(targetNick);
      if (!target || !channel.isMember(target)) {
        client.sendMessage(":" SERVER_NAME " " ERR_USERNOTINCHANNEL " "
                           + client.getNickname() + " " + targetNick + " "
                           + channelName + " :They aren't on that channel\r\n");
        continue;
      }
      if (adding) channel.addOperator(target);
      else channel.removeOperator(target);
    }
  }
}
