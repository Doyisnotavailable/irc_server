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
		std::string key;
		std::string topic;
		bool invFlag; //flag for invite only channel
		bool keyFlag; //flag for channel password
		bool topicFlag; //flag for topic operator
		bool clientFlag; // flag for user limit
		int	limit;
    public:
		Channel();
		// Channel(std::string line);
		~Channel();
		Channel(const std::string& chName);
		Channel(const std::string& chName, Client& cl);
		Channel& operator=(const Channel& toasgn);
		void addClient(Client& client);

		//getter setter here//
		const std::string getKey() const;
		std::string getchannelName() const;
		Client* getClient(const std::string& nName);
		bool getinvFlag();
		bool getkeyFlag();
		bool gettopicFlag();
		bool getclientFlag();
		int getclientSize();
		int getlimit();
		std::vector<class Client> getclientList();

		void setinvFlag(bool a);
		void setkeyFlag(bool a);
		void settopicFlag(bool a);
		void setclientFlag(bool a);
		void setClientOper(Client* cl, char c);
		// ------------------ //

		bool checkclientExist(Client* cl);
		bool checkclientOper(Client* cl);
		bool joinFlags(); //function to check if any of a flag is true when joining
		void removeClient(Client *cl);
		std::string getTopic() const;
		void  setTopic(const std::string& top);
};

#endif