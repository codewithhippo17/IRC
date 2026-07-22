#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <set>
#include <string>

class Client;

class Channel {
public:
  Channel();
  Channel(const std::string &name);
  ~Channel();

  /* ── Name ───────────────────────────────────────────────────────────── */
  const std::string &getName() const;

  /* ── Members ────────────────────────────────────────────────────────── */
  // NOTE: hippo should look at this.
  void addMember(Client *client);
  void removeMember(Client *client);
  bool isMember(Client *client) const;
  int getMemberCount() const;
  std::set<Client *> &getMembers();

  /* ── Operators ──────────────────────────────────────────────────────── */
  void addOperator(Client *client);
  void removeOperator(Client *client);
  bool isOperator(Client *client) const;

  /* ── Invitations (for +i mode) ──────────────────────────────────────── */
  void addInvited(Client *client);
  void removeInvited(Client *client);
  bool isInvited(Client *client) const;

  /* ── Topic ──────────────────────────────────────────────────────────── */
  const std::string &getTopic() const;
  void setTopic(const std::string &topic);

  /* ── Mode flags ─────────────────────────────────────────────────────── */
  bool isInviteOnly() const;
  void setInviteOnly(bool val);
  bool isTopicRestricted() const;
  void setTopicRestricted(bool val);
  bool hasKey() const;
  void setKey(const std::string &key);
  const std::string &getKey() const;
  void removeKey();
  bool hasLimit() const;
  void setLimit(int limit);
  int getLimit() const;
  void removeLimit();

  /* ── Broadcast message to all members (except 'exclude') ────────────── */
  void broadcast(const std::string &message, Client *exclude = 0);

  /* ── Mode string for RPL_CHANNELMODEIS ──────────────────────────────── */
  // std::string getModeString() const;

private:
  std::string _name;
  std::string _topic;
  std::string _key;
  int _userLimit;

  std::set<Client *> _members;
  std::set<Client *> _operators;
  std::set<Client *> _invited;

  /* Mode flags */
  bool _inviteOnly;      /* +i */
  bool _topicRestricted; /* +t */
  bool _hasKey;          /* +k */
  bool _hasLimit;        /* +l */
};

#endif
