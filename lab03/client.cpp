#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Использование: " << argv[0] << " <IP> <порт> <N>" << endl;
        return 1;
    }

    const char* server_ip = argv[1];
    int port = atoi(argv[2]);
    int N = atoi(argv[3]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr << "Ошибка создания сокета" << endl;
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Ошибка подключения" << endl;
        close(sock);
        return 1;
    }

    for (int i = 1; i <= N; i++) {
        string message = "Число: " + to_string(i);
        send(sock, message.c_str(), message.size(), 0);
        cout << "Отправлено: " << message << endl;
        sleep(i);
    }

    close(sock);
    return 0;
}
