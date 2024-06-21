#include "../includes/Channel.hpp"

class Client;

Channel::Channel() {
	this->channelName = "";
}

Channel::~Channel() {

}

Channel::Channel(const std::string& chName): channelName(chName){

}

Channel& Channel::operator=(const Channel& toasgn) {
	if (this != &toasgn) {
		channelName = toasgn.channelName;
        clientlist = toasgn.clientlist;
        operlist = toasgn.operlist;
        key = toasgn.key;            
		topic = toasgn.topic;
        invFlag = toasgn.invFlag;
        keyFlag = toasgn.keyFlag;
        topicFlag = toasgn.topicFlag;           
		clientFlag = toasgn.clientFlag;
		limit = toasgn.limit;
	}
	return *this;
}

// channel constructor for creating a channel
Channel::Channel(const std::string& chName, Client& cl): channelName(chName){
	invFlag = false;
	keyFlag = false;
	topicFlag = false;
	clientFlag = false;
	key = "";
	topic = "";
	limit = 0;
	clientlist.push_back(cl);
	operlist.push_back(cl);
}

void Channel::addClient(Client& client) {
	client.addChannel(*this);
	this->clientlist.push_back(client);
}

std::string Channel::getchannelName() const {
	return this->channelName;
}

bool Channel::getinvFlag() {
	return this->invFlag;
}

bool Channel::getkeyFlag() {
	return this->keyFlag;
}

bool Channel::gettopicFlag() {
	return this->topicFlag;
}

bool Channel::getclientFlag() {
	return this->clientFlag;
}
const std::string Channel::getKey() const {
	return this->key;
}

int Channel::getlimit() {
	return this->limit;
}

int Channel::getclientSize(){
	return this->clientlist.size();
}
bool Channel::joinFlags() {
	if (getinvFlag() || getkeyFlag() || getclientFlag())
		return true;
	return false;
}

bool Channel::checkclientExist(Client* cl) {
	for(size_t i = 0; i < clientlist.size(); ++i) {
		if (cl->getfd() == clientlist[i].getfd())
			return true;
	}
	return false;
}