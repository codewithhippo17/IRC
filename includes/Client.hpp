#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>

class Client {
public:
  Client();
  Client(int fd, const std::string &hostname);
  ~Client();

  /* ── Socket ─────────────────────────────────────────────────────────── */
  int getFd() const;

  /* ── Read buffer (partial TCP messages) ─────────────────────────────── */
  void appendToBuffer(const std::string &data);
  std::vector<std::string> extractMessages();

  /* ── Send buffer (non-blocking write) ───────────────────────────────── */
  const std::string &getSendBuffer() const;
  void appendToSendBuffer(const std::string &data);
  void clearSendBuffer();
  bool hasPendingSend() const;
  void sendMessage(const std::string &msg);

  /* ── Authentication state ───────────────────────────────────────────── */
  bool hasPassAuth() const;
  bool hasNickAuth() const;
  bool hasUserAuth() const;
  bool isRegistered() const;
  void setPassAuth(bool val);
  void setNickAuth(bool val);
  void setUserAuth(bool val);
  void setRegistered(bool val);

  /* ── User info ──────────────────────────────────────────────────────── */
  const std::string &getNickname() const;
  const std::string &getUsername() const;
  const std::string &getHostname() const;
  const std::string &getRealname() const;
  void setNickname(const std::string &nick);
  void setUsername(const std::string &user);
  void setHostname(const std::string &host);
  void setRealname(const std::string &real);

  /* ── IRC prefix :nick!user@host ─────────────────────────────────────── */
  std::string getPrefix() const;

  /* ── Channel tracking ───────────────────────────────────────────────── */
  void addChannel(const std::string &channel);
  void removeChannel(const std::string &channel);
  bool isInChannel(const std::string &channel) const;
  const std::vector<std::string> &getChannels() const;

private:
  int _fd;
  std::string _readBuffer;
  std::string _sendBuffer;

  /* Auth flags — must be set in order: PASS -> NICK -> USER */
  bool _passAuth;
  bool _nickAuth;
  bool _userAuth;
  bool _registered;

  /* User identity */
  std::string _nickname;
  std::string _username;
  std::string _hostname;
  std::string _realname;

  /* Channels this client has joined */
  std::vector<std::string> _channels;
};

#endif
