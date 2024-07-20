#ifndef SERVER_HPP
#define SERVER_HPP

#include <arpa/inet.h>
#include <csignal>
#include <fcntl.h>
#include <map>
#include <netinet/in.h>
#include <poll.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <cctype>
#include <climits>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cstdio>

#include "Channel.hpp"
#include "Client.hpp"
#include "Errormsg.hpp"
#include "Util.hpp"

// Constants
#define MAX_CLIENTS INT_MAX
#define MAX_NICK_LENGTH 12
#define MAX_CHAN_NAME_LEN 20

class Server {
private:
    // Member variables
    std::string pass;             
    int port;
    std::string hostname;
    int serverfd;
    int stopflag;
    std::vector<Client> clients;  // All clients in the server
    std::vector<Channel> channels; // Channels available in the server
    std::vector<struct pollfd> pollfds; // Used for I/O of file descriptors
    std::map<int, std::string> clbuffer;
    int clientCount;

    // Private constructor
    Server();

public:
    // Constructors and Destructors
    Server(std::string port, std::string pass);
    ~Server();

    // Getters
    std::string getpass() const;
    std::string getHostname() const;
    Client* getClient(int fd); // Finding client using its file descriptor
    Client* getClient(const std::string& name);
    Channel* getChannel(const std::string& chname);

    // Server member functions
    void initserverSock();
    void startServer(); // Main loop for server
    void receive(int fd); // Get incoming messages
    void addClient();
    void addChannel(std::string& chName, Client& cl);
    void removeClient(int fd);
    int  checkReceived(std::string str, Client* cl);
    bool isChannel(const std::string& chname);

    // Channel management
    void addclienttoChannel(Channel* chName, Client* cl);
    int setChannelLimit(Channel* chName, Client* cl, std::string str);
    void setChannelKey(Channel* chName, Client* cl, std::string str);

    // Commands
    void passCMD(int fd, const std::vector<std::string>& vec, bool isCap);
    void nickCMD(int fd, const std::vector<std::string>& vec, bool isCap);
    void userCMD(int fd, const std::vector<std::string>& vec);
    void joinCMD(std::vector<std::string> line, Client* cl);
    void topicCMD(std::vector<std::string>& vec, Client* cl);
    void joinChannel(std::string chName, const char* key, Client* cl);
    void joinPass(Channel* chName, const char* key, Client* cl);
    void kickCMD(std::vector<std::string> line, Client* cl);
    void privCMD(std::vector<std::string> line, Client* cl);
    void privCMDsendtoChannel(Channel* ch, Client* cl, std::string tosend);
    void modeCMD(std::vector<std::string> line, Client* cl);
    void pingCMD(std::vector<std::string> line, Client* cl);
    void quitCMD(std::vector<std::string> line, Client* cl);
    void partCMD(std::vector<std::string> line, Client* cl);
    void inviteCMD(std::vector<std::string> line, Client* cl);

    // Capability Negotiation and new client registration
    void clAuthentication(int fd, std::vector<std::string>& vec);
    void capCMD(Client* client, std::vector<std::string>& vec, int fd);
    void sendCapabilities(int fd);

    // Utility functions
    std::vector<std::string> splitCmd(const std::string& str);
    Client* getClientByFd(int fd);
    ssize_t sendToClient(int fd, const std::string& msg);
    void sendWelcome(int fd, Client* client);
    void sendToChannel(Channel& ch, const std::string& msg);
    bool isNickValid(const std::string& nick);
    std::string addStrings(std::vector<std::string> lines, size_t i);
    void eraseClient(Client* cl);
    void partialMessage(char* str, int fd);
};

// Signal handler
void sigHandler(int signum);

#endif // SERVER_HPP
