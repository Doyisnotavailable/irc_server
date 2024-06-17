#ifndef ERRORMSG_HPP
#define ERRORMSG_HPP

#include <exception>

class emptyArg: public std::exception{
    virtual const char* what() const throw();
};

class InvalidInput: public std::exception{
    virtual const char* what() const throw();
};

class InvalidPort: public std::exception{
    virtual const char* what() const throw();
};

class SockCreation: public std::exception{
    virtual const char* what() const throw();
};

class Sockaddroption: public std::exception{
    virtual const char* what() const throw();
};

class Sockfdoption: public std::exception{
    virtual const char* what() const throw();
};

class Sockbind: public std::exception{
    virtual const char* what() const throw();
};

class Socklisten: public std::exception{
    virtual const char* what() const throw();
};

class paulRunner: public std::exception{
    virtual const char* what() const throw();
};

#endif

