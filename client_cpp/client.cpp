#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstdlib>
#include <sstream>  
#include <chrono>
#include <iomanip>



const char* SERVER_HOST = "chat-server";
const int SERVER_PORT = 12345;
int MESSAGES_PER_CLIENT = 10;

int get_env_int(const char* name, int fallback) {
    const char* val = std::getenv(name);
    if (val) {
        std::istringstream iss(val);
        int result;
        if (iss >> result) return result;
    }
    return fallback;
}

int connect_to_server(const char* host, int port) {
    struct addrinfo hints{}, *res, *p;
    int sock = -1;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_STREAM;  // TCP

    // Convert port to string
    std::string port_str = std::to_string(port);

    int status = getaddrinfo(host, port_str.c_str(), &hints, &res);
    if (status != 0) {
        std::cerr << "Erro em getaddrinfo: " << gai_strerror(status) << "\n";
        return -1;
    }

    for (p = res; p != nullptr; p = p->ai_next) {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock == -1) continue;

        if (connect(sock, p->ai_addr, p->ai_addrlen) == 0) {
            break; // Sucesso
        }

        close(sock);
        sock = -1;
    }

    freeaddrinfo(res);

    if (sock == -1) {
        std::cerr << "Falha ao conectar ao servidor\n";
    }

    return sock;
}

bool send_msg(int sock, const std::string& msg) {
    ssize_t sent = send(sock, msg.c_str(), msg.size(), 0);
    return sent == (ssize_t)msg.size();
}

std::string recv_msg(int sock) {
    char buffer[1024] = {0};
    ssize_t valread = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (valread <= 0) return "";
    return std::string(buffer, valread);
}

int run_client(std::string& username) {
    int sock = connect_to_server(SERVER_HOST, SERVER_PORT);
    if (sock < 0) return 1;

    srand(time(nullptr));


    // Enviar mensagens de autenticação com métricas
    int received_count = 0;
    std::chrono::duration<double> elapsed{0.0};
    

    for (int i = 0; i < MESSAGES_PER_CLIENT; ++i) {
        
        std::string msg = "Requisicao " + std::to_string(i+1) + " de " + std::to_string(MESSAGES_PER_CLIENT) + "\n";

        auto start = std::chrono::high_resolution_clock::now();
        if (!send_msg(sock, msg)) {
            std::cerr << "[" << username << "] Erro ao enviar mensagem\n";
            break;
        }
        
        std::string response = recv_msg(sock);
        if (response.empty()) {
            break;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> diff = end - start;
        std::cout << std::fixed << std::setprecision(9);
        std::cout << "Ping para Servidor, seq=" + std::to_string(i+1) +", time=" + std::to_string(diff.count() * 1000) +" (ms)\n";
        elapsed += diff;
        received_count++;

    }
        
    std::cout << "[" << username << "] Total sent: " << MESSAGES_PER_CLIENT << "\n";
    std::cout << "[" << username << "] Total received: " << received_count << "\n";
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "[" << username << "] Elapsed time: " << elapsed.count() << "\n";

    close(sock);
    return 0;
}


int main(int argc, char* argv[]) {
    std::string username = std::getenv("USERNAME") ? std::getenv("USERNAME") : "user-unknown";
    int messages_to_send = get_env_int("NUM_MSG_PER_CLIENTE", 10);

    MESSAGES_PER_CLIENT = messages_to_send;

    std::cout << "Executando cliente com username: " << username << std::endl;

    int result = run_client(username);

    return result;
}
