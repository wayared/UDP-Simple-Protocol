#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_SEGMENT_SIZE 1024

void receive_file(int port) {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    unsigned char buffer[MAX_SEGMENT_SIZE]; // Usar unsigned char para manejar datos binarios
    socklen_t addr_size;
    int n;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("[-]socket error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    n = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (n < 0) {
        perror("[-]bind error");
        exit(EXIT_FAILURE);
    }

    // Crear la carpeta "archivos_recibidos" si no existe
    mkdir("archivos_recibidos", 0777);

    struct sockaddr_in client_addr_rcv;
    socklen_t addr_rcv_size;

    int file_fd = -1;  // Mantener el descriptor de archivo abierto fuera del bucle

    while (1) {
        bzero(buffer, MAX_SEGMENT_SIZE);
        addr_rcv_size = sizeof(client_addr_rcv);
        n = recvfrom(sockfd, buffer, MAX_SEGMENT_SIZE, 0, (struct sockaddr*)&client_addr_rcv, &addr_rcv_size);
        if (n < 0) {
            perror("Error al recibir el paquete");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        if (file_fd < 0) {
            // Abrir el archivo si aún no está abierto
            char filename[50];
            sprintf(filename, "archivos_recibidos/archivo_%d.txt", (int)time(NULL));
            file_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (file_fd < 0) {
                perror("Error al abrir el archivo");
                close(sockfd);
                exit(EXIT_FAILURE);
            }
        }

        // Escribir los datos en binario
        int bytes_written = write(file_fd, buffer, n);
        if (bytes_written != n) {
            perror("Error al escribir en el archivo");
            close(file_fd);
            close(sockfd);
            exit(EXIT_FAILURE);
        }
    }

    close(file_fd); // Cerrar el archivo después del bucle (si se abre)
    close(sockfd);
}

int main(int argc, char **argv) {
    int port = 0;

    // Parsing de argumentos usando getopt
    int opt;
    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p':
                port = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -p <puerto>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (port == 0) {
        fprintf(stderr, "Usage: %s -p <puerto>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    receive_file(port);

    return 0;
}