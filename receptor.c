#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>

#define BUF_SIZE 1024
#define MAX_SEQ_NUM 10000

struct Packet {
    int seq_num;
    char data[BUF_SIZE];
};

void createReceivedFolder() {
    struct stat st = {0};
    if (stat("archivos_recibidos", &st) == -1) {
        if (mkdir("archivos_recibidos", 0777) == -1) {
            perror("Error al crear la carpeta para archivos recibidos");
            exit(1);
        }
    }
}

void receiveAndSaveFiles(int sockfd, int num_emisores) {
    FILE *file = NULL;
    int expected_seq_num = 0;
    int received_files = 0;

    while (1) {
        struct Packet packet;
        struct sockaddr_in sender_addr;
        socklen_t sender_len = sizeof(sender_addr);

        ssize_t recv_len = recvfrom(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&sender_addr, &sender_len);

        if (recv_len == -1) {
            perror("Error al recibir el paquete");
            continue;
        }

        if (packet.seq_num == -1) {
            printf("Archivo recibido correctamente por el emisor #%d.\n", received_files + 1);
            received_files++;

            if (received_files == num_emisores) {
                printf("Todos los archivos han sido recibidos.\n");
                break;
            }
            continue;
        }

        if (packet.seq_num == expected_seq_num) {
            if (file == NULL) {
                char filepath[100];
                sprintf(filepath, "archivos_recibidos/archivo_recibido_emisor_%d", received_files + 1);
                file = fopen(filepath, "wb");
                if (file == NULL) {
                    perror("Error al abrir el archivo");
                    break;
                }
            }

            fwrite(packet.data, 1, recv_len - sizeof(int), file);
            expected_seq_num++;
        }

        sendto(sockfd, &expected_seq_num, sizeof(int), 0, (struct sockaddr *)&sender_addr, sender_len);
    }

    if (file != NULL) {
        fclose(file);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <puerto> <num_emisores>\n", argv[0]);
        return 1;
    }

    int sockfd;
    struct sockaddr_in receiver_addr;
    int port = atoi(argv[1]);
    int num_emisores = atoi(argv[2]);

    createReceivedFolder();

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Error al crear el socket");
        return 1;
    }

    memset(&receiver_addr, 0, sizeof(receiver_addr));
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    receiver_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr)) == -1) {
        perror("Error al vincular el socket");
        close(sockfd);
        return 1;
    }

    receiveAndSaveFiles(sockfd, num_emisores);

    close(sockfd);
    return 0;
}
