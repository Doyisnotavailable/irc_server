#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include "Errormsg.hpp"

class Client {
    private:
        int             fd; // file descriptor for client
        bool            operFlag; // operator flag for the commands
        std::string     nName; // nickname
        std::string     uName; // username 
        std::string     ipAdd; // ip address

    public:
        Client();
        ~Client();

        // getter functions //
        int     getfd() const;
        bool    getoperFlag() const;
        std::string getnName() const;
        std::string getuName() const;
        std::string getipAdd() const;

        // ------------------//


};

#endif