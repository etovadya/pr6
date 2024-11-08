#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 12345
#define BUFFER_SIZE 1024

void log_message(const char *client_address, const char *message) {
    printf("%s:%s\n", client_address, message);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];
    int number_to_guess;

    // Создание сокета
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Привязка сокета к порту
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Начало прослушивания
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен на порту %d\n", PORT);

    while (1) {
        // Принятие нового соединения
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Получение адреса клиента
        char client_address[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &address.sin_addr, client_address, sizeof(client_address));
        
        log_message(client_address, "Подключение установлено");

        // Генерация случайного числа
        srand(time(NULL));
        number_to_guess = rand() % 100 + 1; // случайное число от 1 до 100
        
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int read_size = read(new_socket, buffer, BUFFER_SIZE);
            if (read_size <= 0) {
                log_message(client_address, "Отключение");
                close(new_socket);
                break;
            }

            int guess = atoi(buffer);
            if (guess < number_to_guess) {
                log_message(client_address, "Ваше число меньше");
                send(new_socket, "Ваше число меньше\n", 18, 0);
            } else if (guess > number_to_guess) {
                log_message(client_address, "Ваше число больше");
                send(new_socket, "Ваше число больше\n", 18, 0);
            } else {
                log_message(client_address, "Вы угадали число!");
                send(new_socket, "Вы угадали число!\n", 18, 0);
                break;
            }
        }
    }

    close(server_fd);
    return 0;
}
