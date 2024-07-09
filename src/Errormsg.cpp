#include "../includes/Errormsg.hpp"

const char* emptyArg::what() const throw(){
    return "Segmentation fault\n";
};

const char* InvalidInput::what() const throw(){
    return "Invalid input!\n";
};

const char* SockCreation::what() const throw(){
    return "Socket Creation failed\n";
};

const char* InvalidPort::what() const throw(){
    return "Invalid Port range\n";
};

const char* InvalidPassword::what() const throw(){
    return "Invalid Password\n";
};

const char* Sockaddroption::what() const throw(){
    return "Failed to set Socket address option to reuseable\n";
};

const char* Sockfdoption::what() const throw(){
    return "Failed to set fd option to non blocking\n";
};

const char* Sockbind::what() const throw(){
    return "Failed to bind socket\n";
};

const char* Socklisten::what() const throw(){
    return "Server listen failed\n";
};