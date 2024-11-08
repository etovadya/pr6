#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char const *argv[]) {
    if (argc != 3) {
        printf("Использование: %s <адрес сервера> <0 для автоматического или 1 для интерактивного>\n", argv[0]);
        return -1;
    }

    const char *server_address = argv[1];
    int mode = atoi(argv[2]);
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    
    // Создание сокета
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Ошибка создания сокета \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(12345);
    
    // Конвертация адреса
    if (inet_pton(AF_INET, server_address, &serv_addr.sin_addr) <= 0) {
        printf("\n Неверный адрес/адрес не поддерживается \n");
        return -1;
    }

    // Подключение к серверу
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\n Не удалось подключиться к серверу \n");
        return -1;
    }

    // Игровой цикл
    while (1) {
        if (mode == 1) {
            // Интерактивный режим
            int guess;
            printf("Введите число от 1 до 100: ");
            scanf("%d", &guess);
            snprintf(buffer, sizeof(buffer), "%d", guess);
        } else {
            // Автоматический режим
            int guess = rand() % 100 + 1;
            printf("Автоматический выбор числа: %d\n", guess);
            snprintf(buffer, sizeof(buffer), "%d", guess);
        }

        // Отправка числа на сервер
        send(sock, buffer, strlen(buffer), 0);

        // Получение ответа от сервера
        memset(buffer, 0, BUFFER_SIZE);
        read(sock, buffer, BUFFER_SIZE);
        printf("%s", buffer);

        // Завершение игры, если угадали число
        if (strstr(buffer, "Вы угадали число") != NULL) {
            break;
        }
    }

    close(sock);
    return 0;
}
