#include "../includes/Util.hpp"

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
