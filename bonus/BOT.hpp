#ifndef BOT_HPP
#define BOT_HPP

#include "../includes/Server.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Client.hpp"
#include "../includes/Errormsg.hpp"

class BOT {
	private:
		std::vector<std::string> swearWords;

	public:
		BOT();
		~BOT();
		BOT(const BOT& tocpy);
		BOT& operator=(const BOT& toasgn);
		
		bool sensorMsg(const std::string& msg);

		bool containsSwearWord(const std::string& msg);
};


#endif