import socket
import threading
import uuid


PORT = 12345
HOST = '0.0.0.0'
NAME = f"server-{uuid.uuid4().hex[:8]}"



print(f"Servidor ouvindo na porta {PORT}...")


class Server:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.name = NAME

        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)


    def send(self, message, sender_conn):
        sender_conn.sendall(message.encode())
        
    def handle_client(self, conn, addr):
        try:
            while True:
                data = conn.recv(1024)
                
                if not data:
                    break
                print(f"[{self.name}] Mensagem recebida de {addr}: {data.decode()}")

                self.send("Mensagem recebida com sucesso!", conn)

        except Exception as e:
            print("Erro ", e)

        finally:
            print(f"[-] User disconnected.")
            conn.close()

    def run(self):
        self.server_socket.bind((self.host, self.port))
        self.server_socket.listen()

        print(f"Server listening on {self.host}:{self.port}")

        while True:
            conn, addr = self.server_socket.accept()
            
            thread = threading.Thread(target=self.handle_client, args=(conn, addr))
            thread.start()


server = Server(HOST, PORT)
server.run()