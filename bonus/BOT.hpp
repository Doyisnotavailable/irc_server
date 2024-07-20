#ifndef BOT_HPP
#define BOT_HPP

#include "../includes/Server.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Client.hpp"
#include "../includes/Errormsg.hpp"
extern bool _censorFlag;

class BOT {
	private:
		std::vector<std::string> swearWords;

	public:
		BOT();
		~BOT();

		int registerBot(std::string channelName, const char *password, const char *ipAddress, int port, int sockfd);
		bool censorMsg(const std::string& msg);
		bool containsSwearWord(const std::string& msg);

		void monitor(struct pollfd *pollfds, std::string channelName, int sockfd);

};

void sigHandlerBOT(int signum);

#endif