#include "../../includes/Server.hpp"
#include <sstream>

void Server::_cmdMode(Client &client, const Command &cmd)
{
    std::string channelName = cmd.getParams()[0];
    std::map<std::string, Channel>::iterator it = _channels.find(channelName);
    
    if(it == _channels.end())
    {
        _sendReply(client, ERR_NOSUCHCHANNEL);
        return ;
    }
    Channel &channel = it->second;

    if(!channel.isOperator(&client))
    {
        _sendReply(client, ERR_CHANOPRIVSNEEDED);
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
                    _sendReply(client, ERR_NEEDMOREPARAMS);
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
                    _sendReply(client, ERR_NEEDMOREPARAMS);
                    continue;
                }
                std::string limitstr = cmd.getParams()[argIndex];
                std::istringstream ss(limitstr);
                int limit;
                ss >> limit;
                argIndex++;
                channel.setLimit(limit);
            }
            else
            {
                channel.removeLimit();
            }
            continue;
        }

        if (c == 'o')
        {
            if (argIndex >= cmd.getParams().size())
            {
                _sendReply(client, ERR_NEEDMOREPARAMS);
                continue;
            }
            std::string targetNick = cmd.getParams()[argIndex];
            argIndex++;

            Client *target = _findClientByNick(targetNick);
            if (!target || !channel.isMember(target))
            {
                _sendReply(client, ERR_USERNOTINCHANNEL);
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