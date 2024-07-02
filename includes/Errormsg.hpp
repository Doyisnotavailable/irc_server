#ifndef ERRORMSG_HPP
#define ERRORMSG_HPP

#include <exception>

//////////////////////////////////////
////MACROS FOR RPL CODES AND ERRORS///
//////////////////////////////////////

/** Client Registration RPLs **/
#define RPL_WELCOME " 001 "
#define RPL_YOURHOST " 002 "
#define RPL_CREATED " 003 "
#define RPL_MYINFO " 004 "
#define RPL_ISUPPORT " 005 "
#define RPL_LUSERCLIENT " 251 "
#define RPL_LUSEROP " 252 "
#define RPL_LUSERUNKNOWN " 253 "
#define RPL_LUSERCHANNELS " 254 "
#define RPL_LUSERME " 255 "
#define RPL_MOTDSTART " 375 "
#define RPL_MOTD " 372 "
#define RPL_ENDOFMOTD " 376 "

/** Channel RPLs **/
#define RPL_NOTOPIC " 331 "
#define RPL_TOPIC "332 "
#define RPL_NAMREPLY "353 "
#define RPL_ENDOFNAMES "366 "


/** ERRORS **/
#define ERR_NOSUCHNICK " 401 "
#define ERR_NOSUCHSERVER " 402 "
#define ERR_NOSUCHCHANNEL " 403 "
#define ERR_CANNOTSENDTOCHAN " 404 "
#define ERR_NOORIGIN " 409 "
#define ERR_UNKNOWNCOMMAND " 421 "
#define ERR_NONICKNAMEGIVEN " 431 "
#define ERR_ERRONEUSNICKNAME " 432 "
#define ERR_NICKNAMEINUSE "433 * "
#define ERR_NOTONCHANNEL " 442 "
#define ERR_NOTREGISTERED " 451 "
#define ERR_NEEDMOREPARAMS " 461 "
#define ERR_ALREADYREGISTRED " 462 "
#define ERR_PASSWDMISMATCH " 464 "
#define ERR_CHANNELISFULL " 471 "
#define ERR_UNKNOWNMODE " 472 "
#define ERR_INVITEONLYCHAN " 473 "
#define ERR_BADCHANNELKEY " 475 "
#define ERR_CHANOPRIVSNEEDED " 482 "
#define ERR_INVALIDKEY " 525 "

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

