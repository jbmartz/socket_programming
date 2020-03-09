//
// Created by Justin Martz on 2019-11-08.
//
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
#include "main.h"
#include "P2P.h"
#include "tcp_server.h"


int main(int argc, char *argv[]) {
    if (argc < 3 || argc == 4 || argc > 5) {
        std::cerr << "Please specify SERVER_IP SERVER_PORT as first two args and SEED_IP SEED_PORT as optional third and fourth args." << std::endl;
        return 1;
    }

    if (argc == 3)
           setup_server(argv[1], argv[2], 0, 0, NULL);
    else if(argc == 5){



        // Alias for argv[1] for convenience
        char *ip_string;
        // Alias for argv[2] for convenience
        char *port_string;
        // Alias for argv[3] for convenience
        char *data_string;

        char *SERVER_HOST_IP = argv[1];
        char *SERVER_PORT = argv[2];

        char *SEED_HOST_IP = argv[3];
        char *SEED_PORT = argv[4];
        // Port to send TCP data to. Need to convert from command line string to a number
        unsigned int port;
        // The socket used to send data
        int tcp_socket;
        // Variable used to check return codes from various functions
        int ret;

       // struct Client *client;



        struct addrinfo hints;
        struct addrinfo *results;
        struct addrinfo *results_it;

        // Note: this needs to be 4, because the program name counts as an argument!

        // Set up variables "aliases"



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
        ret = getaddrinfo(SEED_HOST_IP, SEED_PORT, &hints, &results);

        if (ret != 0) {
            std::cerr << "Getaddrinfo failed with error " << ret << std::endl;
            return 1;
        }

        // Check we have at least one result
        results_it = results;

        sockaddr_in *client_addr = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
        while (results_it != NULL) {
            ret = connect(tcp_socket, results_it->ai_addr, results_it->ai_addrlen);
            if (ret == 0) {
                memcpy(client_addr, results_it->ai_addr, results_it->ai_addrlen);
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

        // Structure to fill in with connect message info
        struct ConnectMessage connect_message;
        // Zero out structure
        memset(&connect_message, 0, sizeof(struct ConnectMessage));
        connect_message.control_header.header.type = htons(CONTROL_MSG);
        connect_message.control_header.header.length = htons(sizeof(struct ConnectMessage));
        connect_message.control_header.control_type = htons(CONNECT);


        inet_pton(AF_INET, SERVER_HOST_IP, &connect_message.peer_data.ipv4_address);
        connect_message.peer_data.peer_listen_port = htons(std::stoi(SERVER_PORT));


        // If we get here, the connect worked. Now send the data.
        ret = send(tcp_socket, &connect_message, sizeof(struct ConnectMessage), 0); // Replace with call to send

        // Check if send worked, clean up and exit if not.
        if (ret == -1) {
            std::cerr << "Failed to send data!" << std::endl;
            perror("send");
            std::cerr << strerror(errno) << std::endl;
            close(tcp_socket);
            return 1;
        }



        struct Client *new_client;
<<<<<<< HEAD
        new_client = (struct Client *) malloc(sizeof(struct Client));
        memset(new_client, 0, sizeof(struct Client));
=======
	new_client = (struct Client *) malloc(sizeof(struct Client));
	memset(new_client, 0, sizeof(struct Client));
>>>>>>> 2070b7d8defc684223d5e1d1baaaa51a9467bcf6
        new_client->client_socket = tcp_socket;
        new_client->client_address_len = sizeof(struct sockaddr_in);
      //  memset(&new_client->client_address, 0, sizeof(struct sockaddr_in));
        memcpy(&new_client->client_address, client_addr, sizeof(sockaddr_in));
        new_client->recv_buf_bytes = sizeof(struct ConnectMessage);
      //  memcpy(&new_client->recv_buf, response_message, sizeof(struct ConnectMessage));
<<<<<<< HEAD
//         memset(&new_client->recv_buf, 0, RECEIVE_BUF_SIZE);
//
//        memset(&new_client->send_buf_bytes, 0, RECEIVE_BUF_SIZE);
//        memset(&new_client->send_buf, 0, RECEIVE_BUF_SIZE);
=======
        // memset(&new_client->recv_buf, 0, RECEIVE_BUF_SIZE);
//new_client->recv_buf = 0;
//        memset(&new_client->send_buf_bytes, 0, RECEIVE_BUF_SIZE);
  //      memset(&new_client->send_buf, 0, RECEIVE_BUF_SIZE);
>>>>>>> 2070b7d8defc684223d5e1d1baaaa51a9467bcf6
////pass client list here
        setup_server(SERVER_HOST_IP, SERVER_PORT, SEED_HOST_IP, SEED_PORT, new_client);
    //    close(tcp_socket);
        return 0;

    }

}
