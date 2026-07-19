#include "Server.hpp"
#include "Colors.hpp"
#include <iostream>
#include <cstdlib>
#include <csignal>
#include <cctype>

/*
** isValidPort — check that the port string is a number in [1024, 65535]
*/
static bool isValidPort(const char *str)
{
	if (!str || !*str)
		return false;
	for (int i = 0; str[i]; i++)
	{
		if (!std::isdigit(static_cast<unsigned char>(str[i])))
			return false;
	}
	long port = std::atol(str);
	return (port >= 1024 && port <= 65535);
}

int main(int argc, char **argv)
{
	/* ── Validate arguments ────────────────────────────────────────────── */
	if (argc != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	if (!isValidPort(argv[1]))
	{
		std::cerr << "Error: port must be a number between 1024 and 65535" << std::endl;
		return 1;
	}

	std::string password = argv[2];
	if (password.empty())
	{
		std::cerr << "Error: password cannot be empty" << std::endl;
		return 1;
	}

	int port = std::atoi(argv[1]);

	/* ── Setup signal handlers ─────────────────────────────────────────── */
	signal(SIGINT, Server::signalHandler);
	signal(SIGQUIT, Server::signalHandler);
	signal(SIGPIPE, SIG_IGN); /* Ignore broken pipe */

	/* ── Create and run server ─────────────────────────────────────────── */
	try
	{
		Server server(port, password);
		server.runServer();
	}
	catch (const std::exception &e)
	{
		std::cerr << CLR_RED << "Fatal: " << e.what()
				  << CLR_RESET << std::endl;
		return 1;
	}

	return 0;
}
