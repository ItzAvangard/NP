#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstdlib>

using namespace std;

int main() {
    string server_ip;
    int server_port;
    cout << "Введите IP-адрес сервера: ";
    cin >> server_ip;
    cout << "Введите порт сервера: ";
    cin >> server_port;

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        cerr << "Ошибка создания сокета!" << endl;
        exit(1);
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr);

    if (connect(client_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Ошибка подключения к серверу!" << endl;
        close(client_socket);
        exit(1);
    }

    int count;
    cout << "Введите количество отправок: ";
    cin >> count;

    for (int i = 1; i <= count; ++i) {
        send(client_socket, &i, sizeof(i), 0);
        cout << "Отправлено число: " << i << endl;
        sleep(i);
    }

    close(client_socket);
    return 0;
}
