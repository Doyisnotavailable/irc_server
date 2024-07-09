#include "../includes/Util.hpp"
#include <iostream>
#include <cctype>
#include <cstdlib>
#include <string>

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::string trimmedStr = trim(str);  // Trim the input string
    std::string::size_type start = 0;
    std::string::size_type end;

    while ((end = trimmedStr.find(delimiter, start)) != std::string::npos) {
        result.push_back(trimmedStr.substr(start, end - start));
        start = end + 1;
    }
    result.push_back(trimmedStr.substr(start));

    return result;
}

std::string ltrim(const std::string &s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(s[start])) {
        ++start;
    }
    return s.substr(start);
}

// Function to trim trailing whitespace
std::string rtrim(const std::string &s) {
    size_t end = s.size();
    while (end > 0 && std::isspace(s[end - 1])) {
        --end;
    }
    return s.substr(0, end);
}

// Function to trim both leading and trailing whitespace
std::string trim(const std::string &s) {
    return ltrim(rtrim(s));
}

bool checkpass(std::string& str){
	if (str.empty())
		return false;
	for (size_t i = 0; i < str.size(); ++i){
		char c = str[i];
		if (!std::isalnum(c) || !std::ispunct(c))
			return false ;
	}
	return true;
}

int parse_port(const char *str) {
    char *endptr;
    double port;

    port = std::strtod(str, &endptr);

    if (*endptr != '\0' || str == endptr) {
        return -1;  /* Invalid input */
    }

    if (port < 1024 || port > 49151) {      // 65535 may want to change this as the upper limit
        return -1;  /* Out of valid range */
    }

    return (int)port;
}


int parse_password(const char *str) {
    if (str == NULL || str[0] == '\0') {
        return -1;
    }

    std::string password(str);
    if (password.size() < 4) {
        return -1;
    }

    for (size_t i = 0; i < password.size(); i++) {
        if (!std::isalnum(password[i]) && !std::ispunct(password[i])) {
            return -1;
        }
    }

    return 0;
}