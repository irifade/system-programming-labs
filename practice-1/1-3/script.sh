#!/bin/bash

# Каталог назначения
DEST_DIR="testdir"
# Файлы для копирования 
FILES=("/etc/hosts" "/etc/hostname" "/etc/issue")

# 1. Создать директорию testdir
mkdir -p "$DEST_DIR"
echo "Директория '$DEST_DIR' создана."

# 2. Скопировать 3 файла из /etc
for file_path in "${FILES[@]}"; do
    cp "$file_path" "$DEST_DIR/"
    echo "Файл $(basename "$file_path") скопирован в '$DEST_DIR/'."
done

# 3. Установить права 444 (только чтение для всех) для всех файлов
chmod 444 "$DEST_DIR"/*
echo "Права доступа 444 установлены для всех файлов в '$DEST_DIR/'."
ls -l "$DEST_DIR"

# 4. Попробовать изменить один из файлов (должна быть ошибка)
echo -e "\nПопытка записи в файл (ожидается ошибка 'Отказано в доступе'):"
echo "Попытка записи" >> "$DEST_DIR/hosts"
