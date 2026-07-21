NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g
INC = -I includes/

#SRCS =	srcs/main.cpp \
		srcs/Server/Server.cpp \
		srcs/Server/ServerRun.cpp \
		srcs/Server/ServerClient.cpp \
		srcs/Server/ServerChannels.cpp \
		srcs/Server/ServerCommands.cpp \
		srcs/Client/Client.cpp \
		srcs/Client/ClientBuffer.cpp \
		srcs/Channel/Channel.cpp \
		srcs/Channel/ChannelModes.cpp \
		srcs/Channel/ChannelOps.cpp \
		srcs/Commands/Pass.cpp \
		srcs/Commands/Nick.cpp \
		srcs/Commands/User.cpp \
		srcs/Commands/Join.cpp \
		srcs/Commands/Privmsg.cpp \
		srcs/Commands/Kick.cpp \
		srcs/Commands/Invite.cpp \
		srcs/Commands/Topic.cpp \
		srcs/Commands/Mode.cpp \
		srcs/Commands/Part.cpp \
		srcs/Commands/Quit.cpp \
		srcs/Commands/Ping.cpp \
		srcs/Commands/Whois.cpp \
		srcs/Command/Command.cpp \
		srcs/Utils/Utils.cpp

SRCS =	srcs/main.cpp \
		srcs/Channel/Channel.cpp \
		srcs/Channel/ChannelModes.cpp \
		srcs/Channel/ChannelManager.cpp \
		srcs/Commands/Join.cpp \
		srcs/Commands/Kick.cpp \
		srcs/Commands/Invite.cpp \
		srcs/Commands/Privmsg.cpp \
		srcs/Commands/Topic.cpp \
		srcs/Commands/Part.cpp \
		srcs/Commands/Mode.cpp \
		srcs/Command/Command.cpp \

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
