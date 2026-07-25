#include "Replies.hpp"
#include "Server.hpp"
#include "Utils.hpp"

void Server::_cmdWhois(Client &client, const Command &cmd) {
  if (cmd.getParams().empty()) {
    client.sendMessage(":" SERVER_NAME " " ERR_NONICKNAMEGIVEN " " +
                       client.getNickname() + " :No nickname given\r\n");
    return;
  }

  std::string target = cmd.getParams()[0];
  Client *targetClient = _findClientByNick(target);

  if (!targetClient) {
    client.sendMessage(":" SERVER_NAME " " ERR_NOSUCHNICK " " +
                       client.getNickname() + " " + target +
                       " :No such nick/channel\r\n");
  } else {
    client.sendMessage(":" SERVER_NAME " " RPL_WHOISUSER " " +
                       client.getNickname() + " " + target + " " +
                       targetClient->getUsername() + " " +
                       targetClient->getHostname() +
                       " * :" + targetClient->getRealname() + "\r\n");
    client.sendMessage(":" SERVER_NAME " " RPL_ENDOFWHOIS " " +
                       client.getNickname() + " " + target +
                       " :End of WHOIS list\r\n");
  }
}
