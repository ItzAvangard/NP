#include <iostream>
#include <fstream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

void send_file(const char *ip, int port, const char *filename) {
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &servaddr.sin_addr);

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file\n";
        close(sockfd);
        return;
    }

    char buffer[1024];
    while (file.read(buffer, sizeof(buffer))) {
        send(sockfd, buffer, file.gcount(), 0);
    }
    send(sockfd, buffer, file.gcount(), 0);

    std::cout << "File sent successfully\n";

    file.close();
    close(sockfd);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <IP> <Port> <FileName>\n";
        return 1;
    }

    send_file(argv[1], std::stoi(argv[2]), argv[3]);
    return 0;
}