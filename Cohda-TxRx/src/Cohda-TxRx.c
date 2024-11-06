#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int waitForTCPConnection(int port) {

	struct sockaddr_in all_IPv4_addr;
	int all_IPv4_len = sizeof(all_IPv4_addr);
	all_IPv4_addr.sin_family = AF_INET;
	all_IPv4_addr.sin_addr.s_addr = INADDR_ANY;
	all_IPv4_addr.sin_port = htons(port);

	int connection_req_sock = -1;
	int reuseaddr_opt = 1;

	int computer_sock = -1;

	if ((connection_req_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Connection socket failed to initialize");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(connection_req_sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_opt, sizeof(reuseaddr_opt))) {
		perror("setsockopt failed");
		exit(EXIT_FAILURE);
	}

	if (bind(connection_req_sock, (struct sockaddr *)&all_IPv4_addr, sizeof(all_IPv4_addr)) < 0) {
		perror("all_IPv4_addr did not bind to the connection socket.");
		exit(EXIT_FAILURE);
	}

	if (listen(connection_req_sock, 3) < 0) {
		perror("Failed to listen on connection socket");
		exit(EXIT_FAILURE);
	}

	printf("Listening for connection attempt...");

	if ((computer_sock = accept(connection_req_sock, (struct sockaddr *)&all_IPv4_addr, (socklen_t*)&all_IPv4_len)) < 0) {
		perror("Failed to connect");
		exit(EXIT_FAILURE);
	}

	return computer_sock;
}

int setupUDPSocket(int src_port, int dest_port, struct sockaddr_in *out_broadcast_dest) {

	struct sockaddr_in broadcast_src;
	int broadcast_src_len = sizeof(broadcast_src);
	broadcast_src.sin_family = AF_INET;
	char * src_ip_str = "127.0.0.1";
	struct in_addr src_ip;
	inet_pton(AF_INET, src_ip_str, &src_ip);
	broadcast_src.sin_addr.s_addr = src_ip.s_addr;
	broadcast_src.sin_port = htons(src_port);

	struct sockaddr_in broadcast_dest;
	broadcast_dest.sin_family = AF_INET;
	char * dest_ip_str = "127.0.0.1";
	struct in_addr dest_ip;
	inet_pton(AF_INET, dest_ip_str, &dest_ip);
	broadcast_dest.sin_addr.s_addr = dest_ip.s_addr;
	broadcast_dest.sin_port = htons(dest_port);

	int broadcast_sock = -1;
	int broadcast_enable = 1;

	if ((broadcast_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Broadcast socket could not be created.");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(broadcast_sock, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) < 0) {
		perror("Failed to set broadcast socket to broadcast");
		exit(EXIT_FAILURE);
	}

	if (bind(broadcast_sock, (struct sockaddr *)&broadcast_src, broadcast_src_len) < 0) {
		perror("Broadcast address failed to bind");
		exit(EXIT_FAILURE);
	}

	*out_broadcast_dest = broadcast_dest;
	return broadcast_sock;
}

int main(int argc, char *argv[]) {

	const int SENDER_PORT = 51001;
	const int BROADCAST_SRC_PORT = 51002;
	const int BROADCAST_DEST_PORT = 51010;

	int computer_sock = waitForTCPConnection(SENDER_PORT);
	printf("Connection established");

	struct sockaddr_in broadcast_dest_addr;
	int broadcast_sock = setupUDPSocket(BROADCAST_SRC_PORT, BROADCAST_DEST_PORT, &broadcast_dest_addr);

	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	read(computer_sock, buffer, sizeof(buffer));
	printf("\nSending message:\n\n");
	printf(buffer);

	if (sendto(broadcast_sock, buffer, strlen(buffer), 0, (struct sockaddr *)&broadcast_dest_addr, sizeof(broadcast_dest_addr)) < 0) {
		perror("Failed to send UDP packet on 'broadcast_sock'");
	}


	return EXIT_SUCCESS;
}
