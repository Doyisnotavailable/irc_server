#include "BOT.hpp"

// bool censorFlag = false;

BOT::BOT() {
	swearWords.push_back("SHIT");
	swearWords.push_back("DAMN");
	swearWords.push_back("HELL");
    // Et cetera 
}

BOT::~BOT() {}

bool BOT::censorMsg(const std::string& msg) {
    return containsSwearWord(msg);
}

bool BOT::containsSwearWord(const std::string& msg) {
    std::string upperMsg;

    // Change msg to upper-case
    for (size_t j = 0; j < msg.length(); j++) {
        upperMsg += (char)std::toupper(msg[j]);
    }

    for (std::vector<std::string>::iterator it = swearWords.begin(); it != swearWords.end(); ++it) {
        if (upperMsg == *it || upperMsg == ":" + *it) {
            return true;
        }
    }
    return false;
}

void sigHandlerBOT(int signum) {
    (void) signum;
    std::cout << "\nBOT QUITING\r\n" << std::endl;
    _censorFlag = true;
}

