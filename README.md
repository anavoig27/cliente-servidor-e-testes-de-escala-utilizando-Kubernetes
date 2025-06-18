# cliente-servidor-e-testes-de-escala-utilizando-Kubernetes
# Projeto Cliente-Servidor e Testes de Escala com Kubernetes

## Descrição
Este projeto implementa uma comunicação cliente-servidor usando Python e C++, e realiza testes de escala usando Kubernetes para orquestrar vários clientes simultâneos.

## Tecnologias e Dependências
- Python 3.x
- C++11
- GCC/G++ para compilar o cliente C++
- Kubernetes (kubectl)
- Docker (opcional, se usar containers)

## Estrutura do projeto
- `/client_cpp`: código do cliente em C++
- `/client_python`: código do cliente em Python
- `/server`: código do servidor em Python
- `/k8s`: manifests do Kubernetes
- `/scripts`: scripts auxiliares (se tiver)
- `README.md`: este arquivo

## Como rodar localmente
### 1. Rodar o servidor (Python)

Primeiro, abra um terminal e entre na pasta do servidor:
```bash
cd server
python3 server_threaded.py

1. Rodar o servidor (Python)
Primeiro, abre um terminal e entra na pasta do servidor:
cd server  
python3 server_threaded.py  

2. Compilar e rodar o cliente C++
Em outro terminal, entra na pasta do cliente C++ e compila o código:
cd client_cpp  
g++ -std=c++11 -pthread client.cpp -o client  

Depois, executa o cliente:
./client  
O cliente vai se conectar ao servidor e enviar mensagens.