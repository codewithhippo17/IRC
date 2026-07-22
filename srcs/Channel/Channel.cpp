#include "../../includes/Channel.hpp"
#include "../../includes/Client.hpp"

/* ── Construction / Destruction ─────────────────────────────────────────── */
Channel::Channel()
    : _userLimit(0), _inviteOnly(false), _topicRestricted(false),
      _hasKey(false), _hasLimit(false) {}

Channel::Channel(const std::string &name)
    : _name(name), _topic(""), _key(""), _userLimit(0), _inviteOnly(false),
      _topicRestricted(false), _hasKey(false), _hasLimit(false) {}

Channel::~Channel() {}

/* ── Name ──────────────────────────────────────────────────────────────── */
const std::string &Channel::getName() const { return _name; }

/* ── Members ───────────────────────────────────────────────────────────── */
void Channel::addMember(Client *client) { _members.insert(client); }

void Channel::removeMember(Client *client) {
  _members.erase(client);
  _operators.erase(client);
  _invited.erase(client);
}

bool Channel::isMember(Client *client) const {
  return _members.find(client) != _members.end();
}

int Channel::getMemberCount() const { return _members.size(); }

std::set<Client *> &Channel::getMembers() { return _members; }

/* ── Operators ─────────────────────────────────────────────────────────── */
void Channel::addOperator(Client *client) { _operators.insert(client); }

void Channel::removeOperator(Client *client) { _operators.erase(client); }

bool Channel::isOperator(Client *client) const {
  return _operators.find(client) != _operators.end();
}

/* ── Invitations (for +i mode) ─────────────────────────────────────────── */
void Channel::addInvited(Client *client) { _invited.insert(client); }

void Channel::removeInvited(Client *client) { _invited.erase(client); }

bool Channel::isInvited(Client *client) const {
  return _invited.find(client) != _invited.end();
}

/* ── Topic ─────────────────────────────────────────────────────────────── */
const std::string &Channel::getTopic() const { return _topic; }

void Channel::setTopic(const std::string &topic) { _topic = topic; }

/* ── Mode flags ────────────────────────────────────────────────────────── */
bool Channel::isInviteOnly() const { return _inviteOnly; }
void Channel::setInviteOnly(bool val) { _inviteOnly = val; }
bool Channel::isTopicRestricted() const { return _topicRestricted; }
void Channel::setTopicRestricted(bool val) { _topicRestricted = val; }

bool Channel::hasKey() const { return _hasKey; }
void Channel::setKey(const std::string &key) {
  _key = key;
  _hasKey = true;
}
const std::string &Channel::getKey() const { return _key; }
void Channel::removeKey() {
  _key.clear();
  _hasKey = false;
}

bool Channel::hasLimit() const { return _hasLimit; }
void Channel::setLimit(int limit) {
  _userLimit = limit;
  _hasLimit = true;
}
int Channel::getLimit() const { return _userLimit; }
void Channel::removeLimit() {
  _userLimit = 0;
  _hasLimit = false;
}

/* ── Broadcast ─────────────────────────────────────────────────────────── */
void Channel::broadcast(const std::string &message, Client *exclude) {
  for (std::set<Client *>::iterator it = _members.begin(); it != _members.end();
       ++it) {
    if (*it != exclude)
      (*it)->sendMessage(message + "\r\n");
  }
}

/* ── Mode string for RPL_CHANNELMODEIS ─────────────────────────────────── */
// std::string Channel::getModeString() const
// {
//     std::string modes = "+";
//     if (_inviteOnly)       modes += "i";
//     if (_topicRestricted)  modes += "t";
//     if (_hasKey)           modes += "k";
//     if (_hasLimit)         modes += "l";
//     return modes;
// }
