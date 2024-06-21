#include "../includes/Server.hpp"
// #include "../includes/Errormsg.hpp"
#include <algorithm>
#include <poll.h>
#include <cstring>

bool stopfl = false;
Server::Server(std::string port, std::string pass) {
    if (port.empty() || pass.empty()) {
		throw emptyArg();
	}
    for (size_t i = 0; i < port.length(); i++) {
        if (!std::isdigit(port[i]))
            throw InvalidInput();
    }

	for (size_t i = 0; i < pass.length(); i++) {
        if (!std::isdigit(pass[i]))
            throw InvalidInput();
    }

    // if (!std::all_of(port.begin(), port.end(), std::isdigit()))
    //     throw InvalidInput();
    // if(!std::all_of(pass.begin(), pass.end(), std::isalnum()))
    //     throw InvalidInput();
    int a = std::atoi(port.c_str()); // PS: Above INT_MAX can cause overflow. Better to use std::strtol
	// int a = 6667;
	std::cout << a << std::endl;
    if (a < 0 || a > 65535) {
		std::cout << "invalid port cout" << std::endl;
		throw InvalidPort();
	} else
		std::cout << "here before aasdasdtoi" << std::endl;
	
	std::cout << "here" << std::endl;
    this->pass = pass;
    this->port = a;
    this->stopflag = false;

    initserverSock();
    startServer();
}

Server::~Server() {
    // close(serverfd); // close all fds
}

void Server::initserverSock() {
    struct sockaddr_in adr;

    adr.sin_port = htons(this->port);
    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = INADDR_ANY;

	std::cout << "pass = " << this->pass << std::endl;
	std::cout << "port = " << this->port << std::endl;

    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd == -1)
        throw SockCreation();
    int a = 1;
    if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &a, sizeof(a)) == -1)
        throw Sockaddroption();
    if (fcntl(serverfd, F_SETFL, O_NONBLOCK) == -1)
        throw Sockfdoption();
    if (bind(serverfd, (struct sockaddr *)&adr, sizeof(adr)) == -1)
        throw Sockbind();
    if (listen(serverfd, SOMAXCONN) == -1)
        throw Socklisten();
    
    struct pollfd paul;
    paul.fd = serverfd;
    paul.events = POLLIN;
    paul.revents = 0;
    pollfds.push_back(paul);
}

void Server::startServer() {
    std::cout << "Starting Server" << std::endl;

	std::signal(SIGINT, sigma);
    while (stopfl == false) {
        if (poll(&pollfds[0], pollfds.size(), -1) == -1 && stopfl == false)
            throw InvalidInput();

        for (size_t i = 0; i < pollfds.size(); i++) {
            if (pollfds[i].revents && POLLIN) {
                if (pollfds[i].fd == serverfd)
                    Server::addClient();
                else
					Server::receive(pollfds[i].fd);
            }
        }   
    }
	for (size_t i = 0; i < pollfds.size(); ++i) {
		// if (pollfds[i].fd == serverfd)
		// 	std::cout << "saw serverfd inside pollfds" << std::endl;
		close(pollfds[i].fd);
	}
	// close(serverfd);
}

void Server::receive(int fd) {
	char str[32767];
	memset(str, 0, sizeof(str));

	size_t size = recv(fd, str, sizeof(str) - 1, 0);

	if (size <= 0) {
		std::cout << "Client " << this->getClient(fd)->getfd() << " disconnected" << std::endl;
		removeClient(fd);
		close(fd);
	} else {
		str[size] = '\0';
		std::string line = str;
		checkReceived(line, getClient(fd));
	}
}

void Server::checkReceived(std::string str, Client* cl) {

	std::vector<std::string> line = ::split(str, ' ');
	if (line[0].empty())
		return ;
	if (line[0] == "JOIN")
		joinCMD(line, cl);
	else if (line[0] == "KICK")
		kickCMD(line, cl);
	else if (line[0] == "TOPIC")
		std::cout << "process topic command" << std::endl;
	else if (line[0] == "MODE")
		std::cout << "process mode command" << std::endl;
	else
		std::cout << "wtf are you typing " << cl->getnName() << std::endl;
}

void Server::addClient() {
	Client client;
	struct sockaddr_in cliadd;
	struct pollfd NewPoll;
	socklen_t len = sizeof(cliadd);

	int clientfd = accept(serverfd, (sockaddr *)&(cliadd), &len);
	if (clientfd == -1)
		{std::cout << "accept() failed" << std::endl; return;}

	if (fcntl(clientfd, F_SETFL, O_NONBLOCK) == -1) //-> set the socket option (O_NONBLOCK) for non-blocking socket
		{std::cout << "fcntl() failed" << std::endl; return;}

	NewPoll.fd = clientfd; //-> add the client socket to the pollfd
	NewPoll.events = POLLIN; //-> set the event to POLLIN for reading data
	NewPoll.revents = 0; //-> set the revents to 0

	client.setfd(clientfd); //-> set the client file descriptor
	client.setipAdd(inet_ntoa((cliadd.sin_addr))); //-> convert the ip address to string and set it
	
	// this brackets are for testing purposes
	{
	std::stringstream ss;
	ss << clientfd;
	std::string nick = "nickname test " + ss.str();
	std::string user = "username test " + ss.str();

	client.setnName(nick);
	client.setuName(user);
	}

	clients.push_back(client); //-> add the client to the vector of clients
	pollfds.push_back(NewPoll); //-> add the client socket to the pollfd
	displayClient();
	std::cout << "done" << std::endl;
}

// basic function for removing client
/* cases to handle when removing client 
	remove client in every channels he exists
	if the client is the only one guy left in the channel delete the channel as well
*/ 

