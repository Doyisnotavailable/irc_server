#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Errormsg.hpp"

int main(int ac, char **av) {
    if (ac == 3) {
        std::string pass = av[1];
        std::string port = av[2];
        try {
            // catch signals to terminante server
            Server serv(port, pass);

        } catch (std::exception &e) {
            // clean fds
            e.what();
            return 1;
        }
        return 0;
    }
    return 1;
}