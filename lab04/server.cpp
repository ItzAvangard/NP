#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    std::vector<int> client_sockets;
    fd_set readfds;
    int max_sd;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = 0;

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    socklen_t len = sizeof(address);
    getsockname(server_fd, (struct sockaddr *)&address, &len);
    std::cout << "Сервер запущен на порту: " << ntohs(address.sin_port) << std::endl;

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Ожидание подключений..." << std::endl;

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        for (size_t i = 0; i < client_sockets.size(); i++) {
            int sd = client_sockets[i];
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) {
            std::cout << "Ошибка select" << std::endl;
        }

        if (FD_ISSET(server_fd, &readfds)) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            std::cout << "Новое подключение, сокет fd: " << new_socket 
                      << ", IP: " << inet_ntoa(address.sin_addr) 
                      << ", порт: " << ntohs(address.sin_port) << std::endl;

            client_sockets.push_back(new_socket);
        }

        for (size_t i = 0; i < client_sockets.size(); i++) {
            int sd = client_sockets[i];
            if (FD_ISSET(sd, &readfds)) {
                int valread = read(sd, buffer, BUFFER_SIZE);
                if (valread == 0) {
                    getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                    std::cout << "Клиент отключился, IP: " << inet_ntoa(address.sin_addr) 
                              << ", порт: " << ntohs(address.sin_port) << std::endl;
                    close(sd);
                    client_sockets.erase(client_sockets.begin() + i);
                } else {
                    buffer[valread] = '\0';
                    std::cout << "Получено от клиента " << sd << ": " << buffer << std::endl;
                }
            }
        }
    }

    return 0;
} 