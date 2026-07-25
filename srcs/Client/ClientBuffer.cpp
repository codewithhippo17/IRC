#include "../../includes/Client.hpp"
#include <sys/socket.h>
#include <unistd.h>

/* ── Read buffer (partial TCP messages) ─────────────────────────────────── */
void Client::appendToBuffer(const std::string &data) { _readBuffer += data; }

std::vector<std::string> Client::extractMessages() {
  std::vector<std::string> messages;
  size_t pos;

  while ((pos = _readBuffer.find("\r\n")) != std::string::npos ||
         (pos = _readBuffer.find('\n')) != std::string::npos) {
    std::string msg = _readBuffer.substr(0, pos);
    _readBuffer.erase(0, pos + 1);
    if (_readBuffer[0] == '\n')
      _readBuffer.erase(0, 1);
    if (!msg.empty())
      messages.push_back(msg);
  }
  return messages;
}

/* ── Send buffer (non-blocking write) ───────────────────────────────────── */
const std::string &Client::getSendBuffer() const { return _sendBuffer; }

void Client::appendToSendBuffer(const std::string &data) {
  _sendBuffer += data;
}

void Client::clearSendBuffer() { _sendBuffer.clear(); }

bool Client::hasPendingSend() const { return !_sendBuffer.empty(); }

void Client::sendMessage(const std::string &msg) { _sendBuffer += msg; }
