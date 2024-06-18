#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <iostream>
#include "Client.hpp"

class Client;

class Channel {
    private:
		std::string channelName;
		std::vector<class Client> clientlist;
    public:
		Channel();
		~Channel();
		std::string getchannelName() const;
		Client* getClient(const std::string& nName);

};

#endif