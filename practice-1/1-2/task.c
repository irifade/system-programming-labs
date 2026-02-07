#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

void process_info(const char* name) {
    printf("Я процесс %s, PID: %d, родитель: %d\n", name, getpid(), getppid());
}

int main() {
    pid_t pid_b, pid_c;

    process_info("A"); // Процесс A

    // A создает B
    pid_b = fork();
    if (pid_b < 0) {
        perror("fork A->B failed"); // Обработка ошибок
        exit(1);
    }

    if (pid_b == 0) {
        // Это процесс B
        process_info("B");

        // B создает C
        pid_c = fork();
        if (pid_c < 0) {
            perror("fork B->C failed");
            exit(1);
        }

        if (pid_c == 0) {
            // Это процесс C
            process_info("C");
            exit(0); // C завершается
        } else {
            // Это снова процесс B (родитель C)
            wait(NULL); // B ждет C
            exit(0); // B завершается
        }
    } else {
        // Это снова процесс A (родитель B)
        wait(NULL); // A ждет B
    }

    return 0;
}
