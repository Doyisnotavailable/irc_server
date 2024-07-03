#include "../includes/Server.hpp"
// #include "../includes/Errormsg.hpp"
#include <algorithm>
#include <poll.h>
#include <cstring>

bool stopflag = false;
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

	std::signal(SIGINT, sigHandler);
    while (::stopflag == false) {
        if (poll(&pollfds[0], pollfds.size(), -1) == -1 && stopflag == true)
            throw::InvalidInput();
        for (size_t i = 0; i < pollfds.size(); i++) {
            if (pollfds[i].revents && POLLIN && !stopflag) {
                if (pollfds[i].fd == serverfd)
                    Server::addClient();
                else
					Server::receive(pollfds[i].fd);
            }
        }   
    }
}

void Server::eraseClient(Client* cl) {
	// Erase client from all channels
	for (size_t i = 0; i < channels.size(); ++i){
		if (channels[i].checkclientExist(cl)){
			channels[i].removeClient(cl);
			channels[i].removeclientOper(cl);
		}
	}
}

void Server::receive(int fd) {
	char str[32767];
	memset(str, 0, sizeof(str));

	size_t size = recv(fd, str, sizeof(str) - 1, 0);

	if (size <= 0) {
		std::cout << "Client " << this->getClient(fd)->getfd() << " disconnected" << std::endl;

		Client *cl = getClient(fd);
		if (cl) {
			eraseClient(cl);
			removeClient(fd);
			close(fd);
		}
	} else {
		// str[size] = '\0';  ######################## THIS NEEDS REVIWING ########################## (commented to resolve a sigfault case when client send /disconnect cmd from irssi)
		std::string line = str;

		std::vector<std::string> vec = splitCmd(str);

		// std::cout << *vec.begin() << std::endl;

		if (vec.empty())
			return ;
		Client *client = getClient(fd);

		if (!client)
			return ;
		if ((*vec.begin() == "CAP" || *vec.begin() == "PASS" || *vec.begin() == "NICK"
			|| *vec.begin() == "USER" || *vec.begin() == "PING" || *vec.begin() == "QUIT")) {
			if (!handleCommand(fd, vec)) // handle the commands(PASS, NICK, USER, CAP, PING, QUIT).
				return ;
			}
		else if (client->getisCapNegotiated() == true && client->getisRegistered() == true)
			checkReceived(line, getClient(fd));
		else {
			sendToClient(fd, "451 * :You have not registered\r\n");
			std::cerr << "Client not registered" << std::endl;
		}
		
		if (client->getisCapNegotiated() == true || client->getnName().empty() || client->getuName().empty())
			return;
		else
			sendWelcome(fd, client); // Send welcome message to the client(Neccessary for the client to start the connection.)
	}
		// displayChannel();
}

