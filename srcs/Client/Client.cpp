#include "../../includes/Client.hpp"
#include <algorithm>

/* ── Construction / Destruction ─────────────────────────────────────────── */
Client::Client()
	: _fd(-1), _passAuth(false), _nickAuth(false),
	  _userAuth(false), _registered(false)
{}

Client::Client(int fd, const std::string &hostname)
	: _fd(fd), _passAuth(false), _nickAuth(false),
	  _userAuth(false), _registered(false), _hostname(hostname)
{}

Client::~Client() {}

/* ── Socket ─────────────────────────────────────────────────────────────── */
int Client::getFd() const { return _fd; }

/* ── Authentication state ───────────────────────────────────────────────── */
bool Client::hasPassAuth() const { return _passAuth; }
bool Client::hasNickAuth() const { return _nickAuth; }
bool Client::hasUserAuth() const { return _userAuth; }
bool Client::isRegistered() const { return _registered; }

void Client::setPassAuth(bool val) { _passAuth = val; }
void Client::setNickAuth(bool val) { _nickAuth = val; }
void Client::setUserAuth(bool val) { _userAuth = val; }
void Client::setRegistered(bool val) { _registered = val; }

/* ── User info ──────────────────────────────────────────────────────────── */
const std::string &Client::getNickname() const { return _nickname; }
const std::string &Client::getUsername() const { return _username; }
const std::string &Client::getHostname() const { return _hostname; }
const std::string &Client::getRealname() const { return _realname; }

void Client::setNickname(const std::string &nick) { _nickname = nick; }
void Client::setUsername(const std::string &user) { _username = user; }
void Client::setHostname(const std::string &host) { _hostname = host; }
void Client::setRealname(const std::string &real) { _realname = real; }

/* ── IRC prefix :nick!user@host ─────────────────────────────────────────── */
std::string Client::getPrefix() const
{
	return _nickname + "!" + _username + "@" + _hostname;
}

/* ── Channel tracking ───────────────────────────────────────────────────── */
void Client::addChannel(const std::string &channel)
{
	if (!isInChannel(channel))
		_channels.push_back(channel);
}

void Client::removeChannel(const std::string &channel)
{
	std::vector<std::string>::iterator it =
		std::find(_channels.begin(), _channels.end(), channel);
	if (it != _channels.end())
		_channels.erase(it);
}

bool Client::isInChannel(const std::string &channel) const
{
	return std::find(_channels.begin(), _channels.end(), channel)
		!= _channels.end();
}

const std::vector<std::string> &Client::getChannels() const
{
	return _channels;
}
