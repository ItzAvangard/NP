#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstring>
#include <cstdlib>
#include <sys/wait.h>

using namespace std;

void handle_client(int client_socket) {
    int number;
    while (recv(client_socket, &number, sizeof(number), 0) > 0) {
        cout << "Получено число: " << number << endl;
    }
    close(client_socket);
    exit(0);
}

void sigchld_handler(int) {
    while (waitpid(-1, nullptr, WNOHANG) > 0);
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        cerr << "Ошибка создания сокета!" << endl;
        exit(1);
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = 0;

    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Ошибка привязки сокета!" << endl;
        close(server_socket);
        exit(1);
    }

    socklen_t addr_len = sizeof(server_addr);
    getsockname(server_socket, (sockaddr*)&server_addr, &addr_len);
    cout << "Сервер запущен на порту: " << ntohs(server_addr.sin_port) << endl;

    listen(server_socket, 5);

    signal(SIGCHLD, sigchld_handler);

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_len);

        if (client_socket < 0) {
            cerr << "Ошибка принятия соединения!" << endl;
            continue;
        }

        if (fork() == 0) {
            close(server_socket);
            handle_client(client_socket);
        }
        close(client_socket);
    }

    close(server_socket);
    return 0;
}
