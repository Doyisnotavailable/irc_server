#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <iostream>
#include "Client.hpp"

class Client;
class Server;
class Channel {
    private:
		std::string channelName;
		std::vector<class Client> clientlist;
		std::vector<class Client> operlist;
		std::string topic;
		static bool invFlag; //flag for invite only channel
		static bool passFlag; //flag for channel password
		static bool topicFlag; //flag for topic operator
		static bool userFlag; // flag for user limit

    public:
		Channel();
		~Channel();
		Channel(const std::string& chName);
		std::string getchannelName() const;
		Client* getClient(const std::string& nName);
		void addClient(Client& client);

};

#endif