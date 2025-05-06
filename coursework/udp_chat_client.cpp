#include <iostream>
#include <thread>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>

#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

void receive_messages(int sockfd) {
    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        recv(sockfd, buffer, BUFFER_SIZE, 0);
        std::string msg(buffer);
        std::cout << msg << std::endl;

        if (msg.find("[Сервер] Перейдите на TCP для передачи файла:") != std::string::npos) {
            std::istringstream iss(msg);
            std::string skip, skip2, skip3, skip4, skip5, filename;
            iss >> skip >> skip2 >> skip3 >> skip4 >> skip5 >> filename;

            int port = 5000;
            std::string command = "./tcp_file_receiver 127.0.0.1 " + std::to_string(port) + " received_" + filename;
            std::cout << "[Клиент] Автоматический запуск приёма файла на порту " << port << "..." << std::endl;

            std::thread file_receiver_thread([command]() {
                system(command.c_str());
            });
            file_receiver_thread.detach();
        }
    }
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr{};

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    connect(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));

    std::string login, password;
    std::cout << "Введите логин: ";
    std::getline(std::cin, login);
    std::cout << "Введите пароль: ";
    std::getline(std::cin, password);

    std::string auth_data = login + " " + password;
    send(sockfd, auth_data.c_str(), auth_data.size(), 0);

    std::thread receiver(receive_messages, sockfd);

    while (true) {
        std::string msg;
        std::getline(std::cin, msg);

        if (msg.substr(0, 4) == "/pm ") {
            std::stringstream ss(msg);
            std::string command, recipient, private_message;
            ss >> command >> recipient;
            std::getline(ss, private_message);
            private_message = "[Приватное сообщение от " + login + "]: " + private_message;

            std::string pm_msg = "/pm " + recipient + " " + private_message;
            send(sockfd, pm_msg.c_str(), pm_msg.size(), 0);
            continue;
        }

        if (msg.substr(0, 5) == "/file") {
            std::stringstream ss(msg);
            std::string command, filename;
            int port;
            ss >> command >> filename >> port;

            std::string file_transfer_message = "[Сервер] Перейдите на TCP для передачи файла: " + filename;
            send(sockfd, file_transfer_message.c_str(), file_transfer_message.size(), 0);

            std::this_thread::sleep_for(std::chrono::seconds(2));
            std::string sender_command = "./tcp_file_sender 127.0.0.1 " + std::to_string(port) + " " + filename;
            system(sender_command.c_str());
            continue;
        }

        send(sockfd, msg.c_str(), msg.size(), 0);
    }

    receiver.join();
    close(sockfd);
    return 0;
}