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