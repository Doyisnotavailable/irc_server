#include "../includes/Server.hpp"
#include "../includes/Errormsg.hpp"
#include <algorithm>
#include <poll.h>

// Server::stopflag = false;

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

    while (stopflag == false) {
        if (poll(&pollfds[0], pollfds.size(), -1) == -1)
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
}

void Server::receive(int fd) {
	char str[32767];
	std::memset(str, 0, sizeof(str));

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
	int size = line.size();
	if (size == 2 && line[0] == "JOIN") {
		std::cout << "Trying to create a channel" << std::endl;
		addChannel(line[1], cl->getfd());
	} else {
		std::cout << "Size is = " << size << std::endl;
		for (size_t i = 0; i < line.size(); ++i) {
			std::cout << "Index [" << i << "]" << "'" << line[i] << "'" << std::endl;

		}
	}
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


void Server::addChannel(const std::string& chName, int clientfd) {
	// try {
	// 	Channel ch(chName);
	// 	channels.push_back(ch);
	// } catch (std::exception& e) {
	// 	std::cerr << e.what();
	// 	return ;
	// }
	Channel ch(chName);
	ch.addClient(*this->getClient(clientfd));
	channels.push_back(ch);
	displayChannel();
}

Client* Server::getClient(int fd) {
	for (size_t i = 0; i < this->clients.size(); ++i) {
		if (this->clients[i].getfd() == fd)
			return &this->clients[i];
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