// Sends welcome messages and server details to the newly connected client. Crucial for the client to start the connection.
void Server::sendWelcome(int fd, Client* client) {

	sendToClient(fd, ":" + client->getuName() + " 001 " + client->getnName() + " :Welcome to the IRC_M2 Network, " + client->getnName() + "!~" + client->getuName() + "@" + client->getipAdd() + "\r\n"); // 001 RPL_WELCOME
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

void Server::topicCMD(std::vector<std::string>& vec, Client *cl) {

	if (vec.size() == 2) {
		Channel *channel = getChannel(vec[1]);

		if (!channel) {
			sendToClient(cl->getfd(), "403 * " + vec[1] + " :No such channel\r\n");
			std::cerr << "Channel does not exist for client [" << cl->getfd() << "]" << std::endl;
			return ;
		} else {
			sendToClient(cl->getfd(), "332 * " + channel->getchannelName() + " :" + channel->getTopic() + "\r\n");
			return ;
		}
	}

	if (vec.size() < 3) {
		sendToClient(cl->getfd(), "461 * TOPIC :Not enough parameters\r\n");
		std::cerr << "Invalid TOPIC command format from client [" << cl->getfd() << "]" << std::endl;
		return ;
	}

	Channel *channel = getChannel(vec[1]);
	if (!channel) {
		sendToClient(cl->getfd(), "403 * " + vec[1] + " :No such channel\r\n");
		std::cerr << "Channel does not exist for client [" << cl->getfd() << "]" << std::endl;
		return ;
	}

	if (!channel->checkclientExist(cl)) {
		sendToClient(cl->getfd(), "442 * " + vec[1] + " :You're not on that channel\r\n");
		std::cerr << "Client is not in the channel for client [" << cl->getfd() << "]" << std::endl;
		return ;
	}

	std::string newTopic = vec[2];
	for (size_t i = 3; i < vec.size(); i++) {
		newTopic += " " + vec[i];
	}

	channel->setTopic(newTopic);
	for (size_t i = 0; i < channel->getclientList().size(); i++) {
		sendToClient(channel->getclientList()[i].getfd(), ":" + cl->getnName() + " TOPIC " + channel->getchannelName() + " :" + newTopic + "\r\n");
	}

	// sendToClient(cl->getfd(), ":" + cl->getnName() + " TOPIC " + channel->getchannelName() + " :" + newTopic + "\r\n");
	return ;
}

void Server::checkReceived(std::string str, Client* cl) {

	if (cl->getisNick() == false) {
		sendToClient(cl->getfd(), "451 * :You have not registered\r\n");
		return ;
	}
	std::vector<std::string> line = ::split(str, ' ');
	if (line[0].empty())
		return ;
	if (line[0] == "JOIN")
		joinCMD(line, cl);
	else if (line[0] == "KICK")
		kickCMD(line, cl);
	else if (line[0] == "TOPIC")
		topicCMD(line, cl);
	else if (line[0] == "MODE")
		modeCMD(line, cl);
	else if (line[0] == "PRIVMSG")
		privCMD(line, cl);
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
	// client.setuName("irc"); //-> set the user name
	clients.push_back(client); //-> add the client to the vector of clients
	pollfds.push_back(NewPoll); //-> add the client socket to the pollfd
	// displayClient();
}

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

void Server::addChannel(const std::string& chName, Client& cl) {
	
	// dont forget check channel name if its valid.
	Channel ch(chName, cl);

	channels.push_back(ch);
	cl.addChannel(ch);
	// sendToClient(cl.getfd(), "JOIN :" + ch.getchannelName() + "\r\n");
	sendToClient(cl.getfd(), ":" + cl.getnName() + "!~" + cl.getuName() + "@" + cl.getipAdd() + " JOIN :" + ch.getchannelName() + "\r\n");
	sendToClient(cl.getfd(), "332 " + cl.getnName() + " " + chName + " :" + ch.getTopic() + "\r\n");
	sendToClient(cl.getfd(), "353 " + cl.getnName() + " = " + chName + " :@" + cl.getnName() + "\r\n");
	sendToClient(cl.getfd(), "366 " + cl.getnName() + " " + chName + " :End of /NAMES list\r\n");
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
		if (this->clients[i].getnName() == name)
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
	if (channels.size() < 1)
		return;
	std::vector<class Client> tmp = channels[0].getclientList();
	for (size_t i = 0; i < tmp.size(); ++i){
		std::cout << "Client user name: " << tmp[i].getuName() << std::endl;
	}
}

void Server::joinCMD(std::vector<std::string> line, Client* cl) {
	
	// std::cout << "inside join cmd" << std::endl;
	// std::cout << "line size is = " << line.size() << std::endl;
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
				sendToClient(cl->getfd(), ":" + cl->getnName() + "!~" + cl->getuName() + "@" + cl->getipAdd() + " JOIN :" + tmpch->getchannelName() + "\r\n");
				sendToClient(cl->getfd(), "332 " + cl->getnName() + " " + tmpch->getchannelName() + " :" + tmpch->getTopic() + "\r\n");
				
				std::vector<class Client> tmplist = tmpch->getclientList();
				
				std::string clientListStr;
				for (size_t i = 0; i < tmplist.size(); ++i) {
					if (i > 0) {
						clientListStr += " ";
					}
					clientListStr += "@" + tmplist[i].getnName();
				}

				sendToClient(cl->getfd(), "353 " + cl->getnName() + " = " + tmpch->getchannelName() + " :" + clientListStr + "\r\n");
				sendToClient(cl->getfd(), "366 " + cl->getnName() + " " + tmpch->getchannelName() + " :End of /NAMES list\r\n");
				std::cout << "client joined" << std::endl;
			}

			else if (tmpch->getinvFlag() == true) {
				sendToClient(cl->getfd(), "473 " + cl->getnName() + " " + tmpch->getchannelName() + " :Cannot join channel (+i)\r\n");
				std::cout << "Invite only channel client cant join" << std::endl;
			}
			else if (tmpch->getkeyFlag() == true && key == NULL) {
				sendToClient(cl->getfd(), "475 " + cl->getnName() + " " + tmpch->getchannelName() + " :Cannot join channel (+k)\r\n");
				std::cout << "Channel requires key to join" << std::endl;
			}
			else if (tmpch->getkeyFlag() == true && key != NULL) {
				joinPass(tmpch, key, cl);
			} else if (tmpch->getclientFlag() == true && tmpch->getlimit() <= tmpch->getclientSize()) {
				sendToClient(cl->getfd(), "471 " + cl->getnName() + " " + tmpch->getchannelName() + " :Cannot join channel (+l)\r\n");
				std::cout << "Client limit in the channel already reached" << std::endl;
			} else if (tmpch->getclientFlag() == true && tmpch->getlimit() > tmpch->getclientSize()){
				tmpch->addClient(*cl);
				sendToClient(cl->getfd(), ":" + cl->getnName() + "!~" + cl->getuName() + "@" + cl->getipAdd() + " JOIN :" + tmpch->getchannelName() + "\r\n");
				sendToClient(cl->getfd(), "332 " + cl->getnName() + " " + tmpch->getchannelName() + " :" + tmpch->getTopic() + "\r\n");
			}
			} else
				addChannel(chName, *cl); //creates channel if it doesnt exist
	} else {
		sendToClient(cl->getfd(), "403 " + cl->getnName() + " " + chName + " :No such channel\r\n");
		std::cout << "Invalid channel name" << std::endl;
	}
}

