#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define MAX_SEQ_NUM 10000

// Estructura para los paquetes a enviar
struct Packet {
    int seq_num;
    char data[BUF_SIZE];
};

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Uso: %s <host_destino> <puerto_destino> <nombre_archivo>\n", argv[0]);
        return 1;
    }

    FILE *file;
    char *filename = argv[3];
    int sockfd;
    struct sockaddr_in receiver_addr;
    int port = atoi(argv[2]);

    // Crear socket UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Error al crear el socket");
        return 1;
    }

    // Configurar la dirección del receptor
    memset(&receiver_addr, 0, sizeof(receiver_addr));
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(port);

    if (inet_aton(argv[1], &receiver_addr.sin_addr) == 0) {
        fprintf(stderr, "Dirección IP no válida\n");
        close(sockfd);
        return 1;
    }

    // Abrir el archivo a enviar
    file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        close(sockfd);
        return 1;
    }

    struct Packet packet;
    int seq_num = 0;
    ssize_t read_bytes;

    while ((read_bytes = fread(packet.data, 1, BUF_SIZE, file)) > 0) {
        packet.seq_num = seq_num;
        
        // Envío del paquete
        if (sendto(sockfd, &packet, read_bytes + sizeof(int), 0, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr)) == -1) {
            perror("Error al enviar el paquete");
            fclose(file);
            close(sockfd);
            return 1;
        }

        // Esperar ACK del receptor
        int ack;
        if (recvfrom(sockfd, &ack, sizeof(int), 0, NULL, NULL) == -1) {
            perror("Error al recibir ACK");
            fclose(file);
            close(sockfd);
            return 1;
        }

        seq_num++;
    }

    // Enviar paquete de finalización
    struct Packet end_packet = {-1, ""};
    sendto(sockfd, &end_packet, sizeof(end_packet), 0, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr));

    fclose(file);
    close(sockfd);
    return 0;
}
