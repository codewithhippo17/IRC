
#include "Server.hpp"
#include "Replies.hpp"
#include "Utils.hpp"

void Server::_cmdPass(Client &client, const Command &cmd)
{
	if (client.isRegistered())
	{
		client.sendMessage(":" SERVER_NAME " " ERR_ALREADYREGISTERED " " + client.getNickname() + " :You may not reregister\r\n");
		return;
	}

	if (cmd.paramCount() == 0 && !cmd.hasTrailing())
	{
		client.sendMessage(":" SERVER_NAME " " ERR_NEEDMOREPARAMS
						   " * PASS :Not enough parameters\r\n");
		return;
	}

	std::string pass = cmd.paramCount() > 0 ? cmd.getParam(0) : cmd.getTrailing();

	if (pass != _password)
	{
		client.sendMessage(":" SERVER_NAME " " ERR_PASSWDMISMATCH
						   " * :Password incorrect\r\n");
		return;
	}

	client.setPassAuth(true);
}
