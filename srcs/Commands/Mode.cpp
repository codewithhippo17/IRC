#include "../../includes/Server.hpp"

void Server::_cmdMode(Client &client, const Command &cmd)
{
    std::string channelName = cmd.getParams()[0];
    std::map<std::string, Channel>::iterator it = _channels.find(channelName);
    
    if(it == _channels.end())
    {
        client.sendMessage(":" SERVER_NAME " " ERR_NOSUCHCHANNEL " " + client.getNickname() + " " + channelName + " :No such Channel\r\n");
        return ;
    }
    Channel &channel = it->second;

    if(!channel.isOperator(&client))
    {
        client.sendMessage(":" SERVER_NAME " " ERR_CHANOPRIVSNEEDED " " + client.getNickname() + " " + channelName + " :You're not channel operator\r\n");
        return ;
    }

    std::string modes = cmd.getParams()[1];
    size_t argIndex = 2;
    bool adding = true;

    for(size_t i = 0; i < modes.size(); i++)
    {
        char c = modes[i];
        if(c == '+')
        {
            adding = true;
            continue;
        }
        if(c == '-')
        {
            adding = false;
            continue;
        }

        if(c == 'i')
        {
            channel.setInviteOnly(adding);
            continue;
        }

        if(c == 't')
        {
            channel.setTopicRestricted(adding);
            continue;
        }

        if(c == 'k')
        {
            if(adding)
            {
                if(argIndex >= cmd.getParams().size())
                {
                    client.sendMessage(":" SERVER_NAME " " ERR_NEEDMOREPARAMS " " + client.getNickname() + " " + cmd.getName() + " :Not enough parameters\r\n");
                    continue;
                }
                std::string key = cmd.getParams()[argIndex];
                argIndex++;
                channel.setKey(key);
            }
            else
            {
                channel.removeKey();
            }
            continue;
        }
                
        if(c == 'l')
        {
            if(adding)
            {
                if(argIndex >= cmd.getParams().size())
                {
                    client.sendMessage(":" SERVER_NAME " " ERR_NEEDMOREPARAMS " " + client.getNickname() + " " + cmd.getName() + " :Not enough parameters\r\n");
                    continue;
                }
                std::string limitstr = cmd.getParams()[argIndex];
                std::stringstream ss(limitstr);
                size_t limit;
                ss >> limit;
                argIndex++;
                channel.setUserLimit(limit);
            }
            else
            {
                channel.removeUserLimit();
            }
            continue;
        }

        if (c == 'o')
        {
            if (argIndex >= cmd.getParams().size())
            {
                client.sendMessage(":" SERVER_NAME " " ERR_NEEDMOREPARAMS " " + client.getNickname() + " " + cmd.getName() + " :Not enough parameters\r\n");
                continue;
            }
            std::string targetNick = cmd.getParams()[argIndex];
            argIndex++;

            Client *target = _findClientByNick(targetNick);
            if (!target || !channel.isMember(target))
            {
                client.sendMessage(":" SERVER_NAME " " ERR_USERNOTINCHANNEL " " + client.getNickname() + " " + targetNick + " " + channelName + " :They aren't on that channel\r\n");
                continue;
            }

            if (adding)
                channel.addOperator(target);
            else
                channel.removeOperator(target);
            continue;
        }
        
    }
}