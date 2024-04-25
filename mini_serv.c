#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> //man 7 ip
#include <string.h>

typedef struct client {
	int id;
	int fd;
} client;

int main(int ac, char **argv) {
	if (ac != 2) {
		write (2, "Wrong number of arguments\n", 26);
		exit (1);
	}
	
	int buffer_size = 200000;
	int max_client = 12800;
	char buffer[buffer_size];
	char message[buffer_size];
	int port = atoi(argv[1]);
	client client_array[max_client];
	bzero(client_array, sizeof(client) * max_client);
	
	int maxfd;
	int counter = 0;
	int server_socket;
	
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		write (2, "Fatal error\n", 12);
		exit (1);
	}
	
	struct sockaddr_in server_address = {0};
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	
	if ((bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address))) < 0) {
		write (2, "Fatal error\n", 12);
		exit (1);
	}
	
	fd_set activefd, readyfd;
	FD_ZERO(&activefd);
	FD_SET(server_socket, &activefd);
	maxfd = server_socket;
	
	if ((listen(server_socket, max_client)) < 0) {
		write (2, "Fatal error\n", 12);
		exit (1);
	}
	
	while (1) {
		readyfd = activefd;
		if ((select(maxfd + 1, &readyfd, NULL, NULL, NULL)) < 0) {
			write (2, "Fatal error\n", 12);
			exit (1);
		}
		for (int clientfd = 0; clientfd <= maxfd; clientfd++) {
			if (!FD_ISSET(clientfd, &readyfd))
				continue;
			bzero(buffer, sizeof(buffer));
			if (clientfd == server_socket) {
				int newfd;
				if ((newfd = accept(clientfd, NULL, NULL)) < 0) {
					continue;
				}
				if (newfd > maxfd)
					maxfd = newfd;
				FD_SET(newfd, &activefd);
				client_array[newfd].fd = newfd;
				client_array[newfd].id = counter++;
				
				sprintf(buffer, "server: client %d just arrived\n", client_array[newfd].id);
				for (int i = 0; i < max_client; i++) {
					if (client_array[i].fd != 0 && client_array[i].fd != newfd)
						send(client_array[i].fd, buffer, strlen(buffer), 0);
				}
			} else {
				bzero(buffer, sizeof(buffer));
				int len = -1;
				int bytes = 1;
				while (bytes == 1) {
					bytes = recv(clientfd, buffer + len + 1, 1, 0);
					len += 1;
					if (buffer[len] == '\n')
						break;
				}
				buffer[len + 1] = '\0';
				if (bytes <= 0) {
					bzero(message, buffer_size);
					sprintf(message, "server: client %d just left\n", client_array[clientfd].id);
					for (int i = 0; i < max_client; i++) {
						if (client_array[i].fd != 0 && client_array[i].fd != clientfd)
							send(client_array[i].fd, message, strlen(message), 0);
					}
					if ((close(clientfd)) < 0) {
						continue;
					}
					FD_CLR(clientfd, &activefd);
				} else {
					bzero(message, buffer_size);
					sprintf(message, "client %d: %s", client_array[clientfd].id, buffer);
					for (int i = 0; i < max_client; i++) {
						if (client_array[i].fd != 0 && client_array[i].fd != clientfd)
							send(client_array[i].fd, message, strlen(message), 0);
					}
				}
			}
		}
	}
	return (0);
}
