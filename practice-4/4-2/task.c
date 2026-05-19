#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Использование: %s файл1 файл2\n", argv[0]);
        return EXIT_FAILURE;
    }
    int src_fd = open(argv[1], O_RDONLY);
    if (src_fd == -1) { perror("Ошибка открытия исходного файла"); return EXIT_FAILURE; }
    struct stat st;
    if (fstat(src_fd, &st) == -1) { perror("Ошибка fstat"); close(src_fd); return EXIT_FAILURE; }
    if (st.st_size == 0) {
        int dst_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (dst_fd != -1) close(dst_fd);
        close(src_fd);
        return EXIT_SUCCESS;
    }
    int dst_fd = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (dst_fd == -1) { perror("Ошибка создания целевого файла"); close(src_fd); return EXIT_FAILURE; }
    if (ftruncate(dst_fd, st.st_size) == -1) { perror("Ошибка ftruncate"); close(src_fd); close(dst_fd); return EXIT_FAILURE; }
    void *src_map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, src_fd, 0);
    if (src_map == MAP_FAILED) { perror("Ошибка mmap исходного файла"); close(src_fd); close(dst_fd); return EXIT_FAILURE; }
    void *dst_map = mmap(NULL, st.st_size, PROT_WRITE, MAP_SHARED, dst_fd, 0);
    if (dst_map == MAP_FAILED) { perror("Ошибка mmap целевого файла"); munmap(src_map, st.st_size); close(src_fd); close(dst_fd); return EXIT_FAILURE; }
    memcpy(dst_map, src_map, st.st_size);
    msync(dst_map, st.st_size, MS_SYNC);
    munmap(src_map, st.st_size);
    munmap(dst_map, st.st_size);
    close(src_fd);
    close(dst_fd);
    printf("Файл успешно скопирован через mmap!\n");
    return EXIT_SUCCESS;
}