void Server::removeClient(int fd) {
	
	for (size_t i = 0; i < clients.size(); ++i) {
		if (clients[i].getfd() == fd) {
			clients.erase(clients.begin() + i);
			break ;
		}
	}
	for (size_t i = 0; i < pollfds.size(); ++i){
		if (pollfds[i].fd == fd){
			pollfds.erase(pollfds.begin() + i);
			break ;
		}
	}


}

// void Server::removeClientAllChannels(int fd) {
// 	std::vector<Channel> tmp = client[]
// }


void Server::addChannel(const std::string& chName, Client& cl) {
	
	// dont forget check channel name if its valid.
	Channel ch(chName, cl);

	channels.push_back(ch);
	cl.addChannel(ch);
	std::cout << "Client " << cl.getuName() << " created a channel " << chName << std::endl; 
}

bool Server::isChannel(const std::string& chname) {
	for (size_t i = 0; i < channels.size(); ++i) {
		if (chname == channels[i].getchannelName())
			return true;
	}
	return false;
}



Client* Server::getClient(int fd) {
	for (size_t i = 0; i < this->clients.size(); ++i) {
		if (this->clients[i].getfd() == fd)
			return &this->clients[i];
	}
	return NULL;
}

Client* Server::getClient(const std::string& name){
	for (size_t i = 0; i < this->clients.size(); ++i){
		if (this->clients[i].getuName() == name)
			return &clients[i];
	}
	return NULL;
}

Channel* Server::getChannel(const std::string& chname) {
	for (size_t i = 0; i < this->channels.size(); ++i){
		if (this->channels[i].getchannelName() == chname)
			return &this->channels[i];
	}
	return NULL;
}

void Server::displayClient() {
	for (size_t i = 0; i < clients.size(); ++i) {
		std::cout << "Client nick name: " << clients[i].getnName() << std::endl;
		std::cout << "Client user name: " << clients[i].getuName() << std::endl;
		std::cout << "Client fd: " << clients[i].getfd() << std::endl;
		std::cout << "Client ip: " << clients[i].getipAdd() << std::endl;
	}
}

void Server::displayChannel() {
	for (size_t i = 0; i < channels.size(); ++i) {
		std::cout << "channel name : " << channels[i].getchannelName() << std::endl;
	}
}

void Server::joinCMD(std::vector<std::string> line, Client* cl) {
	
	std::cout << "inside join cmd" << std::endl;
	std::cout << "line size is = " << line.size() << std::endl;
	//process join join command we have to check in this function contents of the line to determine which constructor show
	if (line.size() == 2) {
		std::vector<std::string> chname = split(line[1], ',');
		for(size_t i = 0; i < chname.size(); ++i){
			joinChannel(chname[i], NULL, cl);
		}
	} else if (line.size() == 3) {
		// this case is for taking keys for channel
		std::vector<std::string> chname = ::split(line[1], ',');
		std::vector<std::string> keys = ::split(line[2], ',');
		for (size_t i = 0; i < chname.size(); ++i){
			if (i < keys.size())
				joinChannel(chname[i], keys[i].c_str(), cl);
			else
				joinChannel(chname[i], NULL, cl);
		}
	}
	else
		std::cout << "Invalid use of command JOIN" << std::endl; //need to check what error to send to client.
}

void Server::joinChannel(std::string chName, const char* key, Client* cl){
	const char *tmp = chName.c_str();
	if (tmp[0] == '#'){
		//check if the channel is existing
		if (isChannel(chName)) {
			Channel *tmpch = getChannel(chName);
			//check flags if its possible to join
			if (tmpch->checkclientExist(cl)){
				std::cout << "Client already at channel" << std::endl;
				return ;
			}
			if (!tmpch->joinFlags()) {
				tmpch->addClient(*cl);
				std::cout << "client joined" << std::endl;
			}
			else if (tmpch->getinvFlag() == true)
				std::cout << "Invite only channel client cant join" << std::endl;
			else if (tmpch->getkeyFlag() == true && key == NULL)
				std::cout << "Channel requires key to join" << std::endl;
			else if (tmpch->getkeyFlag() == true && key != NULL) {
				joinPass(tmpch, key, cl);
			} else if (tmpch->getclientFlag() == true && tmpch->getlimit() <= tmpch->getclientSize())
				std::cout << "Client limit in the channel already reached" << std::endl;
			} else
				addChannel(chName, *cl); //creates channel if it doesnt exist
	} else
		std::cout << "Invalid channel name" << std::endl;
}

void Server::joinPass(Channel* chName, const char* key, Client* cl){
	std::string tmpkey(key);
	if (chName->getclientFlag() == true && chName->getlimit() <= chName->getclientSize()){
		std::cout << "Client limit in the channel already reached" << std::endl;
		return ;
	}
	if (tmpkey == chName->getKey()) {
		chName->addClient(*cl);
	} else
		std::cout << "Wrong channel key" << std::endl;
}

void Server::kickCMD(std::vector<std::string> line, Client* cl){
	if(!isChannel(line[1])){
		std::cout << "Channel doesnt exit" << std::endl;
		return ;
	}
	Channel* tmpch = getChannel(line[1]);
	Client* removeCl = getClient(line[2]);
	//have to check if cl is a channel operator or operator
	if (!removeCl){
		std::cout << "Client not found" << std::endl;
		return ;
	}
	if (tmpch->checkclientExist(removeCl)){
		tmpch->removeClient(removeCl);
		std::cout << "KICK message on channel " << tmpch->getchannelName() << " from " << cl->getuName() << " to remove " << removeCl->getuName() << " from channel" << std::endl;
	}
}

void sigma(int signum) {
	(void)signum;
	stopfl = true;
}