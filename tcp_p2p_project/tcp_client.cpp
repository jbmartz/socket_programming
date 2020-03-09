#include <iostream>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>

/**
 *
 * TCP client example. Reads in IP PORT
 * from the command line, and sends DATA via TCP to IP:PORT.
 *
 * e.g., ./tcpclient 127.0.0.1 8888
 *
 * @param argc count of arguments on the command line
 * @param argv array of command line arguments
 * @return 0 on success, non-zero if an error occurred
 */
int setup_client(int argc, char *argv[]) {
    // Alias for argv[1] for convenience
    char *ip_string;
    // Alias for argv[2] for convenience
    char *port_string;
    // Alias for argv[3] for convenience
    char *data_string;

    // Port to send TCP data to. Need to convert from command line string to a number
    unsigned int port;
    // The socket used to send data
    int tcp_socket;
    // Variable used to check return codes from various functions
    int ret;

    struct addrinfo hints;
    struct addrinfo *results;
    struct addrinfo *results_it;

    // Note: this needs to be 4, because the program name counts as an argument!
    if (argc < 4) {
        std::cerr << "Please specify IP PORT MESSAGE as first three arguments." << std::endl;
        return 1;
    }
    // Set up variables "aliases"
    ip_string = argv[1];
    port_string = argv[2];
    data_string = argv[3];

    // Create the TCP socket.
    // AF_INET is the address family used for IPv4 addresses
    // SOCK_STREAM indicates creation of a TCP socket
    // Create TCP socket
    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Make sure socket was created successfully, or exit.
    if (tcp_socket == -1) {
        std::cerr << "Failed to create tcp socket!" << std::endl;
        std::cerr << strerror(errno) << std::endl;
        return 1;
    }
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_addr = NULL;
    hints.ai_canonname = NULL;
    hints.ai_family = AF_INET;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    // Instead of using inet_pton, use getaddrinfo to convert.
    ret = getaddrinfo(ip_string, port_string, &hints, &results);

    if (ret != 0) {
        std::cerr << "Getaddrinfo failed with error " << ret << std::endl;
        return 1;
    }

    // Check we have at least one result
    results_it = results;

    while (results_it != NULL) {
        std::cout << "Trying to connect to something?" << std::endl;
        // Replace with call to connect
        ret = connect(tcp_socket, results_it->ai_addr, results_it->ai_addrlen);
        if (ret == 0) {
            break;
        }
        perror("connect");
        results_it = results_it->ai_next;
    }

    // Whatever happened, we need to free the address list.
    freeaddrinfo(results);

    // Check if connecting succeeded at all
    if (ret != 0) {
        std::cout << "Failed to connect to any addresses!" << std::endl;
        return 1;
    }

    // If we get here, the connect worked. Now send the data.
    ret = send(tcp_socket, data_string, strlen(data_string), 0); // Replace with call to send

    // Check if send worked, clean up and exit if not.
    if (ret == -1) {
        std::cerr << "Failed to send data!" << std::endl;
        perror("send");
        std::cerr << strerror(errno) << std::endl;
        close(tcp_socket);
        return 1;
    }

    std::cout << "Sent " << ret << " bytes " << std::endl;

    char recv_buf[2048];
    // Replace with call to receive
    if (ret != -1) {
        std::cout << "Received " << ret << " bytes from server." << std::endl;
        std::cout << "Received " << recv_buf << " from server." << std::endl;
    }
    close(tcp_socket);
    return 0;
}