void Server::joinPass(Channel* chName, const char* key, Client* cl){
	std::string tmpkey(key);
	if (chName->getclientFlag() == true && chName->getlimit() <= chName->getclientSize()) {
		sendToClient(cl->getfd(), "471 " + cl->getnName() + " " + chName->getchannelName() + " :Cannot join channel (+l)\r\n");
		std::cout << "Client limit in the channel already reached" << std::endl;
		return ;
	}
	if (tmpkey == chName->getKey()) {
		sendToClient(cl->getfd(), "JOIN :" + chName->getchannelName() + "\r\n");
		sendToClient(cl->getfd(), "332 " + cl->getnName() + " " + chName->getchannelName() + " :" + chName->getTopic() + "\r\n");

		std::vector<class Client> tmplist = chName->getclientList();
		std::string clientListStr;
		for (size_t i = 0; i < tmplist.size(); ++i) {
			if (i > 0) {
				clientListStr += " ";
			}
			clientListStr += tmplist[i].getnName();
		}
		sendToClient(cl->getfd(), "353 " + cl->getnName() + " = " + chName->getchannelName() + " :" + clientListStr + "\r\n");
		sendToClient(cl->getfd(), "366 " + cl->getnName() + " " + chName->getchannelName() + " :End of /NAMES list\r\n");
		chName->addClient(*cl);
	} else {
		sendToClient(cl->getfd(), "475 " + cl->getnName() + " " + chName->getchannelName() + " :Cannot join channel (+k)\r\n");
		std::cout << "Wrong channel key" << std::endl;
	}
}

