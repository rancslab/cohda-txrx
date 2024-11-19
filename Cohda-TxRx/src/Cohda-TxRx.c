#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// Waits for an incoming TCP connection request and accepts it if it gets one
int waitForTCPConnection(int port) {

	// Configures the address it's listening to for connections (any ipv6 address)
	struct sockaddr_in6 any_ipv6_addr;
	int any_ipv6_addr_len = sizeof(any_ipv6_addr);
	any_ipv6_addr.sin6_family = AF_INET6;
	any_ipv6_addr.sin6_addr = in6addr_any;
	any_ipv6_addr.sin6_port = htons(port);

	int connection_req_sock = -1;
	int reuseaddr_opt = 1;

	int computer_sock = -1;

	// Creates the socket on which it will listen for connections
	if ((connection_req_sock = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
		perror("Connection socket failed to initialize");
		exit(EXIT_FAILURE);
	}

	// Sets the socket to be reusable
	if (setsockopt(connection_req_sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_opt, sizeof(reuseaddr_opt))) {
		perror("setsockopt failed");
		exit(EXIT_FAILURE);
	}

	// Binds the any_ipv6_addr addres to this socket
	if (bind(connection_req_sock, (struct sockaddr *)&any_ipv6_addr, sizeof(any_ipv6_addr)) < 0) {
		perror("any_ipv6_addr did not bind to the connection socket.");
		exit(EXIT_FAILURE);
	}

	// Listens for connections on this socket
	if (listen(connection_req_sock, 3) < 0) {
		perror("Failed to listen on connection socket");
		exit(EXIT_FAILURE);
	}

	printf("Listening for connection attempt...");

	// Accepts any incoming connection request
	if ((computer_sock = accept(connection_req_sock, (struct sockaddr *)&any_ipv6_addr, (socklen_t*)&any_ipv6_addr_len)) < 0) {
		perror("Failed to connect");
		exit(EXIT_FAILURE);
	}

	// Returns the socket that the connection is on
	return computer_sock;
}

// Sets up the UDP broadcast socket for sending
int setupUDPSocket(int src_port, int dest_port, struct sockaddr_in6 *out_broadcast_dest) {

	// Sets the address from which UDP data will be sent
	struct sockaddr_in6 broadcast_src;
	int broadcast_src_len = sizeof(broadcast_src);
	broadcast_src.sin6_family = AF_INET6;
	char * src_ip_str = "::1";
	struct in6_addr src_ip;
	inet_pton(AF_INET6, src_ip_str, &src_ip);
	broadcast_src.sin6_addr = src_ip;
	broadcast_src.sin6_port = htons(src_port);

	// Sets the address to which UDP data will be sent
	struct sockaddr_in6 broadcast_dest;
	broadcast_dest.sin6_family = AF_INET6;
	char * dest_ip_str = "::1";
	struct in6_addr dest_ip;
	inet_pton(AF_INET6, dest_ip_str, &dest_ip);
	broadcast_dest.sin6_addr = dest_ip;
	broadcast_dest.sin6_port = htons(dest_port);

	int broadcast_sock = -1;
	int broadcast_enable = 1;

	// Creates the socket on which broadcasts will be sent
	if ((broadcast_sock = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {
		perror("Broadcast socket could not be created.");
		exit(EXIT_FAILURE);
	}

	// Enables broadcasting for the socket
	if (setsockopt(broadcast_sock, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) < 0) {
		perror("Failed to set broadcast socket to broadcast");
		exit(EXIT_FAILURE);
	}

	// Binds the address that data will be sent from to the socket
	if (bind(broadcast_sock, (struct sockaddr *)&broadcast_src, broadcast_src_len) < 0) {
		perror("Broadcast address failed to bind");
		exit(EXIT_FAILURE);
	}

	// Returns the broadcast destination address and the broadcast socket
	*out_broadcast_dest = broadcast_dest;
	return broadcast_sock;
}

int main(int argc, char *argv[]) {

	const int SENDER_PORT = 51001;
	const int BROADCAST_SRC_PORT = 51002;
	const int BROADCAST_DEST_PORT = 51010;

	int computer_sock = waitForTCPConnection(SENDER_PORT);
	printf("Connection established");

	struct sockaddr_in6 broadcast_dest_addr;
	int broadcast_sock = setupUDPSocket(BROADCAST_SRC_PORT, BROADCAST_DEST_PORT, &broadcast_dest_addr);

	// Sends test message on the UDP broadcast socket from ::1 port 51002 to ::1 port 51010
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
