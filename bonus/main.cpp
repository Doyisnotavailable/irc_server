#include "BOT.hpp"

// bool censorFlag = false;
bool _censorFlag = false;

int sendToServer(int sockfd, const std::string& message) {
	return send(sockfd, message.c_str(), message.length(), 0);
}

std::string generateRandomNick() {
    std::string nick;
    std::srand(time(0));
    
    for (int i = 0; i < 4; i++) {
        char c = static_cast<char>(std::rand() % 26 + 'A');
        nick += std::toupper(c);
    }
    std::cout << "generated nick = " << nick << std::endl;
    return nick;
}

int main(int ac, char **av) {


	if (ac != 5) {
        std::cerr << "Usage: " << av[0] << " <IP Address> <Port> <Password> <ChannelName" << std::endl;
        return 1;
    }

	const char *ipAddress = av[1];
    int port = atoi(av[2]);
    const char *password = av[3];
	std::string channelName = av[4];

    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    // std::cout << "Socket created successfully" << std::endl;

    // Set socket to non-blocking
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    // Server address structure
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, ipAddress, &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address or address not supported" << std::endl;
        close(sockfd);
        return 1;
    }

    // std::cout << "Address converted successfully" << std::endl;

    // Connect to the server
    int result = connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (result < 0 && errno != EINPROGRESS) {
        std::cerr << "Connection failed immediately" << std::endl;
        close(sockfd);
        return 1;
    }

    // std::cout << "Connecting to the server..." << std::endl;
	usleep(1000000);

    std::cout << "Connected to the server" << std::endl;

    // Set socket back to blocking mode
    fcntl(sockfd, F_SETFL, flags);

    // Send the password
    sendToServer(sockfd, "PASS " + std::string(password) + "\r\n");
	usleep(1000000); 
    std::string nick = generateRandomNick();
    sendToServer(sockfd, "NICK " + nick + "\r\n");
	usleep(1000000);
    sendToServer(sockfd, "USER BOT BOT localhost :BOT\r\n");
	usleep(1000000);


    // Join a channel
    sendToServer(sockfd, "JOIN #" + channelName +  "\r\n");
    usleep(1000000);
    sendToServer(sockfd, "TOPIC #" + channelName + " BOT supervised channel\r\n");

    // Create the bot instance
    BOT bot;
    // Main loop to process incoming messages
    char buffer[1024];
    std::signal(SIGINT, sigHandlerBOT);
	std::signal(SIGQUIT, sigHandlerBOT);

    // set poll for BOT
    struct pollfd pollfds[1];
    pollfds[0].fd = sockfd;
    pollfds[0].events = POLLIN;
    pollfds[0].revents = 0;


    while (_censorFlag == false) {
        if (poll(pollfds, 1, 1000) == -1 && _censorFlag == true) {
            break;
        }
        if (pollfds[0].revents && POLLIN && _censorFlag == false) {
            
            int bytesRead = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead < 0) {
                std::cerr << "Error reading from socket" << std::endl;
                break;
            } else if (bytesRead == 0 || _censorFlag == true) {
                std::cerr << "Server closed connection" << std::endl;
                break;
            } else {
                buffer[bytesRead] = '\0';
                std::string message(buffer);

                // Check if the message contains a swear word
                if (bot.censorMsg(message)) {
                    std::cout << "Swear word detected in message: " << message << std::endl;
                    // Extract the user's nickname and kick them from the channel
                    size_t nickStart = message.find(':') + 1;
                    size_t nickEnd = message.find('!', nickStart);
                    if (nickStart != std::string::npos && nickEnd != std::string::npos) {
                        std::string userNick = message.substr(nickStart, nickEnd - nickStart);
                        std::cout << "Kicking user: " << userNick << std::endl;
                        sendToServer(sockfd, "KICK #" + channelName + " " + userNick + " :Swearing_not_allowed\r\n");
                    }
                }
            }
            std::cout << buffer << std::endl;
        }

    }
    // Close the socket
    close(sockfd);
    return 0;
}
