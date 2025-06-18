#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <thread>
#include <vector>
#include <mutex>

const char* SERVER_HOST = "192.168.49.2";  // Ajusta aqui o IP do seu servidor
const int SERVER_PORT = 32633;             // Ajusta a porta do servidor
int MESSAGES_PER_CLIENT = 10;
int NUM_CLIENTS = 5;

std::mutex log_mutex;
std::mutex print_mutex;

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
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    std::string port_str = std::to_string(port);

    int status = getaddrinfo(host, port_str.c_str(), &hints, &res);
    if (status != 0) {
        std::lock_guard<std::mutex> lock(print_mutex);
        std::cerr << "Erro em getaddrinfo: " << gai_strerror(status) << "\n";
        return -1;
    }

    for (p = res; p != nullptr; p = p->ai_next) {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock == -1) continue;

        if (connect(sock, p->ai_addr, p->ai_addrlen) == 0) {
            break;
        }

        close(sock);
        sock = -1;
    }

    freeaddrinfo(res);
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

void safe_print(const std::string& msg) {
    std::lock_guard<std::mutex> lock(print_mutex);
    std::cout << msg << std::endl;
}

void run_client(std::string username, std::vector<std::string>& results) {
    int sock = connect_to_server(SERVER_HOST, SERVER_PORT);
    if (sock < 0) {
        std::lock_guard<std::mutex> lock(log_mutex);
        results.push_back(username + ",0,0,0.000000");
        safe_print("[" + username + "] Falha ao conectar ao servidor.");
        return;
    }

    int received_count = 0;
    std::chrono::duration<double> elapsed{0.0};

    for (int i = 0; i < MESSAGES_PER_CLIENT; ++i) {
        std::string msg = "Requisicao " + std::to_string(i + 1) + " de " + std::to_string(MESSAGES_PER_CLIENT);

        auto start = std::chrono::high_resolution_clock::now();

        if (!send_msg(sock, msg)) {
            safe_print("[" + username + "] Erro ao enviar mensagem.");
            break;
        }

        std::string response = recv_msg(sock);
        if (response.empty()) {
            safe_print("[" + username + "] Servidor fechou a conexão.");
            break;
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;

        safe_print("[" + username + "] Ping seq=" + std::to_string(i + 1) + ", time=" + std::to_string(diff.count() * 1000) + " ms");
        elapsed += diff;
        received_count++;
    }

    close(sock);

    // Salva resultado com proteção do mutex
    {
        std::lock_guard<std::mutex> lock(log_mutex);
        std::ostringstream oss;
        oss << username << "," << MESSAGES_PER_CLIENT << "," << received_count << "," << std::fixed << std::setprecision(6) << elapsed.count();
        results.push_back(oss.str());
    }
}

int main() {
    MESSAGES_PER_CLIENT = get_env_int("NUM_MSG_PER_CLIENTE", 10);
    NUM_CLIENTS = get_env_int("NUM_CLIENTS", 5);

    std::vector<std::thread> threads;
    std::vector<std::string> results;

    for (int i = 0; i < NUM_CLIENTS; ++i) {
        std::string username = "user-" + std::to_string(i);
        threads.emplace_back(run_client, username, std::ref(results));
    }

    for (auto& t : threads) {
        t.join();
    }

    // Salvar log no CSV
    std::ofstream log_file("log_clientes_cpp.csv");
    log_file << "username,mensagens_enviadas,mensagens_recebidas,tempo_total_segundos\n";
    for (const auto& line : results) {
        log_file << line << "\n";
    }

    std::cout << "Todos os clientes terminaram. Log salvo em: log_clientes_cpp.csv\n";

    return 0;
}
