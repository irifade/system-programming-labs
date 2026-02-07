#include <fcntl.h>   // Для open() и флагов O_...
#include <unistd.h>  // Для read(), write(), close()
#include <stdio.h>   // Для perror()
#include <errno.h>   // Для errno
#include <string.h>  // Для strlen()

#define DATA_FILE "data.txt"
#define LINES_FILE "lines.txt"
#define BUFFER_SIZE 1024

int main() {
    int fd_data, fd_lines;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    int line_count = 0;
    
    // 1. Создать файл data.txt с текстом (5+ строк)
    // Используем open с флагами O_WRONLY (только для записи), O_CREAT (создать, если нет), O_TRUNC (обрезать, если есть)
    // и правами доступа 0644 (чтение/запись для владельца, чтение для остальных)
    fd_data = open(DATA_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_data == -1) {
        perror("Error opening data.txt for writing");
        return 1;
    }

    const char* initial_text = "Line 1\nLine 2\nLine 3\nLine 4\nLine 5\nAnother one\n";
    if (write(fd_data, initial_text, strlen(initial_text)) == -1) {
        perror("Error writing to data.txt");
        close(fd_data);
        return 1;
    }
    close(fd_data);

    // 2. Открыть data.txt для чтения, чтобы подсчитать строки
    fd_data = open(DATA_FILE, O_RDONLY);
    if (fd_data == -1) {
        perror("Error opening data.txt for reading");
        return 1;
    }

    // Читаем файл побайтово/порциями и считаем символы новой строки
    while ((bytes_read = read(fd_data, buffer, BUFFER_SIZE)) > 0) {
        for (int i = 0; i < bytes_read; ++i) {
            if (buffer[i] == '\n') {
                line_count++;
            }
        }
    }

    if (bytes_read == -1) {
        perror("Error reading from data.txt");
        close(fd_data);
        return 1;
    }
    close(fd_data);

    // 3. Записать результат в lines.txt
    fd_lines = open(LINES_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_lines == -1) {
        perror("Error opening lines.txt for writing");
        return 1;
    }

    // Преобразуем количество строк в строку для записи
    char result_buffer[50];
    int len = sprintf(result_buffer, "Total lines: %d\n", line_count);

    if (write(fd_lines, result_buffer, len) == -1) {
        perror("Error writing to lines.txt");
        close(fd_lines);
        return 1;
    }
    close(fd_lines);

    printf("Successfully counted lines and wrote to %s\n", LINES_FILE);

    return 0;
}
