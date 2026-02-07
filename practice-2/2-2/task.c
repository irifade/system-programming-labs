#include <stdio.h>
#include <pthread.h>

long long sum = 0;
pthread_mutex_t m;

void* add_sum(void* arg) {
    int id = *(int*)arg;
    long long limit = id * 100;
    long long local_sum = 0;

    for (int i = 1; i <= limit; i++) local_sum += i;

    pthread_mutex_lock(&m);
    sum += local_sum;
    printf("Поток %d (1..%lld) добавил %lld. Итог: %lld\n", id, limit, local_sum, sum);
    pthread_mutex_unlock(&m);
    return NULL;
}

int main() {
    pthread_t t[4];
    int ids[4];
    pthread_mutex_init(&m, NULL);

    for (int i = 0; i < 4; i++) {
        ids[i] = i + 1;
        pthread_create(&t[i], NULL, add_sum, &ids[i]);
    }

    for (int i = 0; i < 4; i++) pthread_join(t[i], NULL);

    printf("\nФинальный результат: %lld\n", sum);
    pthread_mutex_destroy(&m);
    return 0;
}
