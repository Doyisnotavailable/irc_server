#include "../includes/Server.hpp"
#include "../includes/Errormsg.hpp"
#include <algorithm>
#include <poll.h>

// Server::stopflag = false;

Server::Server(std::string port, std::string pass) {
    if (port.empty() || pass.empty())
        throw emptyArg();

    for (size_t i = 0; i < port.length(); i++) {
        if (!std::isdigit(port[i]) || !std::isalnum(pass[i]))
            throw InvalidInput();
    }
    // if (!std::all_of(port.begin(), port.end(), std::isdigit()))
    //     throw InvalidInput();
    // if(!std::all_of(pass.begin(), pass.end(), std::isalnum()))
    //     throw InvalidInput();
    int a = std::atoi(port.c_str()); // PS: Above INT_MAX can cause overflow. Better to use std::strtol
    if (a < 0 || a > 65535)
        throw InvalidPort();
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
                // if (pollfds[i].fd == serverfd)
                    // add client
                // else
                    // get data from client
            }
        }   
    }
}

// void Server::readMessage() {

// }

void Server::addClient() {

}