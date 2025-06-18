import socket
import time
import os
import uuid
import threading
import csv

SERVER_HOST = "192.168.49.2"
SERVER_PORT = 32633
MESSAGES_PER_CLIENT = 10
NUM_CLIENTES = 5  # Define quantos clientes simultâneos vão ser criados
CSV_FILE = "log_clientes.csv"

# Lock pra evitar que duas threads escrevam no CSV ao mesmo tempo
csv_lock = threading.Lock()

def run_client(username):
    received_count = 0
    total_time = 0
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        sock.connect((SERVER_HOST, SERVER_PORT))

        for i in range(MESSAGES_PER_CLIENT):
            start_time = time.time()
            msg = f"[{username}] Requisicao {i+1} de {MESSAGES_PER_CLIENT}"
            sock.sendall(msg.encode())

            try:
                response = sock.recv(1024).decode()
                received_count += 1
                delta_time = time.time() - start_time
                total_time += delta_time

                print(f"[{username}] Ping seq={i+1}, time={(delta_time * 1000):.3f} ms")

            except Exception as e:
                print(f"[{username}] Erro ao receber: {e}")
                break

    except Exception as e:
        print(f"[{username}] Falha na conexão: {e}")
    finally:
        sock.close()

        # Escreve no CSV de forma segura com o lock
        with csv_lock:
            with open(CSV_FILE, mode="a", newline="") as file:
                writer = csv.writer(file)
                writer.writerow([username, MESSAGES_PER_CLIENT, received_count, f"{total_time:.6f}"])

def main():
    # Cria o cabeçalho do CSV se ele não existir
    
    with open(CSV_FILE, mode="w", newline="") as file:
        writer = csv.writer(file)
        writer.writerow(["username", "mensagens_enviadas", "mensagens_recebidas", "tempo_total_segundos"])

    threads = []
    for i in range(NUM_CLIENTES):
        username = f"user-{i}"
        t = threading.Thread(target=run_client, args=(username,))
        threads.append(t)
        t.start()

    # Espera todas as threads terminarem
    for t in threads:
        t.join()

    print("Todos os clientes terminaram. Log salvo em:", CSV_FILE)

if __name__ == "__main__":
    main()
