#include "Server.hpp"

void Server::_cmdUser(Client &client, const Command &cmd) {
  if (client.isRegistered()) {
    client.sendMessage(":" SERVER_NAME " " ERR_ALREADYREGISTERED " " +
                       client.getNickname() + " :You may not reregister\r\n");
    return;
  }

  if (cmd.paramCount() < 3 || !cmd.hasTrailing()) {
    client.sendMessage(":" SERVER_NAME " " ERR_NEEDMOREPARAMS
                       " * USER :Not enough parameters\r\n");
    return;
  }

  client.setUsername(cmd.getParam(0));
  client.setRealname(cmd.getTrailing());

  client.setUserAuth(true);

  if (client.hasNickAuth()) {
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
