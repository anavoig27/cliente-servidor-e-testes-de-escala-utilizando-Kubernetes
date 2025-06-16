FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y \
    g++ \
    make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY client.cpp .

RUN g++ client.cpp -o client

# Espera o servidor estar pronto
CMD ["./client"]
