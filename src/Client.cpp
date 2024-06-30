// #include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Errormsg.hpp"

class Server;

Client::Client() {
	this->fd = -1;
	this->operFlag = false;
	this->nName = "";
	this->uName = "";
	this->ipAdd = "";
	this->isCapNegotiated = false;
	this->isPass = false;
	this->isNick = false;
}

Client::~Client() {
	// close(fd);
}

Client& Client::operator=(const Client& toasgn){
	if (this != &toasgn){
		fd = toasgn.fd;
		operFlag = toasgn.fd;
		nName = toasgn.nName;
		uName = toasgn.uName;
		ipAdd = toasgn.ipAdd;
		clientChannelList = toasgn.clientChannelList;
		isCapNegotiated = toasgn.isCapNegotiated;
		isPass = toasgn.isPass;
		isNick = toasgn.isNick;
	}
	return *this;
}

int Client::getfd() const {
	return this->fd;
}

bool Client::getoperFlag() const {
	return this->operFlag;
}

std::string Client::getnName() const {
	return this->nName;
}

std::string Client::getuName() const {
	return this->uName;
}

std::string Client::getipAdd() const {
	return this->ipAdd;
}

std::vector<class Channel> Client::getlist() const {
	return this->clientChannelList;
}

bool Client::getisCapNegotiated() const {
	return this->isCapNegotiated;
}

bool Client::getisPass() const {
	return this->isPass;
}

bool Client::getisNick() const {
	return this->isNick;
}

void Client::setfd(int fd) {
	this->fd = fd;
}

void Client::setoperFlag(bool flag) {
	this->operFlag = flag;
}

void Client::setnName(const std::string& str) {
	this->nName = str;
}

void Client::setuName(const std::string& str) {
	this->uName = str;
}

void Client::setipAdd(const std::string& str) {
	this->ipAdd = str;
}

void Client::setisCapNegotiated(bool flag) {
	this->isCapNegotiated = flag;
}

void Client::setisPass(bool flag) {
	this->isPass = flag;
}

void Client::setisNick(bool flag) {
	this->isNick = flag;
}

void Client::addChannel(const Channel& ch) {
	this->clientChannelList.push_back(ch);
}

void Client::removeChannel(const Channel& ch){
	for (size_t i = 0; i < clientChannelList.size(); ++i){
		if (ch.getchannelName() == clientChannelList[i].getchannelName())
			clientChannelList.erase(clientChannelList.begin() + i);
	}
}

bool Client::operator==(const Client& tocheck) {
	return this->getfd() == tocheck.getfd();
}

std::vector<class Channel> Client::getChannelList() const {
	return this->clientChannelList;
}