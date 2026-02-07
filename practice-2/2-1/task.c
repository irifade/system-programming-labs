#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 5

void* thread_function(void* arg) {
    int id = *((int*)arg);
    // Генерация случайного времени от 1 до 5 секунд
    int sleep_time = (rand() % 5) + 1;
    
    sleep(sleep_time);
    
    printf("Поток [%d] проснулся (спал %d сек.)\n", id, sleep_time);
    
    free(arg);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    time_t start_time = time(NULL);
    srand(time(NULL));

    for (int i = 0; i < NUM_THREADS; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        if (pthread_create(&threads[i], NULL, thread_function, id) != 0) {
            perror("Ошибка создания потока");
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    time_t end_time = time(NULL);
    printf("Общее время работы программы: %ld сек.\n", end_time - start_time);

    return 0;
}
