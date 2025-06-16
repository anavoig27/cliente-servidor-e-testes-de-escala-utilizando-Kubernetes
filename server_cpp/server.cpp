#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <mutex>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>


#define PORT 12345



void send_msg(int client_sock, const std::string& msg) {
    send(client_sock, msg.c_str(), msg.size(), 0);
}

void* handle_client(void* arg) {
    int client_sock = *(int*)arg;
    delete (int*)arg;

    std::string username;
    char buffer[1024] = {0};

    try {

        // Loop para autenticação
        while (true) {
            ssize_t len = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
            if (len <= 0) break;

            buffer[len] = '\0';
            std::string msg(buffer);
            
            std::cout << "[Servidor] Mensagem recebida: " << msg;
            send_msg(client_sock, "Recebido\n");
        }

    } catch (...) {
        std::cerr << "Erro ao lidar com cliente.\n";
    }

    std::cout << "[-] " << username << " desconectado.\n";
    

    close(client_sock);
    pthread_exit(nullptr);
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Criar socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Falha ao criar socket");
        exit(EXIT_FAILURE);
    }

    // Reutilizar porta
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt falhou");
        exit(EXIT_FAILURE);
    }

    // Bind
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind falhou");
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, 10) < 0) {
        perror("listen falhou");
        exit(EXIT_FAILURE);
    }

    std::cout << "[*] Servidor escutando na porta " << PORT << "...\n";

    // Aceitar clientes
    while (true) {
        int* new_socket = new int;
        *new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (*new_socket < 0) {
            perror("accept falhou");
            delete new_socket;
            continue;
        }

        std::cout << "[*] Novo cliente conectado\n";
        pthread_t tid;
        pthread_create(&tid, nullptr, handle_client, new_socket);
        pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}
