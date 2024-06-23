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
	}
    this->pass = pass;
    this->port = a;
    this->stopflag = false;

    initserverSock();
    startServer();
}

Server::~Server() {
    // close(serverfd); // close all fds
	for (size_t i = 0; i < pollfds.size(); ++i){
		std::cout << "closing port " << pollfds[i].fd << std::endl;
		close(pollfds[i].fd);
	}
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

		std::vector<std::string> vec = splitCmd(str);

		if (*vec.begin() == "CAP" || *vec.begin() == "PASS" || *vec.begin() == "NICK"
			|| *vec.begin() == "USER" || *vec.begin() == "PING" || *vec.begin() == "QUIT") {
			if (!handleCommand(fd, vec)) // handle the commands(PASS, NICK, USER, CAP, PING, QUIT).
				return ;
			}
		else
			checkReceived(line, getClient(fd));

		Client *client = getClient(fd);
		if (client->getisCapNegotiated() == true || client->getnName().empty() || client->getuName().empty())
			return;
		else
			sendWelcome(fd, client); // Send welcome message to the client(Neccessary for the client to start the connection.)

		displayChannel();
	}
}

// Sends welcome messages and server details to the newly connected client. Crucial for the client to start the connection.
void Server::sendWelcome(int fd, Client* client) {

	sendToClient(fd, ":" + client->getuName() + " 001 " + client->getnName() + " :Welcome to the IRC_M2 Network, " + client->getnName() + "\r\n"); // 001 RPL_WELCOME
    sendToClient(fd, ":" + client->getuName() + " 002 " + client->getnName() + " :Your host is IRC_M2, running version APEX.1.0\r\n"); // 002 RPL_YOURHOST
    sendToClient(fd, ":" + client->getuName() + " 003 " + client->getnName() + " :This server was created by Martin and Miguel\r\n"); // 003 RPL_CREATED
    sendToClient(fd, ":" + client->getuName() + " 004 " + client->getnName() + " IRC_M2 APEX.1.0 io ovimnqpsrtklbf :This server supports multiple modes\r\n"); // 004 RPL_MYINFO
    sendToClient(fd, ":" + client->getuName() + " 005 " + client->getnName() + " CHANTYPES=#& PREFIX=(ov)@+ NETWORK=IRC_M2 :are supported by this server\r\n"); // 005 RPL_ISUPPORT

    sendToClient(fd, ":" + client->getuName() + " 251 " + client->getnName() + " :There are 10 users and 3 services on 1 server\r\n"); // 251 RPL_LUSERCLIENT
    sendToClient(fd, ":" + client->getuName() + " 252 " + client->getnName() + " 2 :operator(s) online\r\n"); // 252 RPL_LUSEROP
    sendToClient(fd, ":" + client->getuName() + " 253 " + client->getnName() + " 1 :unknown connection(s)\r\n"); // 253 RPL_LUSERUNKNOWN
    sendToClient(fd, ":" + client->getuName() + " 254 " + client->getnName() + " 5 :channels formed\r\n"); // 254 RPL_LUSERCHANNELS
    sendToClient(fd, ":" + client->getuName() + " 255 " + client->getnName() + " :I have 10 clients and 1 servers\r\n"); // 255 RPL_LUSERME

    sendToClient(fd, ":" + client->getuName() + " 375 " + client->getnName() + " :- " + client->getuName() + " Message of the Day -\r\n"); // 375 RPL_MOTDSTART
    sendToClient(fd, ":" + client->getuName() + " 372 " + client->getnName() + " :- Welcome to the best IRC server!\r\n"); // 372 RPL_MOTD
    sendToClient(fd, ":" + client->getuName() + " 372 " + client->getnName() + " :- Enjoy your stay!\r\n"); // 372 RPL_MOTD
    sendToClient(fd, ":" + client->getuName() + " 376 " + client->getnName() + " :End of /MOTD command.\r\n"); // 376 RPL_ENDOFMOTD

	client->setisCapNegotiated(true);
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
	// {
	// std::stringstream ss;
	// ss << clientfd;
	// std::string nick = "nickc" + ss.str();
	// std::string user = "userc" + ss.str();

	// client.setnName(nick);
	// client.setuName(user);
	// }

	clients.push_back(client); //-> add the client to the vector of clients
	pollfds.push_back(NewPoll); //-> add the client socket to the pollfd
	// displayClient();
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
	// for (size_t i = 0; i < channels.size(); ++i) {
	// 	std::cout << "channel name : " << channels[i].getchannelName() << std::endl;
	// 	for (size_t i = 0; i < channels[i].clientlist.size(); ++i) {
	// 		std::cout << "Client user name: " << channels[i].clientlist[i].getuName() << std::endl;
	// 	}
	// }
	if (channels.size() < 1)
		return;
	std::vector<class Client> tmp = channels[0].getclientList();
	for (size_t i = 0; i < tmp.size(); ++i){
		std::cout << "Client user name: " << tmp[i].getuName() << std::endl;
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
	if (line.size() < 3){
		std::cout << "KICK command not enough param" << std::endl;
		return ;
	}
	if(!isChannel(line[1])){
		std::cout << "Channel doesnt exist" << std::endl;
		return ;
	}
	Channel* tmpch = getChannel(line[1]);
	Client* removeCl = getClient(line[2]);
	//have to check if cl is a channel operator or operator
	if (!removeCl){
		std::cout << "Client not found" << std::endl;
		return ;
	}
	if (tmpch->checkclientExist(cl) && tmpch->checkclientOper(cl))  {
		if (tmpch->checkclientExist(removeCl)){
			tmpch->removeClient(removeCl);
			std::cout << "KICK message on channel " << tmpch->getchannelName() << " from " << cl->getuName() << " to remove " << removeCl->getuName() << " from channel" << std::endl;
		}
	} else
		std::cout << "Client not in channel or not operator" << std::endl;
}

void sigma(int signum) {
	(void)signum;
	stopfl = true;
}


//  NEW FUNCTIONS ADDED //

void Server::sendCapabilities(int fd) {

    std::vector<std::string> serverCapabilities;
    serverCapabilities.push_back("TLS");
    serverCapabilities.push_back("UTF8_ONLY");
    serverCapabilities.push_back("CHANNEL_MODES");
	serverCapabilities.push_back("multi-prefix server-time");

    std::string capabilityList = "CAP * LS :";
    for (std::vector<std::string>::const_iterator it = serverCapabilities.begin(); it != serverCapabilities.end(); ++it) {
        capabilityList += *it + " ";
    }

    capabilityList += "\r\n";
	// std::cout << "capabilityList = " << capabilityList << std::endl;
	// std::cout << "size = " << capabilityList.length() << std::endl;
    ssize_t sentBytes = send(fd, capabilityList.c_str(), capabilityList.length(), 0);
    if (sentBytes == -1) {
        // Handle send error
        std::cerr << "Error sending capabilities to client [" << fd << "]" << std::endl;
        removeClient(fd);
        close(fd);
    } else if (sentBytes < (ssize_t)capabilityList.length()) {
        // Handle partial send
        std::cerr << "Partial send of capabilities to client [" << fd << "]" << std::endl;
    } else {
		std::cout << "sentBytes = " << sentBytes << std::endl;
        std::cout << "Sent capabilities to client [" << fd << "]" << std::endl;
    }
}


int Server::handleCommand(int fd, std::vector<std::string>& vec) {
	Client *client = getClientByFd(fd);

	if (vec.empty()) {
		return 0;
	}
	for (size_t i = 0; i < vec.size(); i++) {
		if (vec[0] == "PASS" || vec[0] == "pass") 
			handlePass(fd, vec);
		else if (vec[0] == "NICK" || vec[0] == "nick") 
			handleNick(fd, vec);
		else if (vec[0] == "USER" || vec[0] == "user") 
			handleUser(fd, vec);
		else if (vec[0] == "PING") 
			sendToClient(fd,  "PONG " + vec[1] + "\"\r\n");
		else if (vec[0] == "QUIT") {
			std::cout << "Client [" << getClientByFd(fd)->getnName() << "] has quit" << std::endl;
			return (removeClient(fd), close(fd), 0);
		} else if (vec[0] == "CAP" || vec[0] == "cap")
			doCAP(client, vec, fd);

		vec.erase(vec.begin());
		if (vec.empty()) {
			break;
		}
		vec.erase(vec.begin());
	}
	return 1;
}

// Handle the CAP command from the client. 
void Server::doCAP(Client* client, std::vector<std::string>& vec, int fd) {
	if (vec[1] == "LS" || vec[1] == "ls") {
		if (client->getisCapNegotiated() == true) {
			return ;
		}
		setClientInfo(fd); // set the client info. Send the capabilities to the client.
	} else if (vec[1] == "REQ" || vec[1] == "req") {
		std::string capResponse = "CAP * ACK :" + vec[2] + "\r\n";
		sendToClient(fd, capResponse);
	} else if (vec[1] == "END" || vec[1] == "end") {
		
		std::string capResponse = "CAP * ACK :" + vec[2] + "\r\n";
		sendToClient(fd, capResponse);		

	} else {
		std::cerr << "Unknown CAP command received from client [" << fd << "]: " << vec[1] << std::endl;
	}
}

// Handle the PASS command from the client. Remove the client if the password is incorrect.
void Server::handlePass(int fd, const std::vector<std::string>& vec) {

	if (vec.size() < 2) {
		std::cerr << "Invalid PASS command format from client [" << fd << "]" << std::endl;
		removeClient(fd);
		close(fd);
		return;
	}

	if (vec[1] == this->pass) {
		std::cout << "Password is correct for client [" << fd << "]" << std::endl;
	} else {
		std::cerr << "Password is incorrect for client [" << fd << "]" << std::endl;
		std::cout << "Password entered = " << vec[1] << " and password = " << this->pass << std::endl;
		sendToClient(fd, "ERROR :Invalid password\r\n");
		removeClient(fd);
		close(fd);
	}
}

// Handle the NICK command from the client. Set the nickname for the client.
// Send an error message if the nickname is already in use. irssi out automatically set the nickname to a different but similar one.
void Server::handleNick(int fd, const std::vector<std::string>& vec) {

	if (vec.size() < 2) {
		sendToClient(fd, "431 * :No nickname given\r\n");
		std::cerr << "######## Invalid NICK command format from client [" << fd << "]" << std::endl;
		return;
	}

	if (!vec[1].empty()) {
		Client *client = getClientByFd(fd);

		for (size_t i = 0; i < clients.size(); i++) {
			if (clients[i].getnName() == vec[1]) {
				sendToClient(fd, "433 * " + vec[1] + " :Nickname is already in use\r\n");
				std::cout << "Error: Nickname already exists" << std::endl;
				return ;
			}
		}
		std::string oldNick = client->getnName();
		client->setnName(vec[1]);
		sendToClient(fd, ":" + oldNick + "!" + getClientByFd(fd)->getuName() + "@" + getClientByFd(fd)->getipAdd() + " NICK " + vec[1] + "\n");
		std::cout << "Nickname set to [" << vec[1] << "] for client [" << fd << "]" << std::endl;
	} else {
		sendToClient(fd, "431 * :No nickname given\r\n");
		std::cerr << "Invalid NICK command format from client [" << fd << "]" << std::endl;
	}
}


// Handle the USER command from the client. Set the username for the client.
void Server::handleUser(int fd, const std::vector<std::string>& vec) {

	if (vec.size() < 5) {
		sendToClient(fd, "461 * USER :Not enough parameters\r\n");
		std::cerr << "Invalid USER command format from client [" << fd << "]" << std::endl;
		return;
	}

	if (!vec[1].empty()) {
		Client *client = getClientByFd(fd);
		
			client->setuName(vec[1]);
			std::cout << "Username set to [" << vec[1] << "] for client [" << fd << "]" << std::endl;
			sendToClient(fd, "You're username has been set to " + vec[1] + "\r\n");
	}
	else {
		sendToClient(fd, "461 * USER :Not enough parameters\r\n");
		std::cerr << "Invalid USER command format from client [" << fd << "]" << std::endl;
	}
}


// Send a message to the client. Return the number of bytes sent.
ssize_t Server::sendToClient(int fd, const std::string& msg) {
	ssize_t sentBytes = send(fd, msg.c_str(), msg.length(), 0);
	if (sentBytes == -1) {
		std::cerr << "Error sending message to client [" << fd << "]" << std::endl;
	} else if (sentBytes < (ssize_t)msg.length()) {
		std::cerr << "Partial send of message to client [" << fd << "]" << std::endl;
	} else {
		// std::cout << "Sent message to client [" << fd << "]" << std::endl;
	}
	return sentBytes;
}

// Split the command string into a vector of strings. Clears empty strings and returns the vector.
std::vector<std::string> Server::splitCmd(const std::string& str) {
	std::vector<std::string> vec;
	std::string tmp;
	for (size_t i = 0; i < str.length(); i++) {
		// if (str[i] == '\n' || str[i] == '\r')
		// 	break;
		if (str[i] == ' ' || str[i] == '\n' || str[i] == '\r') {
			vec.push_back(tmp);
			tmp.clear();
		} else {
			tmp += str[i];
		}
	}
	if (!tmp.empty())
		vec.push_back(tmp);

	// Remove empty strings from the vector.
	for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end();) {
		if (it->empty()	|| *it == "\r" || *it == "\n") {
			it = vec.erase(it); // erase returns the next valid iterator
		} else {
			++it; // move to the next element
		}
	}

	return vec;
}


// Get the client object using the file descriptor.
Client* Server::getClientByFd(int fd) {
	for (size_t i = 0; i < clients.size(); i++) {
		if (clients[i].getfd() == fd) {
			return &clients[i];
		}
	}
	return NULL;
}

// Set the client information. Send the capabilities to the client.
void Server::setClientInfo(int fd) {
	Client client;

	for (size_t i = 0; i < clients.size(); i++) {
		if (clients[i].getfd() == fd) {
			client = clients[i];
			sendCapabilities(fd); // Send the capabilities to the client.
		}
	}
}
