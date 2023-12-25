#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <getopt.h>

#define MAX_SEGMENT_SIZE 1024

struct Packet {
    int seq_num;
    char data[MAX_SEGMENT_SIZE];
};

void send_file(const char *dest_ip, int dest_port, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    int sockfd;
    struct sockaddr_in dest_addr;
    char buffer[MAX_SEGMENT_SIZE];
    socklen_t addr_size;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(dest_port);
    dest_addr.sin_addr.s_addr = inet_addr(dest_ip);

    struct Packet packet;
    int seq_num = 0;
    size_t bytes_read;

    while ((bytes_read = fread(packet.data, 1, MAX_SEGMENT_SIZE, file)) > 0) {
        packet.seq_num = seq_num++;
        if (sendto(sockfd, &packet, sizeof(int) + bytes_read, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
            perror("Error al enviar el paquete");
            fclose(file);
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        usleep(1000); // Simular un retardo para pruebas
    }

    fclose(file);
    close(sockfd);
}

int main(int argc, char **argv) {
    char *dest_ip = "127.0.0.1";
    int dest_port = 0;
    char *filename = NULL;

    // Parsing de argumentos usando getopt
    int opt;
    while ((opt = getopt(argc, argv, "h:p:f:")) != -1) {
        switch (opt) {
            case 'h':
                dest_ip = optarg;
                break;
            case 'p':
                dest_port = atoi(optarg);
                break;
            case 'f':
                filename = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s -h <host_destino> -p <puerto_destino> -f <nombre_archivo>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (dest_port == 0 || filename == NULL) {
        fprintf(stderr, "Usage: %s -h <host_destino> -p <puerto_destino> -f <nombre_archivo>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    send_file(dest_ip, dest_port, filename);

    return 0;
}
