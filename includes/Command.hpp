
#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <vector>

class Command
{
public:
	Command();
	~Command();

	/* Parse a raw IRC message into a Command object */
	static Command parse(const std::string &raw);

	/* Getters */
	const std::string &getPrefix() const;
	const std::string &getCommand() const;
	const std::vector<std::string> &getParams() const;
	const std::string &getTrailing() const;
	bool hasTrailing() const;

	/* Convenience */
	std::string getParam(size_t index) const;
	size_t paramCount() const;

private:
	std::string _prefix;
	std::string _command;
	std::vector<std::string> _params;
	std::string _trailing;
	bool _hasTrailing;
};

#endif
