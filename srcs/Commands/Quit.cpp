
#include "Server.hpp"
#include "Replies.hpp"
#include "Utils.hpp"

void Server::_cmdQuit(Client &client, const Command &cmd)
{
	std::string reason = cmd.hasTrailing()
							 ? cmd.getTrailing()
							 : "Client Quit";

	std::string prefix = client.getPrefix();

	std::string quitMsg = ":" + prefix + " QUIT :" + reason + "\r\n";
	const std::vector<std::string> &channels = client.getChannels();
	for (size_t i = 0; i < channels.size(); i++)
	{
		std::map<std::string, Channel>::iterator it =
			_channels.find(channels[i]);
		if (it != _channels.end())
			it->second.broadcast(quitMsg, &client);
	}

	client.sendMessage("ERROR :Closing link (" + prefix + ") [Quit: " + reason + "]\r\n");


	_removeClient(client.getFd());
}
