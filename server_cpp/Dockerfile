FROM debian:bookworm-slim

# Instalar ferramentas básicas e bibliotecas
RUN apt-get update && apt-get install -y \
    g++ \
    uuid-dev \
    make \
    && rm -rf /var/lib/apt/lists/*

# Criar diretório da aplicação
WORKDIR /app

# Copiar código-fonte
COPY server.cpp .

# Compilar o servidor
RUN g++ server.cpp -o server -lpthread -luuid

# Expor porta do servidor
EXPOSE 12345

# Comando padrão
CMD ["./server"]
