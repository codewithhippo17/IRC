#ifndef UTILS_HPP
#define UTILS_HPP

#include <algorithm>
#include <cctype>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

/* ── String helpers ─────────────────────────────────────────────────────── */
std::string toUpper(const std::string &str);
std::string trim(const std::string &str);
std::vector<std::string> split(const std::string &str, char delimiter);

/* ── Network helper ─────────────────────────────────────────────────────── */
void sendToClient(int fd, const std::string &message);

#endif
