#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#define FIFO_NAME "chat_fifo"
#define BUFFER_SIZE 256
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Использование: %s <1 или 2>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int role = atoi(argv[1]);
    if (role != 1 && role != 2) return EXIT_FAILURE;
    mkfifo(FIFO_NAME, 0666);
    int fd;
    if (role == 1) fd = open(FIFO_NAME, O_WRONLY);
    else fd = open(FIFO_NAME, O_RDONLY);
    if (fd == -1) { perror("open"); return EXIT_FAILURE; }
    pid_t pid = fork();
    if (pid == -1) return EXIT_FAILURE;
    if (pid == 0) {
        int write_fd = (role == 1) ? fd : open(FIFO_NAME, O_WRONLY);
        char write_buf[BUFFER_SIZE];
        while (fgets(write_buf, BUFFER_SIZE, stdin)) {
            write_buf[strcspn(write_buf, "\n")] = 0;
            char msg[BUFFER_SIZE + 20];
            snprintf(msg, sizeof(msg), "[Участник %d]: %s", role, write_buf);
            write(write_fd, msg, strlen(msg) + 1);
            if (strcmp(write_buf, "exit") == 0) break;
        }
        close(write_fd);
        return EXIT_SUCCESS;
    } else {
        int read_fd = (role == 2) ? fd : open(FIFO_NAME, O_RDONLY);
        char read_buf[BUFFER_SIZE];
        while (1) {
            int bytes = read(read_fd, read_buf, BUFFER_SIZE);
            if (bytes <= 0) break;
            printf("%s\n", read_buf);
            if (strstr(read_buf, ": exit") != NULL) break;
        }
        close(read_fd);
        kill(pid, 2);
        wait(NULL);
        if (role == 1) unlink(FIFO_NAME);
    }
    return EXIT_SUCCESS;
}
