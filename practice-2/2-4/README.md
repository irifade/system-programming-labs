# Отчет по Заданию 2-4: Отладка с strace

## Вариант 4

**Задание:**
Использовать утилиту `strace` для анализа системных вызовов в программе, которая создает дочерний процесс с помощью функции `fork()`. Требовалось отследить вызовы `clone()`/`fork()` и определить PID родительского и дочернего процессов.


## 2. Инструкция по запуску

### 2.1. Подготовка программы

Сохраните следующий код в файл `program.c`:

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    if (pid == 0) {
        sleep(1);
        return 0;
    }
    wait(NULL);
    return 0;
}
```
### 2.2. Компиляция и установка strace

Выполните следующие команды в терминале WSL:
```bash
sudo apt install strace 
gcc program.c -o program 
```
### 2.3. Запуск анализа
Запустите программу с strace, используя флаг -f (для отслеживания дочерних процессов) и сохраняя вывод в файл analysis.txt:
```bash
strace -f -e trace=clone,fork,getpid,exit_group -o analysis.txt ./program
```

### 2.4. Просмотр результатов
Для извлечения ключевых строк анализа используйте команду grep:
```bash
grep -E "clone|fork|attached" analysis.txt
```

### 3. Результаты анализа
В результате выполнения команд был получен следующий вывод в консоль (и в файл analysis.txt):

984 clone(child_stack=NULL, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, child_tidptr=0x7a70ab582a10) = 985


На основе этого вывода были определены:
PID родительского процесса: 984 (указан перед вызовом clone).
PID дочернего процесса: 985 (значение, возвращаемое вызовом clone).


---