void Server::kickCMD(std::vector<std::string> line, Client* cl){
	if (line.size() < 3){
		sendToClient(cl->getfd(), "KICK :Not enough parameters\r\n");
		std::cout << "KICK command not enough param" << std::endl;
		return ;
	}
	if(!isChannel(line[1])){
		sendToClient(cl->getfd(), "403 " + cl->getnName() + " " + line[1] + " :No such channel\r\n");
		std::cout << "Channel doesnt exist" << std::endl;
		return ;
	}
	Channel* tmpch = getChannel(line[1]);
	Client* removeCl = getClient(line[2]);
	//have to check if cl is a channel operator or operator
	if (!removeCl){
		sendToClient(cl->getfd(), "401 " + cl->getnName() + " " + line[2] + " :No such nick/channel\r\n");
		std::cout << "Client not found" << std::endl;
		return ;
	}
	if (tmpch->checkclientExist(cl) && tmpch->checkclientOper(cl))  {
		if (tmpch->checkclientExist(removeCl)){
			tmpch->removeClient(removeCl);
			sendToClient(removeCl->getfd(), "KICK " + removeCl->getnName() + " " + tmpch->getchannelName() + " :You have been kicked from the channel\r\n");
			std::cout << "KICK message on channel " << tmpch->getchannelName() << " from " << cl->getuName() << " to remove " << removeCl->getuName() << " from channel" << std::endl;
		}
	} else {
		sendToClient(cl->getfd(), "482 " + cl->getnName() + " " + tmpch->getchannelName() + " :You're not channel operator\r\n");
		std::cout << "Client not in channel or not operator" << std::endl;
	}
}

void Server::privCMD(std::vector<std::string> line, Client* cl){
	//PRIVMSG target,target :msgtosend
	//line[0]      line[1]      line[2 till the end];

	if (line.size() > 2) {
		const char* tmp = line[2].c_str();
		if (tmp[0] != ':') {

			std::cerr << "Invalid msg param" << std::endl;
			return ;
		}
		std::vector<std::string> target = ::split(line[1], ',');
		std::string tosend = addStrings(line, 2);
		for (size_t i = 0; i < target.size(); ++i){
			tmp = target[i].c_str();
			if (tmp[0] == '#'){
				Channel *tmpch = getChannel(target[i]);
				if (tmpch->checkclientExist(cl))
					privCMDsendtoChannel(tmpch, cl, tosend);
				else {
					sendToClient(cl->getfd(), "442 * " + tmpch->getchannelName() + " :Cannot send to channel\r\n");
					// sendToClient(cl->getfd(), cl->getnName() + " " + tmpch->getchannelName() + " :Cannot send to channel\r\n");
				}
				continue ;
			}
			Client* receiver = getClient(target[i]);
			if (receiver == NULL || cl == NULL)
				continue ;
			sendToClient(receiver->getfd(), cl->getnName() + " PRIVMSG " + receiver->getnName() + " " + tosend);
		}
	} else {
		sendToClient(cl->getfd(), "461 * PRIVMSG :Not enough parameters\r\n");
		std::cout << "Invalid use of priv" << std::endl;
	}
}

/* :Angel PRIVMSG Wiz :Hello are you receiving this message ?
    message output format supposed to be
	clientname PRIVMSG targetname :message
	*/

void Server::privCMDsendtoChannel(Channel* ch, Client* cl, std::string tosend){
	if (ch == NULL || cl == NULL)
		return ;
	std::vector<class Client> tmplist = ch->getclientList();
	for (size_t i = 0; i < tmplist.size(); ++i){
		if (cl->getfd() == tmplist[i].getfd())
			continue ;
		sendToClient(tmplist[i].getfd(), ":" + cl->getnName() + "!" + cl->getnName() + "@" + cl->getipAdd() + " PRIVMSG " + ch->getchannelName() + " " + tosend);
	}
}

