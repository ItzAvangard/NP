#include <iostream>
#include <map>
#include <vector>
#include <ctime>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>

#define PORT 12345
#define BUFFER_SIZE 1024

std::string current_time() {
    time_t now = time(nullptr);
    char buf[64];
    strftime(buf, sizeof(buf), "%H:%M:%S", localtime(&now));
    return std::string(buf);
}

std::map<std::string, std::string> users = {
    {"user1", "password1"},
    {"user2", "password2"},
    {"admin", "adminpassword"}
};

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr{}, client_addr{};
    socklen_t addr_len = sizeof(client_addr);

    std::map<std::string, sockaddr_in> clients;
    std::map<std::string, std::string> usernames;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    std::cout << "UDP чат-сервер запущен на порту " << PORT << std::endl;

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (n <= 0) continue;
        buffer[n] = '\0';

        std::string client_ip = inet_ntoa(client_addr.sin_addr);
        uint16_t client_port = ntohs(client_addr.sin_port);
        std::string client_id = client_ip + ":" + std::to_string(client_port);

        std::string message(buffer);

        if (usernames.find(client_id) == usernames.end()) {
            std::string login, password;
            std::istringstream ss(message);
            ss >> login >> password;

            if (users.find(login) != users.end() && users[login] == password) {
                usernames[client_id] = login;
                clients[client_id] = client_addr;
                std::string welcome = "[Сервер] Привет, " + login + "! Ты успешно авторизовался.";
                sendto(sockfd, welcome.c_str(), welcome.size(), 0, (struct sockaddr *)&client_addr, addr_len);
            } else {
                std::string error = "[Сервер] Неверный логин или пароль.";
                sendto(sockfd, error.c_str(), error.size(), 0, (struct sockaddr *)&client_addr, addr_len);
            }
            continue;
        }

        std::string username = usernames[client_id];
        std::string time_str = current_time();

        if (message.substr(0, 4) == "/pm ") {
            std::string recipient = message.substr(4, message.find(' ', 4) - 4);
            std::string private_message = message.substr(message.find(' ', 4) + 1);

            bool found = false;
            for (const auto &pair : clients) {
                if (usernames[pair.first] == recipient) {
                    sendto(sockfd, private_message.c_str(), private_message.size(), 0,
                           (const struct sockaddr *)&pair.second, sizeof(pair.second));
                    found = true;
                    break;
                }
            }
            if (!found) {
                std::string error_msg = "[Сервер] Пользователь " + recipient + " не найден.";
                sendto(sockfd, error_msg.c_str(), error_msg.size(), 0, (const struct sockaddr *)&client_addr, addr_len);
            }
            continue;
        }

        std::string formatted = "[" + time_str + "] " + username + ": " + message;
        std::cout << "Получено: " << formatted << std::endl;

        for (const auto &pair : clients) {
            if (pair.first != client_id) {
                sendto(sockfd, formatted.c_str(), formatted.size(), 0,
                       (const struct sockaddr *)&pair.second, sizeof(pair.second));
            }
        }
    }

    close(sockfd);
    return 0;
}