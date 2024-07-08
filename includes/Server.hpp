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
#include <climits>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include "Util.hpp"

class Server {
    private:
        std::string pass; // pass needed to connect to the server
        int port;
        int serverfd;
        int stopflag;
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
        Client* getClient(const std::string& name);
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
		int checkReceived(std::string str, Client* cl);
		bool isChannel(const std::string& chname);

        void addClienttoChannel(Channel* chName, Client* cl);
		int setChannelLimit(Channel* chName, Client* cl, std::string str);
		void setChannelKey(Channel* chName, Client* cl, std::string str);
		// commands
        void passCMD(int fd, const std::vector<std::string>& vec, bool isCap);
        void nickCMD(int fd, const std::vector<std::string>& vec, bool isCap);
        void userCMD(int fd, const std::vector<std::string>& vec);
		void joinCMD(std::vector<std::string> line, Client* cl);
        void joinChannel(std::string chName, const char* key, Client* cl);
        void joinPass(Channel* chName, const char* key, Client* cl);
		void kickCMD(std::vector<std::string> line, Client* cl);
        void privCMD(std::vector<std::string> line, Client* cl);
        void privCMDsendtoChannel(Channel* ch, Client* cl, std::string tosend);
        void modeCMD(std::vector<std::string> line, Client *cl);
        void pingCMD(std::vector<std::string> line, Client* cl);
        void quitCMD(std::vector<std::string> line, Client* cl);
		void inviteCMD(std::vector<std::string> line, Client* cl);
        // tester utils
		void displayChannel();
		void displayClient();


        // Capability Negotiation and new client registration
        void clAuthentication(int fd, std::vector<std::string>& vec);
        void capCMD(Client* client, std::vector<std::string>& vec, int fd);
        void sendCapabilities(int fd);

        // Utils added
        std::vector<std::string> splitCmd(const std::string& str);
        Client* getClientByFd(int fd);
        ssize_t sendToClient(int fd, const std::string& msg);
        void setClientInfo(int fd);
        void sendWelcome(int fd, Client* client);
        void sendToChannel(Channel& ch, const std::string& msg);
        bool isNickValid(const std::string& nick);
        std::string addStrings(std::vector<std::string> lines, size_t i);

        // Channel commands
        void topicCMD(std::vector<std::string>& vec, Client* cl);
        void eraseClient(Client* cl);

        // int handleNC(int fd, std::string& str);
        // int registerNC(int fd, std::string& str);
};

void sigHandler(int signum);

#endif