void Server::modeCMD(std::vector<std::string> line, Client* cl){
	// line[0] = MODE line[1] = CHANNELname line[2] = modestring line[3]... = param

	// We are only requered to implement channel modes.
	// if Mode is called for non channel (on client), return.
	if (line[1][0] != '#') {
		std::cout << std::endl << "MODE CALLED FOR USER NOT CHANNEL" << std::endl;
		return ;
	}

	if (line.size() >= 3){
		Channel* ch = getChannel(line[1]);
		if (ch == NULL){
			sendToClient(cl->getfd(), "403 * " + line[1] + " :No such channel\r\n");
			// sendToClient(cl->getfd(), cl->getuName() + " " + line[1] + " :No such channel\r\n");
			return ;
		}
		if (!ch->checkclientOper(cl)){
			sendToClient(cl->getfd(), "482 * " + ch->getchannelName() + " :You're not channel operator\r\n");
			// sendToClient(cl->getfd(), cl->getuName() + " " + ch->getchannelName() + " :You're not channel operator");
			return ;
		}
		const char* modestring = line[2].c_str();
		char c = '\0';
		size_t param = 3;
		if (modestring[0] == '\0') {
			sendToClient(cl->getfd(), "324 * " + ch->getchannelName() + " :Channel modes are " + " " + "\r\n");
			return ;
		}
		if (modestring[0] != '+' && modestring[0] != '-'){
			sendToClient(cl->getfd(), "472 * " + ch->getchannelName() + " :Unknown MODE flag\r\n");
			return ;
		}
		for (size_t i = 0; modestring[i]; ++i){
			if (modestring[i] == '-' || modestring[i] == '+')
				c = modestring[i];
			if (c == '-'){
				switch(modestring[i]){
					case 'i':
						ch->setinvFlag(false);
						std::cout << "Changed key flag to -" << std::endl; 
						break ;
					case 't':
						ch->settopicFlag(false);
						std::cout << "Changed key flag to -" << std::endl; 
						break ;
					case 'k':
						ch->setkeyFlag(false);
						std::cout << "Changed key flag to -" << std::endl; 
						break ;
					case 'o':
						if (param >= line.size())
							break ;
						ch->setClientOper(getClient(line[param]), modestring[i]);
						param++;
						break ;
					case 'l':
						std::cout << "Changed key flag to -" << std::endl; 
						ch->setclientFlag(false);
						break ;
				}
			} else if (c == '+'){
				switch(modestring[i]){
					case 'i':
						ch->setinvFlag(true);
						std::cout << "Changed key flag to +" << std::endl; 
						break ;
					case 't':
						ch->settopicFlag(true);
						std::cout << "Changed key flag to +" << std::endl; 
						break ;
					case 'k':
						if (param >= line.size())
							break ;
						setChannelKey(ch, cl, line[param]);
						std::cout << "Changed key flag to +" << std::endl; 
						param++;
						break ;
					case 'o':
						if (param >= line.size())
							break ;
						ch->setClientOper(getClient(line[param]), modestring[i]);
						std::cout << "Changed key flag to +" << std::endl; 
						param++;
						break ;
					case 'l':
						if (param >= line.size())
							break ;
						if (!setChannelLimit(ch, cl, line[param]))
							sendToChannel(*ch, "changing limit successful msg");
						else
							sendToClient(cl->getfd(), "fail");
						param++;
						break ;
				}
			}
		}
	}
	else {
		sendToClient(cl->getfd(), "461 * MODE :Not enough parameters\r\n");
		// sendToClient(cl->getfd(), "Invalid use of MODE\r\n");
	}
}

void Server::inviteCMD(std::vector<std::string> line, Client* cl){
	if (line.size() == 3){
		Client* tmp = getClient(line[1]);
		Channel* tmpch = getChannel(line[1]);

		if (!tmpch || !tmp)
			return ;
		if (tmpch->checkclientExist(cl) && tmpch->checkclientOper(cl)){
			if (tmpch->checkclientExist(tmp))
				std::cout << "Client is already in the channel" << std::endl;
			// else we have to join client inside the channel;
		}
	} else
		std::cout << "Invalid invite param" << std::endl;
}

