#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "Channel.hpp"
#include "Errormsg.hpp"
#include <iostream>
#include <string>
#include <sys/socket.h>
// #include <sys/type.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <csignal>
#include <poll.h>
#include <arpa/inet.h>
#include <vector>
#include <map>
#include <unistd.h>
#include <sstream>
#include <ctype.h>
#include "Util.hpp"


class Server {
    private:
        std::string pass; // pass needed to connect to the server
        int port;
        int serverfd;
        bool stopflag;
        std::vector<class Client> clients;  // all of  the clients in server
        std::vector<class Channel> channels; // channels available in all server
        std::vector<struct pollfd> pollfds; // used for i/o of fds
        Server();
    public:
        Server(std::string port, std::string pass);
        ~Server(); 
        
        //getter and setter here
        std::string getpass() const;
        int getport() const;
        int getserverfd() const;
        bool getstopflag() const;
        Client* getClient(int fd); // finding client using its fd
        Channel* getChannel(const std::string& chname);

        // server mem funcs
        void initserverSock();
        void startServer(); // main loop for server is here
        void receive(int fd); // get incoming messages can make receive and send to clients in one function still not sure about that
        void addClient();
		void setClient();
        void addChannel(const std::string& chName, Client& cl);
		void removeClient(int fd);
        void removeClientAllChannels(int fd);
		void checkReceived(std::string str, Client* cl);
		bool isChannel(const std::string& chname);
		// commands
		void joinCMD(std::vector<std::string> line, Client* cl);
		// tester utils
		void displayChannel();
		void displayClient();
};

	void sigma(int signum);
#endif