#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main() {
    int server_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = 0;

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);

    }

    std::cout << server_addr.sin_port << std::endl;

    getsockname(server_socket, (struct sockaddr *)&server_addr, &client_len);
    std::cout << "Server is running on port: " << ntohs(server_addr.sin_port) << std::endl;

    while (true) {
        int recv_len = recvfrom(server_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);
        if (recv_len < 0) {
            perror("Recvfrom failed");
            continue;
        }

        buffer[recv_len] = '\0';
        std::cout << "Received message: " << buffer << " from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;

        for (int i = 0; i < recv_len; i++) {
            buffer[i] = toupper(buffer[i]);
        }

        if (sendto(server_socket, buffer, recv_len, 0, (struct sockaddr *)&client_addr, client_len) < 0) {
            perror("Sendto failed");
        }
    }

    close(server_socket);
    return 0;
}
