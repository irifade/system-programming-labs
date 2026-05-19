#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define SIZE 5
int main(void) {
    srand(time(NULL));
    int **matrix = (int **)malloc(SIZE * sizeof(int *));
    if (matrix == NULL) { perror("Ошибка выделения памяти для строк"); return EXIT_FAILURE; }
    for (int i = 0; i < SIZE; i++) {
        matrix[i] = (int *)malloc(SIZE * sizeof(int));
        if (matrix[i] == NULL) {
            perror("Ошибка выделения памяти для столбцов");
            for (int j = 0; j < i; j++) free(matrix[j]);
            free(matrix);
            return EXIT_FAILURE;
        }
    }
    printf("Сгенерированная матрица 5x5:\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            matrix[i][j] = rand() % 100;
            printf("%2d ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\nГлавная диагональ матрицы: ");
    for (int i = 0; i < SIZE; i++) {
        printf("%d ", matrix[i][i]);
    }
    printf("\n");
    for (int i = 0; i < SIZE; i++) {
        free(matrix[i]);
    }
    free(matrix);
    return EXIT_SUCCESS;
}
