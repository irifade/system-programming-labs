#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <dirent.h>

#define LOG_FILE "/tmp/daemon_task.log"
#define TARGET_DIR "/tmp" // Директория для подсчета файлов по умолчанию

volatile sig_atomic_t keep_running = 1;

void handle_signal(int sig) {
    if (sig == SIGTERM || sig == SIGINT) {
        keep_running = 0;
    }
}

int count_files(const char *dir_path) {
    int count = 0;
    struct dirent *entry;
    DIR *dir = opendir(dir_path);
    
    if (dir == NULL) {
        return -1;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        // Пропускаем ссылки на текущую директорию (.) и родительскую (..)
        if (entry->d_name[0] == '.' && (entry->d_name[1] == '\0' || (entry->d_name[1] == '.' && entry->d_name[2] == '\0'))) {
            continue;
        }
        count++;
    }
    closedir(dir);
    return count;
}

int main(void) {
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Ошибка fork");
        return EXIT_FAILURE;
    }
    // Завершаем родительский процесс, чтобы демон ушел в фон
    if (pid > 0) {
        return EXIT_SUCCESS;
    }
    
    // Создаем новый идентификатор сессии (SID) для дочернего процесса
    if (setsid() < 0) {
        return EXIT_FAILURE;
    }
    
    // Настраиваем обработку сигналов для корректного завершения
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    
    // Изменяем маску режима создания файлов
    umask(0);
    
    // Смена текущего каталога на безопасный (корневой)
    if (chdir("/") < 0) {
        return EXIT_FAILURE;
    }
    
    // Закрываем стандартные дескрипторы файлов
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    // Основной цикл демона
    while (keep_running) {
        FILE *log = fopen(LOG_FILE, "a");
        if (log != NULL) {
            time_t now = time(NULL);
            char *timestamp = ctime(&now);
            // Убираем перенос строки из ctime
            timestamp[24] = '\0';
            
            int file_count = count_files(TARGET_DIR);
            if (file_count >= 0) {
                fprintf(log, "[%s] Директория '%s' содержит файлов: %d\n", timestamp, TARGET_DIR, file_count);
            } else {
                fprintf(log, "[%s] Ошибка при чтении директории '%s'\n", timestamp, TARGET_DIR);
            }
            fclose(log);
        }
        
        // Спим 15 секунд
        sleep(15);
    }
    
    // Логируем успешное завершение работы демона
    FILE *log = fopen(LOG_FILE, "a");
    if (log != NULL) {
        fprintf(log, "Демон успешно остановлен.\n");
        fclose(log);
    }
    
    return EXIT_SUCCESS;
}
