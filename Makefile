CC = gcc
CFLAGS = -Wall -Wextra

all: emisor receptor

emisor: emisor.c
	$(CC) $(CFLAGS) emisor.c -o emisor

receptor: receptor.c
	$(CC) $(CFLAGS) receptor.c -o receptor

clean:
	rm -f emisor receptor received_file