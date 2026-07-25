
#include "Replies.hpp"
#include "Server.hpp"
#include "Utils.hpp"

void Server::_cmdPing(Client &client, const Command &cmd) {
  if (cmd.getParams().empty()) {
    client.sendMessage(":" SERVER_NAME " " ERR_NOORIGIN " " +
                       client.getNickname() + " :No origin specified\r\n");
    return;
  }

  client.sendMessage(":" SERVER_NAME " PONG " SERVER_NAME " :" +
                     cmd.getParams()[0] + "\r\n");
}
