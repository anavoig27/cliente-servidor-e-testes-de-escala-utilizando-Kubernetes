import socket
import time
import os
import uuid


MESSAGES_PER_CLIENT = 10
SERVER_HOST = "192.168.49.2"
SERVER_PORT = 32633

def run_client(username):
    received_count = 0
    total_time = 0
    

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        sock.connect((SERVER_HOST, SERVER_PORT)) 
        #abre uma conexão TCP com o servidor, usando o IP e a porta 

        mensagens_enviadas=0
        #Inicia uma contagem das mensagens que vão ser enviadas.

        for i in range(MESSAGES_PER_CLIENT): #Esse for vai rodar o número de vezes que foi definido por MESSAGES_PER_CLIENT
            start_time = time.time() #salva o tempo em que a requisição começou, pra depois calcular o tempo que demorou pra voltar a resposta do servidor.
            msg = f"Requisicao {i+1} de {MESSAGES_PER_CLIENT}" #Cria uma mensagem tipo "Requisicao 1 de 10", codifica em bytes (necessário pro envio com socket) e manda pro servidor.
            sock.sendall(msg.encode())
            mensagens_enviadas

            try:
                response = sock.recv(1024).decode() #Tenta receber a resposta do servidor
        
                received_count += 1 #conta que mais uma mensagem foi recebida.
                delta_time = time.time() - start_time #calcula quanto tempo levou pra receber a resposta.

                total_time += delta_time #soma esse tempo total, que pode ser usado depois pra calcular média.

                print(f"Ping para Servidor, seq={i+1}, time={(delta_time * 1000):.7f} (ms)") #mostra o tempo de ida e volta da mensagem (em milissegundos).

                
            except Exception as e:
                print(f"[{username}] Erro ao receber: {e}")
                break


    except Exception as e:
        print(f"[{username}] Falha na conexão: {e}")
    finally:
        sock.close()
        return MESSAGES_PER_CLIENT, received_count, total_time

if __name__ == "__main__":
    # Obtém username do ambiente, do hostname ou gera UUID
    username = os.getenv("USERNAME", os.getenv("HOSTNAME", f"user-{uuid.uuid4().hex[:8]}"))

    MESSAGES_PER_CLIENT = int(os.getenv("NUM_MSG_PER_CLIENTE", 1))
    username = username.split("-")[-1]
    sent, received, elapsed = run_client(username)

    print(f"[{username}] Total sent: {sent}")
    print(f"[{username}] Total received: {received}")
    print(f"[{username}] Elapsed time: {elapsed:.7f}")
