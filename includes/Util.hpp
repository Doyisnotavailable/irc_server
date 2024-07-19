#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

std::vector<std::string> split(const std::string& str, char delimiter);
// std::string ltrim(const std::string &s);
// std::string rtrim(const std::string &s);
std::string trim(const std::string &s);
bool checkpass(std::string& pass);
int parse_port(const char *str);
int parse_password(const char *str);
std::string intToString(int number);
bool check_ChanName(std::string& name);


#endif