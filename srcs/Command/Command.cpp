#include "Command.hpp"

Command::Command() : _hasTrailing(false) {}

Command::~Command() {}

Command Command::parse(const std::string &raw) {
  Command cmd;
  std::string line = raw;

  while (!line.empty() &&
         (line[line.length() - 1] == '\r' || line[line.length() - 1] == '\n'))
    line.erase(line.length() - 1);

  if (line.empty())
    return cmd;

  size_t pos = 0;

  if (line[0] == ':') {
    size_t space = line.find(' ', 1);
    if (space == std::string::npos)
      return cmd;
    cmd._prefix = line.substr(1, space - 1);
    pos = space + 1;
    while (pos < line.length() && line[pos] == ' ')
      pos++;
  }

  size_t space = line.find(' ', pos);
  if (space == std::string::npos) {
    cmd._command = toUpper(line.substr(pos));
    return cmd;
  }
  cmd._command = toUpper(line.substr(pos, space - pos));
  pos = space + 1;
  while (pos < line.length() && line[pos] == ' ')
    pos++;

  while (pos < line.length()) {
    if (line[pos] == ':') {
      cmd._trailing = line.substr(pos + 1);
      cmd._hasTrailing = true;
      break;
    }
    space = line.find(' ', pos);
    if (space == std::string::npos) {
      cmd._params.push_back(line.substr(pos));
      break;
    }
    cmd._params.push_back(line.substr(pos, space - pos));
    pos = space + 1;
    while (pos < line.length() && line[pos] == ' ')
      pos++;
  }

  return cmd;
}

const std::string &Command::getPrefix() const { return _prefix; }

const std::string &Command::getCommand() const { return _command; }

const std::vector<std::string> &Command::getParams() const { return _params; }

const std::string &Command::getTrailing() const { return _trailing; }

bool Command::hasTrailing() const { return _hasTrailing; }

std::string Command::getParam(size_t index) const {
  if (index < _params.size())
    return _params[index];
  return "";
}

size_t Command::paramCount() const { return _params.size(); }
