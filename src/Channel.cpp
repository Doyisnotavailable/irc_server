#include "../includes/Channel.hpp"

Channel::Channel() {
	this->channelName = "";
}

Channel::~Channel() {

}

Channel::Channel(const std::string& chName): channelName(chName){

}

void Channel::addClient(Client& client) {
	client.addChannel(*this);
	this->clientlist.push_back(client);
}

std::string Channel::getchannelName() const {
	return this->channelName;
}