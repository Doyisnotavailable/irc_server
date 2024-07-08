#include "BOT.hpp"


BOT::BOT() {
	swearWords.push_back("SHIT");
	swearWords.push_back("DAMN");
	swearWords.push_back("HELL");
	swearWords.push_back("FUCK");
	swearWords.push_back("ASSHOLE");
	swearWords.push_back("BASTARD");
	swearWords.push_back("WHORE");
	// = {
	// 		"SHIT", "DAMN", "HELL", "FUCK", "CRAP", "ASSHOLE", "BITCH", "BASTARD",
	// 		"CUNT", "DICK", "PISS", "PRICK", "SLUT", "WHORE", "TWAT", "WANKER"
	// 	};


}

BOT::~BOT() {

}

BOT::BOT(const BOT& tocpy) {
	*this = tocpy;
}

BOT& BOT::operator=(const BOT& toasgn) {
	if (this != &toasgn) {

	}
	return *this;
}

bool BOT::sensorMsg(const std::string& msg) {
    return containsSwearWord(msg);
}

bool BOT::containsSwearWord(const std::string& msg) {
    std::string upperMsg;

    // Change msg to upper-case
    for (size_t j = 0; j < msg.length(); j++) {
        upperMsg += (char)std::toupper(msg[j]);
    }

    for (std::vector<std::string>::iterator it = swearWords.begin(); it != swearWords.end(); ++it) {
        if (upperMsg.find(*it) != std::string::npos) {
            return true;
        }
    }
    return false;
}

void sendIrcCommand(int sockfd, const std::string& command) {
    std::string fullCommand = command + "\r\n";
    if (send(sockfd, fullCommand.c_str(), fullCommand.length(), 0) < 0) {
        std::cerr << "Error sending command: " << command << std::endl;
    }
}

