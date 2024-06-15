#include "Server.hpp"
#include <algorithm>
#include "Errormsg.hpp"

Server::stopflag = false;

Server::Server(std::string port, std::string pass) {
    if (port.empty() || pass.empty())
        std::throw emptyArg();
    if (!std::allof(port.begin(), port.end(), std::isdigit()))
        std::throw InvalidInput();
    if(!std:::allof(pass.begin(), pass.end(), std::isalnum()))
        std::throw InvalidInput();
    int a = std::atoi(port.c_str());
    if (port < 0 || port > 65535)
        std::throw InvalidPort();
    this->pass = pass;
    this->port = a;

    initserverSock();
    startServer();
}

void Server::initserverSock() {
    struct sockaddr_in adr;

    adr.sin_port = htons(this->port);
    add.sin_family = AF_INET;
    adr.sin_addr.s_addr = INADDR_ANY;

    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd == -1)
        std::throw SockCreation();
    int a = 1;
    if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADD, &a, sizeof(a)) == -1)
        std::throw Sockaddroption();
    if (fcntl(serverfd, F_SETFL, O_NONBLOCK) == -1)
        std::throw Sockfdoption();
    if (bind(serverfd, (struct sockaddr *)&add, sizeof(add)) == -1)
        std::throw Sockbind();
    if (listen(serverfd, SOMAXCONN) == -1)
        std::throw Socklisten();
    
    struct pollfd paul;
    paul.fd = serverfd;
    paul.events = POLLIN;
    paul.revents = 0;
    pollfds.pushback(paul);
}

void Server::startServer() {
    std::cout << "Starting Server" << std::endl;

    while (stopflag == false) {
        if (poll(&pollfds[0], pollfds.size(), -1) == -1)
            std::throw paulRunner();

        for (size_t i = 0; i < fds.size(); i++) {
            if (pollfds[i].revents && POLLIN) {
                if (pollfds[i].fd == serverfd)
                    // add client
                else
                    // get data from client
            }
        }   
    }
}

void Server::readMessage() {

}

void Server::addClient() {

}