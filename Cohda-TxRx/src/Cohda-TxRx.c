#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {

	const int PORT_INCOMING_SENDER = 51001;
	const int PORT_OUTGOING_SENDER = 51002;
	const int PORT_INCOMING_BROADCAST = 51003;
	const int PORT_OUTGOING_BROADCAST = 51004;

	struct sockaddr_in incoming_any;
	int incoming_any_len = sizeof(incoming_any);
	incoming_any.sin_family = AF_INET;
	incoming_any.sin_addr.s_addr = INADDR_ANY;
	incoming_any.sin_port = htons(PORT_INCOMING_SENDER);

	struct sockaddr_in incoming_computer;


	struct sockaddr_in incoming_broadcast;

	int any_sock = 0;
	int reuseaddr_opt = 1;

	int computer_sock = 0;

	int broadcast_sock = 0;

	if ((any_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Socket failed to initialize");
		exit(EXIT_FAILURE);
	}

	if ((broadcast_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Socket failed to initialize");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(any_sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_opt, sizeof(reuseaddr_opt))) {
		perror("setsockopt failed");
		exit(EXIT_FAILURE);
	}

	if (bind(any_sock, (struct sockaddr *)&incoming_any, sizeof(incoming_any)) < 0) {
		perror("incoming_any did not bind.");
		exit(EXIT_FAILURE);
	}

	if (listen(any_sock, 3) < 0) {
		perror("Failed to listen on incoming_any");
		exit(EXIT_FAILURE);
	}

	printf("Listening for connection attempt...");

	if ((computer_sock = accept(any_sock, (struct sockaddr *)&incoming_any, (socklen_t*)&incoming_any_len)) < 0) {
		perror("Failed to connect");
		exit(EXIT_FAILURE);
	}

	const char * message = "HELLO THANKS FOR CONNECTING";
	send(computer_sock, message, strlen(message), 0);

	return EXIT_SUCCESS;
}