int Server::setChannelLimit(Channel* chName, Client *cl, std::string str){
	char *ptr;
	long num = std::strtol(str.c_str(), &ptr, 10);

	if (num <= 0 || num > INT_MAX) {
		sendToClient(cl->getfd(), "Client limit amount exceeds INT_MAX\r\n");
		return -1;
	}
	else {
		chName->setLimit(num);
		chName->setclientFlag(true);
		return 0;
	}
}

void Server::setChannelKey(Channel* ch, Client* cl, std::string str){
	//check str for alphanumerical and empty.
	// ERR_INVALIDKEY (525) 
	if(!checkpass(str)) {
		sendToClient(cl->getfd(), "525 * :key is not well-formed\r\n");
		return ;
	}
	ch->setKey(str);
	ch->setkeyFlag(true);

}

void Server::pingCMD(std::vector<std::string> line, Client* cl){
	if (line.size() < 2){
		sendToClient(cl->getfd(), "461 * PING :Not enough parameters\r\n");
		return ;
	}
	sendToClient(cl->getfd(), "PONG " + line[1] + "\"\r\n");
}

void Server::quitCMD(std::vector<std::string> line, Client* cl){
	// (void)line;
	int fd = cl->getfd();
	if (cl->getisRegistered() == false) {
		sendToClient(fd, "451 * :You have not registered\r\n");
		std::cerr << "Client [" << fd << "] has not registered" << std::endl;
		return ;
	}
	std::cout << "Client " << cl->getnName() << " has quit" << std::endl;
	if (line.size() > 1) {
		std::cout << "Quit message: " << line[1] << std::endl;
	}
	eraseClient(cl);  // removes client from all its existing channels
	removeClient(fd);
	close(fd);
}

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
		if (vec[0] == "PASS" || vec[0] == "pass") {
			if (!handlePass(fd, vec))
				return 0;
		}
		// if (getClientByFd(fd)->getisPass() == false) {
		// 	sendToClient(fd, "451 * :You have not registered\r\n");
		// 	std::cerr << "Client [" << fd << "] has not registered" << std::endl;
		// 	return 0;
		// }
		else if (vec[0] == "NICK" || vec[0] == "nick") 
			handleNick(fd, vec);
		else if (vec[0] == "USER" || vec[0] == "user") 
			handleUser(fd, vec);
		else if (vec[0] == "PING") 
			pingCMD(vec, client);
		else if (vec[0] == "QUIT") {
			quitCMD(vec, client);
			return 0;
		} else if (vec[0] == "CAP" || vec[0] == "cap")
			capCMD(client, vec, fd);

		vec.erase(vec.begin());
		if (vec.empty()) {
			break;
		}
		vec.erase(vec.begin());
	}
	return 1;
}

