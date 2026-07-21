#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Server.hpp"
#include "../includes/Command.hpp"

int main()
{
    Server server;
    Client client;

    Command command("Join #chan pw");
    
    server._cmdJoin(client, command);
    server._cmdTopic(client, command);
    server._cmdMode(client, command);
    server._cmdPrivmsg(client, command);
    server._cmdInvite(client, command);
    server._cmdKick(client, command);
    server._cmdPart(client, command);
}