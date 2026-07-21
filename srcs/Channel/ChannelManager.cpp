#include "../../includes/Channelmanager.hpp"

ChannelManager::ChannelManager() {}

ChannelManager::~ChannelManager()
{
	std::map<std::string, Channel *>::iterator it;

	for (it = channels.begin(); it != channels.end(); ++it)
		delete it->second;
	channels.clear();
}

Channel *ChannelManager::getChannel(const std::string &name)
{
	std::map<std::string, Channel *>::iterator it = channels.find(name);

	if (it == channels.end())
		return 0;
	return it->second;
}

Channel *ChannelManager::createChannel(const std::string &name)
{
	Channel *channel = new Channel(name);
	channels[name] = channel;
	return channel;
}

void ChannelManager::removeChannel(const std::string &name)
{
	std::map<std::string, Channel *>::iterator it = channels.find(name);

	if (it == channels.end())
		return;
	delete it->second;
	channels.erase(it);
}