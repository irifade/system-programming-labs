@
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define COUNT 20

int main(void) {
    int pipe1[2];
    int pipe2[2];
    pid_t pid;

    if (pipe(pipe1) == -1) {
        perror("Ошибка при создании pipe1");
        return EXIT_FAILURE;
    }
    if (pipe(pipe2) == -1) {
        perror("Ошибка при создании pipe2");
        return EXIT_FAILURE;
    }

    pid = fork();
    if (pid == -1) {
        perror("Ошибка при вызове fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        int received_numbers[COUNT];
        int max_val;

        close(pipe1[1]); 
        close(pipe2[0]); 

        if (read(pipe1[0], received_numbers, sizeof(received_numbers)) == -1) {
            perror("[Дочерний] Ошибка чтения из pipe1");
            return EXIT_FAILURE;
        }
        close(pipe1[0]); 

        max_val = received_numbers[0];
        for (int i = 1; i < COUNT; i++) {
            if (received_numbers[i] > max_val) {
                max_val = received_numbers[i];
            }
        }

        if (write(pipe2[1], &max_val, sizeof(max_val)) == -1) {
            perror("[Дочерний] Ошибка записи в pipe2");
            return EXIT_FAILURE;
        }
        close(pipe2[1]); 

        return EXIT_SUCCESS;
    } else {
        int numbers[COUNT];
        int max_result;

        srand(time(NULL));
        for (int i = 0; i < COUNT; i++) {
            numbers[i] = rand() % 100;
        }

        close(pipe1[0]); 
        close(pipe2[1]); 

        if (write(pipe1[1], numbers, sizeof(numbers)) == -1) {
            perror("[Родитель] Ошибка записи в pipe1");
            return EXIT_FAILURE;
        }
        close(pipe1[1]); 

        if (read(pipe2[0], &max_result, sizeof(max_result)) == -1) {
            perror("[Родитель] Ошибка чтения из pipe2");
            return EXIT_FAILURE;
        }
        close(pipe2[0]); 

        wait(NULL);

        printf("[Родитель] Отправленные числа: ");
        for (int i = 0; i < COUNT; i++) {
            printf("%d ", numbers[i]);
        }
        printf("\n");
        printf("[Родитель] Полученный от потомка максимум: %d\n", max_result);

        return EXIT_SUCCESS;
    }
}
@
