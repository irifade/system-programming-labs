#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

// Требование: volatile sig_atomic_t
volatile sig_atomic_t sig_count = 0;
// Начальное время фиксируем до бесконечного цикла
time_t start_time;

// Вспомогательная функция вывода (async-signal-safe)
void print_msg(const char *msg, int len) {
    write(STDOUT_FILENO, msg, len);
}

// Перевод числа в строку (async-signal-safe, без использования sprintf)
void print_seconds(long seconds) {
    char buf[32];
    int i = 0;
    if (seconds == 0) buf[i++] = '0';
    else {
        long temp = seconds;
        while (temp > 0 && i < 31) {
            buf[i++] = (temp % 10) + '0';
            temp /= 10;
        }
    }
    print_msg("\nSeconds passed: ", 17);
    while (i > 0) write(STDOUT_FILENO, &buf[--i], 1);
    print_msg("\n", 1);
}

void handle_sigint(int sig) {
    sig_count++;
    
    if (sig_count == 1) {
        // Используем clock_gettime вместо time(), так как она async-signal-safe
        struct timespec now;
        clock_gettime(CLOCK_REALTIME, &now); 
        print_seconds(now.tv_sec - start_time);
    } else {
        print_msg("\nExiting...\n", 12);
        _exit(0); // Требование: завершение через async-signal-safe функцию
    }
}

int main() {
    // Фиксируем время старта
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    start_time = ts.tv_sec;

    // Требование: sigaction
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        return 1;
    }

    print_msg("Running... Press Ctrl+C to see time. Press again to exit.\n", 57);

    while (1) {
        pause(); // Бесконечный цикл
    }

    return 0;
}
