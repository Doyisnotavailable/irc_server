#include "../includes/Server.hpp"
// #include "../includes/Client.hpp"
#include "../includes/Errormsg.hpp"


int main(int ac, char **av) {
    if (ac == 3) {
        std::string pass = av[2];
        std::string port = av[1];
        try {
            Server serv(port, pass);

        } catch (std::exception &e) {
            std::cerr << e.what();
            return 1;
        }
        return 0;
	} else
        std::cerr << "Erorr: Usage: ./ircserv <port> <password>\n";
    return 1;
}