#ifndef CHANNELMANAGER_HPP
#define CHANNELMANAGER_HPP

#include "Channel.hpp"
#include <map>
#include <string>

class ChannelManager {
private:
  std::map<std::string, Channel *> channels;
  ChannelManager(const ChannelManager &other);
  ChannelManager &operator=(const ChannelManager &other);

public:
  ChannelManager();
  ~ChannelManager();

  Channel *getChannel(const std::string &name);
  Channel *createChannel(const std::string &name);
  void removeChannel(const std::string &name);
};

#endif
