#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>

class Client{

    public:
    Client() {}
    std::string getNickname()
    {
        return "noura";
    }

    void sendMessage(std::string message)
    {
        std::cout << message << std::endl;
    }

};



#endif