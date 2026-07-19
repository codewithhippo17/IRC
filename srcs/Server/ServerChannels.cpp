
#include "Server.hpp"


void Server::_removeClientFromAllChannels(int fd)
{
	std::map<int, Client *>::iterator clientIt = _clients.find(fd);
	if (clientIt == _clients.end())
		return;

	Client *client = clientIt->second;

	std::vector<std::string> channels = client->getChannels();

	for (size_t i = 0; i < channels.size(); i++)
	{
		std::map<std::string, Channel>::iterator chanIt =
			_channels.find(channels[i]);
		if (chanIt == _channels.end())
			continue;

		std::string quitMsg = ":" + client->getPrefix() + " QUIT :Connection lost\r\n";
		chanIt->second.broadcast(quitMsg, client);

		chanIt->second.removeMember(client);
		chanIt->second.removeOperator(client);
		chanIt->second.removeInvited(client);

		if (chanIt->second.getMemberCount() == 0)
			_channels.erase(chanIt);
	}
}
