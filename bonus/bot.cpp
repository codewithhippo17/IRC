#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        std::cerr << "Usage: ./ircbot <host> <port> <password>" << std::endl;
        return 1;
    }

    std::string host = argv[1];
    int port = std::atoi(argv[2]);
    std::string password = argv[3];

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    struct hostent *server = gethostbyname(host.c_str());
    if (!server)
    {
        std::cerr << "Error: no such host" << std::endl;
        return 1;
    }

    struct sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    std::memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Error connecting to server" << std::endl;
        return 1;
    }

    std::cout << "Connected to IRC server. Authenticating..." << std::endl;

    std::string auth = "PASS " + password + "\r\n"
                                            "NICK JockerBot\r\n"
                                            "USER bot 0 * :Jocker Bot\r\n";
    send(sock, auth.c_str(), auth.length(), 0);

    char buffer[1024];
    std::string readBuf;

    while (true)
    {
        int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0)
        {
            std::cout << "Disconnected from server." << std::endl;
            break;
        }
        buffer[bytes] = '\0';
        readBuf += buffer;

        size_t pos;
        while ((pos = readBuf.find("\n")) != std::string::npos)
        {
            std::string msg = readBuf.substr(0, pos);
            readBuf.erase(0, pos + 1);
            if (!msg.empty() && msg[msg.length() - 1] == '\r')
                msg.erase(msg.length() - 1);

            std::cout << "<< " << msg << std::endl;

            if (msg.find("PING ") == 0)
            {
                std::string pong = "PONG " + msg.substr(5) + "\r\n";
                send(sock, pong.c_str(), pong.length(), 0);
                std::cout << ">> " << pong;
                continue;
            }

            size_t cmdPos = msg.find(" PRIVMSG ");
            if (cmdPos == std::string::npos || msg.empty() || msg[0] != ':')
                continue;

            size_t targetStart = cmdPos + 9;
            size_t targetEnd = msg.find(' ', targetStart);
            if (targetEnd == std::string::npos)
                continue;

            std::string target = msg.substr(targetStart, targetEnd - targetStart);
            size_t textStart = msg.find(" :", targetEnd);
            if (textStart == std::string::npos)
                continue;

            std::string text = msg.substr(textStart + 2);

            if (text.find("jocker") != std::string::npos)
            {
                std::string prefix = msg.substr(1, cmdPos - 1);
                std::string senderNick = prefix.substr(0, prefix.find('!'));

                std::string replyTarget = (!target.empty() && target[0] == '#') ? target : senderNick;

                std::string reply = "PRIVMSG " + replyTarget + " :welcome to 1337 campus\r\n";
                send(sock, reply.c_str(), reply.length(), 0);
                std::cout << ">> " << reply;
            }
        }
    }

    close(sock);
    return 0;
}