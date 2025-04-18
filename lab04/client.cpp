#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <chrono>

int main(int argc, char const *argv[]) {
    if (argc != 3) {
        std::cout << "Использование: " << argv[0] << " <IP сервера> <порт>" << std::endl;
        return -1;
    }

    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "Ошибка создания сокета" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
        std::cout << "Неверный адрес/ Адрес не поддерживается" << std::endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "Ошибка подключения" << std::endl;
        return -1;
    }

    int delay;
    std::cout << "Введите число от 1 до 10: ";
    std::cin >> delay;

    if (delay < 1 || delay > 10) {
        std::cout << "Число должно быть от 1 до 10" << std::endl;
        return -1;
    }

    while (true) {
        std::string message = std::to_string(delay);
        send(sock, message.c_str(), message.length(), 0);
        std::cout << "Отправлено: " << message << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(delay));
    }

    return 0;
} 