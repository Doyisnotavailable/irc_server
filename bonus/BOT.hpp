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
		// bool censorFlag;

	public:
		BOT();
		~BOT();
		BOT(const BOT& tocpy);
		BOT& operator=(const BOT& toasgn);
		
		bool censorMsg(const std::string& msg);

		bool containsSwearWord(const std::string& msg);

		// bool getcensorFlag() const;
		// void setcensorFlag(bool flag);

};

void sigHandlerBOT(int signum);

#endif