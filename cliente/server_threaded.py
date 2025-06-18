import socket
import threading

HOST = '0.0.0.0'  # escuta em todas interfaces
PORT = 32633

def handle_client(conn, addr):
    print(f"[+] Conectado: {addr}")
    try:
        while True:
            data = conn.recv(1024)
            if not data:
                break
            msg = data.decode()
            print(f"[{addr}] Recebido: {msg}")
            # Responde algo pra o cliente saber que recebeu
            conn.sendall(f"OK: {msg}".encode())
    except Exception as e:
        print(f"[{addr}] Erro: {e}")
    finally:
        conn.close()
        print(f"[-] Desconectado: {addr}")

def main():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((HOST, PORT))
    server.listen()
    print(f"Servidor escutando em {HOST}:{PORT}")

    while True:
        conn, addr = server.accept()
        thread = threading.Thread(target=handle_client, args=(conn, addr))
        thread.start()
        print(f"[Threads ativos]: {threading.active_count() - 1}")  # -1 para descontar a main thread

if __name__ == "__main__":
    main()
