#include "../../includes/Channel.hpp"


bool Channel::isInviteOnly() const
{
	return _inviteOnly;
}

void Channel::setInviteOnly(bool value)
{
	_inviteOnly = value;
}


bool Channel::isTopicRestricted() const
{
    return _topicRestricted;
}


void Channel::setTopicRestricted(bool value)
{
    _topicRestricted = value;
}


bool Channel::hasKey() const
{
	return _hasKey;
}


const std::string &Channel::getKey() const
{
	return _key;
}


void Channel::setKey(const std::string &key)
{
	_key = key;
	_hasKey = true;
}

void Channel::removeKey()
{
	_key = "";
	_hasKey = false;
}

bool Channel::hasUserLimit() const
{
	return _hasUserLimit;
}

size_t Channel::getUserLimit() const
{
	return _userLimit;
}


void Channel::setUserLimit(size_t limit)
{
	_userLimit = limit;
	_hasUserLimit = true;
}

void Channel::removeUserLimit()
{
	_userLimit = 0;
	_hasUserLimit = false;
}

void Channel::addInvite(Client *client)
{
	_invited.insert(client);
}

bool Channel::isInvited(Client *client) const
{
	return _invited.find(client) != _invited.end();
}

void Channel::broadcast(const std::string &message, Client *exclude)
{
	std::set<Client *>::const_iterator it;

	for (it = _members.begin(); it != _members.end(); ++it)
	{
		if (*it != exclude)
			(*it)->sendMessage(message);
	}
}