#include "../../includes/Utils.hpp"
#include <algorithm>
#include <cctype>
#include <sys/socket.h>
#include <unistd.h>

/* ── String helpers ─────────────────────────────────────────────────────── */
std::string toUpper(const std::string &str) {
  std::string result = str;
  for (size_t i = 0; i < result.size(); i++)
    result[i] = std::toupper(static_cast<unsigned char>(result[i]));
  return result;
}

std::string trim(const std::string &str) {
  size_t start = 0;
  while (start < str.size() &&
         std::isspace(static_cast<unsigned char>(str[start])))
    start++;
  size_t end = str.size();
  while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1])))
    end--;
  return str.substr(start, end - start);
}

std::vector<std::string> split(const std::string &str, char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  for (size_t i = 0; i < str.size(); i++) {
    if (str[i] == delimiter) {
      if (!token.empty()) {
        tokens.push_back(token);
        token.clear();
      }
    } else {
      token += str[i];
    }
  }
  if (!token.empty())
    tokens.push_back(token);
  return tokens;
}

/* ── Network helper ─────────────────────────────────────────────────────── */
void sendToClient(int fd, const std::string &message) {
  (void)fd;
  (void)message;
}