// Handle the CAP command from the client. 
void Server::capCMD(Client* client, std::vector<std::string>& vec, int fd) {
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
int Server::handlePass(int fd, const std::vector<std::string>& vec) {

	if (getClientByFd(fd)->getisPass() == true) {
		return 1;
	}

	if (vec.size() < 2 || vec[1].empty()) {
		sendToClient(fd, "461 * PASS :Not enough parameters\r\n");
		std::cerr << "Invalid PASS command format from client [" << fd << "]" << std::endl;
		removeClient(fd);
		close(fd);
		return 0;
	}

	if (vec.size() != 2) {
		if (vec[2] != "NICK") {
			sendToClient(fd, "461 * PASS :Not enough parameters\r\n");
			std::cerr << "Invalid PASS command format from client [" << fd << "]" << std::endl;
			removeClient(fd);
			close(fd);
			return 0;
		}
	}

	if (vec[1] == this->pass) {
		getClientByFd(fd)->setisPass(true);
		std::cout << "Password is correct for client [" << fd << "]" << std::endl;
		return 1;
	} else {
		sendToClient(fd, "464 * :Password incorrect\r\n");
		std::cerr << "Password is incorrect for client [" << fd << "]" << std::endl;
		removeClient(fd);
		close(fd);
		return 0;
	}
}


bool Server::isNickValid(const std::string& nick) {
	if (nick.empty() || nick[0] == '\0') { // check if the nickname is empty
		return false;
	}
	if (nick.length() > 30) { // check if the nickname is too long. The maximum length is 30 characters(There is no official limit, used for testing purposes).
		return false;
	}
	if (nick[0] == '#' || isdigit(nick[0]) || nick[0] == ':') { // check if the nickname starts with a disallowed character (: is OK with irssi)
		return false;
	}
	for (size_t i = 0; i < nick.length(); i++) {
        if (!isalnum(nick[i]) && !strchr("[]{}\\|-_", nick[i])) { // check if the nickname contains disallowed characters
			return false;
		}
    }
	return true;
}

// Handle the NICK command from the client. Set the nickname for the client.
// Send an error message if the nickname is already in use. irssi out automatically set the nickname to a different but similar one.
void Server::handleNick(int fd, const std::vector<std::string>& vec) {

	if (getClientByFd(fd)->getisPass() == false) {
		sendToClient(fd, "451 * :You have not registered\r\n");
		std::cerr << "Client [" << fd << "] has not registered" << std::endl;
		return ;
	}

	if (vec.size() < 2) {
		sendToClient(fd, "431 * :No nickname given\r\n");
		std::cerr << "Invalid NICK command format from client [" << fd << "]" << std::endl;
		return;
	}

	if (vec.size() != 2) {
		if (vec[2] != "USER") {
			sendToClient(fd, "461 * NICK :Not enough parameters\r\n");
			std::cerr << "Invalid NICK command format from client [" << fd << "]" << std::endl;
			return ;
		}
	}

	if (!isNickValid(vec[1])) {
		sendToClient(fd, "432 * " + vec[1] + " :Erroneous Nickname\r\n");
		std::cout << "Error: Invalid nickname" << std::endl;
		return ;
	}

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
	client->setisNick(true);
	// client->setisCapNegotiated(true);
	// sendToClient(fd, oldNick + "changed thier nickname to " + vec[1] + "\r\n");
	sendToClient(fd, ":" + oldNick + "!" + getClientByFd(fd)->getuName() + "@" + getClientByFd(fd)->getipAdd() + " NICK " + vec[1] + "\n");
	std::cout << "Nickname set to [" << vec[1] << "] for client [" << fd << "]" << std::endl;
}


// Handle the USER command from the client. Set the username for the client.
void Server::handleUser(int fd, const std::vector<std::string>& vec) {

	if (getClientByFd(fd)->getisPass() == false || getClientByFd(fd)->getisNick() == false) {
		sendToClient(fd, "451 * :You have not registered\r\n");
		std::cerr << "Client [" << fd << "] has not registered" << std::endl;
		return ;
	}
	
	if (vec.size() < 5) {
		sendToClient(fd, "461 * USER :Not enough parameters\r\n");
		std::cerr << "Invalid USER command format from client [" << fd << "]" << std::endl;
		return;
	}

	if (!vec[1].empty()) {
		Client *client = getClientByFd(fd);
		
		client->setuName(vec[1]);
		client->setisRegistered(true);
		std::cout << "Username set to [" << vec[1] << "] for client [" << fd << "]" << std::endl;
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

void Server::sendToChannel(Channel& ch, const std::string& msg){
	
	if (isChannel(ch.getchannelName())){
		std::vector<class Client> tmp = ch.getclientList();
		for (size_t i = 0; i < tmp.size(); ++i){
			sendToClient(tmp[i].getfd(), msg);
		}
	} else
		std::cout << "Channel doesnt exist" << std::endl;
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

std::string Server::addStrings(std::vector<std::string> lines, size_t i) {
	std::string tmp;
	for (size_t j = i; j < lines.size(); ++j){
		tmp += lines[j];
		tmp += " ";
	}
	tmp += "\r\n";
	return tmp;
}

void sigHandler(int signum) {
	(void)signum;
	::stopflag = true;
	std::cout << "\nServer is shutting down" << std::endl;
}