#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Errormsg.hpp"

Client::Client() {
	this->fd = -1;
	this->operFlag = false;
	this->nName = "";
	this->uName = "";
	this->ipAdd = "";
}

Client::~Client() {
	// close(fd);
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