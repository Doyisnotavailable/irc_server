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
	Client *cl = &client;
	Channel ch = *this;
	// client.addChannel(*this);
	// this->clientlist.push_back(client);
	cl->addChannel(ch);
	this->clientlist.push_back(*cl);
	// this->operlist.push_back(*cl);
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

std::string Channel::getMode(){
	std::string mode = "";
	if (getinvFlag())
		mode += "i";
	if (getkeyFlag())
		mode += "k";
	if (gettopicFlag())
		mode += "t";
	if (getclientFlag())
		mode += "l";
	return mode;
}

std::vector<class Client> Channel::getclientList(){
	return this->clientlist;
}

std::vector<class Client>& Channel::getoperList(){
	return this->operlist;
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

bool Channel::checkclientOper(Client* cl) {
	for(size_t i = 0; i < operlist.size(); ++i) {
		if (cl->getfd() == operlist[i].getfd())
			return true;
	}
	return false;
}

void Channel::removeclientOper(Client* cl) {
	for (size_t i = 0; i < operlist.size(); ++i){
		if (cl->getfd() == operlist[i].getfd()){
			operlist.erase(operlist.begin() + i);
		}
	}
}

void Channel::removeClient(Client* cl){
	for (size_t i = 0; i < clientlist.size(); ++i){
		if (cl->getfd() == clientlist[i].getfd()){
			clientlist.erase(clientlist.begin() + i);
		}
	}
	for (size_t i = 0; i < operlist.size(); ++i){
		if (cl->getfd() == operlist[i].getfd()){
			operlist.erase(operlist.begin() + i);
		}
	}
	cl->removeChannel(*this);
}

std::string Channel::getTopic() const {
	return this->topic;
}

void Channel::setTopic(const std::string& top) {
	this->topic = top;
}

void Channel::setinvFlag(bool a){
	this->invFlag = a;
}

void Channel::setkeyFlag(bool a){
	this->keyFlag = a;
}

void Channel::settopicFlag(bool a){
	this->topicFlag = a;
}

void Channel::setclientFlag(bool a){
	this->clientFlag = a;
}

void Channel::setLimit(int i){
	this->limit = i;
}

void Channel::setKey(std::string str){
	this->key = str;
}

void Channel::setClientOper(Client* cl, char c){
	if (cl == NULL){
		std::cout << "Client doesnt exist" << std::endl;
		return ;
	}
	if (c == '-'){
		for (size_t i = 0; i < operlist.size(); ++i){
			if (cl->getfd() == operlist[i].getfd()){
				operlist.erase(operlist.begin() + i);
			}
		}
	}
	else if (c == '+'){
		if (!checkclientOper(cl)){
			operlist.push_back(*cl);
		}
	}
}

void Channel::displayoper(){
	for(size_t i = 0; i < operlist.size(); ++i){
		std::cout << operlist[i].getnName() << std::endl;
	}
}