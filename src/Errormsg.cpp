#include "../includes/Errormsg.hpp"

const char* emptyArg::what() const throw(){
    return "Segmentation fault\r\n";
};

const char* InvalidInput::what() const throw(){
    return "Invalid input!\r\n";
};

const char* SockCreation::what() const throw(){
    return "Socket Creation failed\r\n";
};

const char* InvalidPort::what() const throw(){
    return "Invalid Port range. Port range should be 1024 - 449151 inclusive\r\n";
};

const char* InvalidPassword::what() const throw(){
    return "Invalid Password. Password len must be greater than 4 and contain only alphanumeric and punctuation characters\r\n";
};

const char* Sockaddroption::what() const throw(){
    return "Failed to set Socket address option to reuseable\r\n";
};

const char* Sockfdoption::what() const throw(){
    return "Failed to set fd option to non blocking\r\n";
};

const char* Sockbind::what() const throw(){
    return "Failed to bind socket\r\n";
};

const char* Socklisten::what() const throw(){
    return "Server listen failed\r\n";
};