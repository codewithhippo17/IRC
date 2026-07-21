#include "../../includes/Channel.hpp"

Channel::Channel(const std::string &name) : _name(name), _topic(""), 
    _inviteOnly(false), _topicRestricted(false), 
    _hasKey(false), _key(""), _hasUserLimit(false), 
    _userLimit(0)
{}

Channel::~Channel() {}


void Channel::addClient(Client *client)
{
	_members.insert(client);
}

void Channel::removeClient(Client *client)
{
	_members.erase(client);
	_operators.erase(client);
	_invited.erase(client);
}

bool Channel::isMember(Client *client) const
{
	return _members.find(client) != _members.end();
}

const std::set<Client *> &Channel::getMembers() const
{
	return _members;
}

void Channel::addOperator(Client *client)
{
    _operators.insert(client);
}

void Channel::removeOperator(Client *client)
{
	_operators.erase(client);
}

bool Channel::isOperator(Client *client) const
{
	return _operators.find(client) != _operators.end();
}

const std::string &Channel::getTopic() const
{
	return _topic;
}

void Channel::setTopic(const std::string &topic)
{
	_topic = topic;
}