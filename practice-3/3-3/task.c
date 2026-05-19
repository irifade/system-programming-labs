#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define PORT 8080
#define BUF_SIZE 1024
void run_server() {
    int server_fd, client_fd;
    struct sockaddr_in addr;
    char buffer[BUF_SIZE];
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { perror("socket"); exit(1); }
    int opt = 1; setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    addr.sin_family = AF_INET; addr.sin_addr.s_addr = INADDR_ANY; addr.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) { perror("bind"); exit(1); }
    if (listen(server_fd, 3) < 0) { perror("listen"); exit(1); }
    printf("Сервер запущен на порту %d. Ожидание клиента...
", PORT);
    if ((client_fd = accept(server_fd, NULL, NULL)) < 0) { perror("accept"); exit(1); }
    printf("Клиент подключен!
");
    while (1) {
        memset(buffer, 0, BUF_SIZE);
        int bytes = recv(client_fd, buffer, BUF_SIZE - 1, 0);
        if (bytes <= 0) break;
        printf("[Сервер] Получено: %s
", buffer);
        send(client_fd, buffer, bytes, 0);
    }
    close(client_fd); close(server_fd);
}
void run_client() {
    int sock = 0; struct sockaddr_in serv_addr; char buffer[BUF_SIZE];
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) { perror("socket"); exit(1); }
    serv_addr.sin_family = AF_INET; serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) { perror("inet_pton"); exit(1); }
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) { perror("connect"); exit(1); }
    printf("Подключено к серверу. Введите текст (для выхода - exit):
");
    while (1) {
        printf("> "); fflush(stdout);
        memset(buffer, 0, BUF_SIZE);
        if (!fgets(buffer, BUF_SIZE, stdin)) break;
        buffer[strcspn(buffer, "\n")] = 0;
        if (strcmp(buffer, "exit") == 0) break;
        send(sock, buffer, strlen(buffer), 0);
        memset(buffer, 0, BUF_SIZE);
        int bytes = recv(sock, buffer, BUF_SIZE - 1, 0);
        if (bytes <= 0) break;
        printf("[Эхо от сервера]: %s
", buffer);
    }
    close(sock);
}
int main(int argc, char *argv[]) {
    if (argc != 2) { printf("Использование: %s <server|client>\n", argv[0]); return 1; }
    if (strcmp(argv[1], "server") == 0) run_server();
    else if (strcmp(argv[1], "client") == 0) run_client();
    else printf("Неверный аргумент. Используйте 'server' или 'client'.\n");
    return 0;
}
