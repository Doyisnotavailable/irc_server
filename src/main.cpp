#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Errormsg.hpp"

int main(int ac, char **av) {
    if (ac == 3) {
        std::string pass = av[2];
        std::string port = av[1];
        try {
            // catch signals to terminante server
			// std::cout << "declaring server" << std::endl;
			// std::cout << "pass = '" << pass << "'" << std::endl;
			// std::cout << "port = '" << port << "'" << std::endl;
            Server serv(port, pass);

        } catch (std::exception &e) {
            // clean fds
            std::cerr << e.what();
            return 1;
        }
        return 0;
	}
	std::cout << "Nice input" << std::endl;
    return 1;
}