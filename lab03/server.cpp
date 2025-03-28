#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fstream>
#include <cstring>

using namespace std;

#define PORT 0
#define BACKLOG 5

pthread_mutex_t file_mutex;

void* handle_client(void* arg) {
    int client_sock = *(int*)arg;
    delete (int*)arg;

    char buffer[1024];
    int bytes_received;
    while ((bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';

        cout << "Получено: " << buffer << endl;

        pthread_mutex_lock(&file_mutex);
        ofstream file("server_log.txt", ios::app);
        file << buffer << endl;
        file.close();
        pthread_mutex_unlock(&file_mutex);
    }

    close(client_sock);
    cout << "Клиент отключился" << endl;
    return nullptr;
}

int main() {
    pthread_mutex_init(&file_mutex, nullptr);

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        cerr << "Ошибка создания сокета" << endl;
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Ошибка bind" << endl;
        return 1;
    }

    socklen_t addr_len = sizeof(server_addr);
    getsockname(server_sock, (sockaddr*)&server_addr, &addr_len);
    cout << "Сервер запущен на порту: " << ntohs(server_addr.sin_port) << endl;

    listen(server_sock, BACKLOG);

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int* client_sock = new int(accept(server_sock, (sockaddr*)&client_addr, &client_len));
        if (*client_sock < 0) {
            cerr << "Ошибка accept" << endl;
            delete client_sock;
            continue;
        }

        pthread_t thread_id;
        pthread_create(&thread_id, nullptr, handle_client, client_sock);
        pthread_detach(thread_id);
    }

    close(server_sock);
    pthread_mutex_destroy(&file_mutex);
    return 0;
}
