#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>

#include "Channel.hpp"
#include "Errormsg.hpp"

class Channel;

class Client {
private:
    // Member variables
    int fd; // File descriptor for client
    bool operFlag; // Operator flag for commands
    std::string nName; // Nickname
    std::string uName; // Username
    std::string realName; // Real name
    std::string servName; // Server name
    std::string ipAdd; // IP address
    std::vector<Channel> clientChannelList;
    bool isCapNegotiated;
    bool isPass;
    bool isNick;
    bool isRegistered;

public:
    // Constructors and Destructor
    Client();
    ~Client();
    Client& operator=(const Client& toasgn);

    // Getters
    int getfd() const;
    bool getoperFlag() const;
    std::string getnName() const;
    std::string getuName() const;
    std::string getipAdd() const;
    bool getisCapNegotiated() const;
    bool getisPass() const;
    bool getisNick() const;
    bool getisRegistered() const;
    std::vector<Channel> getlist() const;
    std::vector<Channel> getChannelList() const;

    // Setters
    void setfd(int fd);
    void setoperFlag(bool flag);
    void setnName(const std::string& str);
    void setuName(const std::string& str);
    void setipAdd(const std::string& str);
    void setisCapNegotiated(bool flag);
    void setisPass(bool flag);
    void setisNick(bool flag);
    void setrealName(const std::string& str);
    void setservName(const std::string& str);
    void setisRegistered(bool flag);

    // Channel management
    void addChannel(const Channel& ch);
    void removeChannel(const Channel& ch);

    // Operator overloads
    bool operator==(const Client& tocheck);
};

#endif // CLIENT_HPP
