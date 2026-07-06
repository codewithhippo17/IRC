#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <set>
# include <map>

class Client;

class Channel
{
    private:
        std::string _name;
        std::string _topic;
        std::set<Client *>  _members;
        std::set<Client *>  _operators;
        std::set<Client *>  _invited;

        bool    _inviteOnly;
        bool    _topicRestricted;
        bool    _hasKey;
        std::string	    _key;
        bool    _hasUserLimit;
        size_t  _userLimit;

	public:
		Channel(const std::string &name);
		~Channel();

		const std::string	&getName() const;


		void				addClient(Client *client);
		void				removeClient(Client *client);
		bool				isMember(Client *client) const;
		const std::set<Client *> &getMembers() const;


		void				addOperator(Client *client);
		void				removeOperator(Client *client);
		bool				isOperator(Client *client) const;


		const std::string	&getTopic() const;
		void				setTopic(const std::string &topic);


		bool				isInviteOnly() const;
		void				setInviteOnly(bool value);
		bool				isTopicRestricted() const;
		void				setTopicRestricted(bool value);

		bool				hasKey() const;
		const std::string	&getKey() const;
		void				setKey(const std::string &key);
		void				removeKey();

		bool				hasUserLimit() const;
		size_t				getUserLimit() const;
		void				setUserLimit(size_t limit);
		void				removeUserLimit();

		void				addInvite(Client *client);
		bool				isInvited(Client *client) const;

};

#endif