#include <iostream>
#include <fstream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <IP> <Port> <OutputFile>\n";
        return 1;
    }

    const char* ip = argv[1];
    int port = std::stoi(argv[2]);
    const char* filename = argv[3];

    int server_fd, new_socket;
    struct sockaddr_in address{};
    socklen_t addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip);
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 1) < 0) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    std::cout << "[TCP Receiver] Ожидание подключения на " << ip << ":" << port << "...\n";

    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) < 0) {
        perror("accept");
        close(server_fd);
        return 1;
    }

    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile) {
        std::cerr << "Не удалось открыть файл для записи\n";
        close(new_socket);
        close(server_fd);
        return 1;
    }

    char buffer[1024];
    ssize_t bytes_read;
    while ((bytes_read = read(new_socket, buffer, sizeof(buffer))) > 0) {
        outfile.write(buffer, bytes_read);
    }

    std::cout << "[TCP Receiver] Файл получен и сохранён как " << filename << std::endl;

    outfile.close();
    close(new_socket);
    close(server_fd);
    return 0;
}
