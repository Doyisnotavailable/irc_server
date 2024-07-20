#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <string>
#include <vector>

#include "Client.hpp"

class Client;
class Server;

class Channel {
private:
    // Member variables
    std::string channelName;
    std::vector<Client> clientlist;
    std::vector<Client> operlist;
    std::vector<Client> invlist;
    std::string key;
    std::string topic;
    bool invFlag;   // Flag for invite-only channel
    bool keyFlag;   // Flag for channel password
    bool topicFlag; // Flag for topic operator
    bool clientFlag; // Flag for user limit
    int limit;

public:
    // Constructors and Destructors
    Channel();
    ~Channel();
    Channel(const std::string& chName);
    Channel(const std::string& chName, Client& cl);
    Channel& operator=(const Channel& toasgn);

    // Client management
    void addClient(Client& client);
    void invClient(Client* cl);
    void removeClient(Client* cl);
    void removeInvite(Client* cl);
    void cleaninvList(); // Used to clean invlist when changing flags
    void removeclientOper(Client* cl);

    // Getters
    const std::string getKey() const;
    std::string getchannelName() const;
    Client* getClient(const std::string& nName);
    bool getinvFlag();
    bool getkeyFlag();
    bool gettopicFlag();
    bool getclientFlag();
    int getclientSize();
    int getlimit();
    std::string getMode();
    std::vector<Client> getclientList();
    std::vector<Client>& getoperList();
    std::string getTopic() const;

    // Setters
    void setinvFlag(bool a);
    void setkeyFlag(bool a);
    void settopicFlag(bool a);
    void setclientFlag(bool a);
    void setClientOper(Client* cl, char c);
    void setLimit(int i);
    void setKey(std::string str);
    void setTopic(const std::string& top);
    bool setinvFlag(char c);
    bool settopicFlag(char c);

    // Utility functions
    bool checkinvClient(Client* cl);
    bool checkclientExist(Client* cl);
    bool checkclientOper(Client* cl);
    bool joinFlags(); // Function to check if any of the flags is true when joining
    void displayoper();
};

#endif // CHANNEL_HPP
