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
#include <sys/select.h>
#include <vector>
#include <fcntl.h>
#include "P2P.h"
#include <utility>
#include <ctime>
#include <ratio>
#include <chrono>
#include <thread>
#include "tcp_server.h"
#include <mutex>

#define RECEIVE_BUF_SIZE 2048

/*
 * Print IP:PORT of client address to stdout.
 *
 * @param client_address
 */

const short max_peer_requested = 5;
uint32_t listen_addr = 0;
uint16_t listen_port = 0;
void print_address_details(sockaddr_in *client_address) {
    static char addrbuf[INET_ADDRSTRLEN];
    const char *ret;
    ret = inet_ntop(AF_INET, &client_address->sin_addr, addrbuf,
                    INET_ADDRSTRLEN);

    if (ret != NULL) {
        std::cout << addrbuf << ":" << ntohs(client_address->sin_port);
    }

    return;
}




void remove_peer(sockaddr_in *client_address, std::vector<PeerInfo *> &peer_list) {
    for (int i = 0; i < peer_list.size(); i++) {

        if(client_address->sin_addr.s_addr == peer_list[i]->ipv4_address && ntohs(peer_list[i]->peer_listen_port) == client_address->sin_port) {
            std::cout << "here: " << client_address->sin_port << std::endl;
            peer_list.erase(peer_list.begin() + i);
            peer_list[i] = NULL;
        }
    }
}

std::vector<struct Client *> client_list;

const int peer_lookup_interval = 30;

void find_peers() {
    std::mutex mtx;
    srand(time(NULL));
    struct FindPeersMessage *find_peers_message = (struct FindPeersMessage *) malloc(
            sizeof(struct FindPeersMessage));
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(peer_lookup_interval));
        // std::cout << "Finding peers\n";
        mtx.lock();

        if(client_list.size() == 0) {
            mtx.unlock();
            continue;

        }


        find_peers_message->control_header.control_type = htons(FIND_PEERS);
        find_peers_message->control_header.header.type = htons(CONTROL_MSG);
        find_peers_message->control_header.header.length = htons(sizeof(struct FindPeersMessage));
        find_peers_message->max_results = htons(max_peer_requested);
        find_peers_message->restrict_results = htons(AF_INET);

        int r = rand() % client_list.size();
        while (client_list[r] == NULL) {
            r = rand() % client_list.size();
        }


        memcpy(&client_list[r]->send_buf, find_peers_message, sizeof(struct FindPeersMessage));
        client_list[r]->send_buf_bytes = sizeof(struct FindPeersMessage);
        client_list[r]->recv_buf_bytes = 0;

       // free(find_peers_message);
        mtx.unlock();
//        free(find_peers_message);
//        find_peers_message = NULL;
    }
}

void send_invalid_message_size(Client *c) {
    struct ErrorMessage *error_message = (struct ErrorMessage *) malloc(sizeof(struct ErrorMessage));
    error_message->header.length = htons(sizeof(struct ErrorMessage));
    error_message->header.type = htons(ERROR_MSG);
    error_message->error_type = htons(INCORRECT_MESSAGE_SIZE);
    c->send_buf_bytes = sizeof(struct ErrorMessage);
    memcpy(&c->send_buf, error_message, sizeof(struct ConnectMessage));
}

void print_peer_info(std::vector<struct PeerInfo *> peer_list) {
//    ret = inet_ntop(AF_INET, &client_address->sin_addr, addrbuf,
//                    INET_ADDRSTRLEN);

    static char addrbuf[INET_ADDRSTRLEN];


    for (int i = 0; i < peer_list.size(); i++) {
        if (peer_list[i] == NULL)
            continue;
        inet_ntop(AF_INET, &peer_list[i]->ipv4_address, addrbuf, INET_ADDRSTRLEN);

        std::cout << "Peer " << i << " client listen address: " << addrbuf << ":" << peer_list[i]->peer_listen_port
                  << std::endl;
    }
}

/**
 *
 * TCP server example. Reads in IP PORT
 * from the command line, and accepts connections via TCP
 * on IP:PORT.
 *
 * e.g., ./tcpserver 127.0.0.1 8888
 *
 * @param argc count of arguments on the command line
 * @param argv array of command line arguments
 * @return 0 on success, non-zero if an error occurred
 */

int setup_server(char *ip_string, char *port_string, char *seed_string, char *seed_port, Client *args_client) {
    // Alias for argv[1] for convenience
    //  char *ip_string;
    // Alias for argv[2] for convenience
    // char *port_string;


    // Port to send TCP data to. Need to convert from command line string to a number
    unsigned int port;
    // The socket used to send data
    int tcp_socket;
    // Variable used to check return codes from various functions
    int ret;

    int client_socket;

    struct sockaddr_in client_address;
    socklen_t client_address_len;

    struct addrinfo hints;
    struct addrinfo *results;
    struct addrinfo *results_it;

    //list of clients connected to this node

    //set of file descriptors that can send data
    fd_set read_set;

    //set of file descriptors that we need to write data to
    fd_set write_set;

    //set of file descriptors that have an exception
    fd_set except_set;

    int max_fds;
    struct timeval tv;

    std::vector<struct PeerInfo *> peer_list;
    tv.tv_usec = 0;
    tv.tv_sec = 2;

    int timer = tv.tv_sec;


    std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
    std::thread find_peers_thread(find_peers);


    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);



    // Make sure socket was created successfully, or exit.
    if (tcp_socket == -1) {
        std::cerr << "Failed to create tcp socket!" << std::endl;
        std::cerr << strerror(errno) << std::endl;
        return 1;
    }

    ret = fcntl(tcp_socket, F_SETFL, O_NONBLOCK);

    if (ret == -1) {
        std::cerr << "Failed to set socket to be non blocking." << std::endl;
        return 1;
    }

    int opt = 1;
    setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    // What is hints?
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_addr = NULL;
    hints.ai_canonname = NULL;
    hints.ai_family = AF_INET;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;
    // Instead of using inet_pton, use getaddrinfo to convert.

    ret = getaddrinfo(ip_string, port_string, &hints, &results);

    inet_pton(AF_INET, ip_string, &listen_addr);
    listen_port = htons(std::stoi(port_string));
    if (ret != 0) {
        std::cerr << "Getaddrinfo failed with error " << ret << std::endl;
        perror("getaddrinfo");
        return 1;
    }

    // Check we have at least one result
    results_it = results;


    ret = -1;
    while (results_it != NULL) {


        //std::cout << std::endl;
        ret = bind(tcp_socket, results_it->ai_addr, results_it->ai_addrlen);
        if (ret == 0) {

            if (results_it->ai_addr->sa_family == AF_INET) {
                ntohs(((struct sockaddr_in *) results_it->ai_addr)->sin_port);
            }
            break;
        }
        perror("bind");
        results_it = results_it->ai_next;
    }

    // Whatever happened, we need to free the address list.
    freeaddrinfo(results);

    // Check if connecting succeeded at all
    if (ret != 0) {
        std::cout << "Failed to bind to any addresses!" << std::endl;
        return 1;
    }

    // Listen on the tcp socket, allow up to 50 outstanding connections
    ret = listen(tcp_socket, 50);

    // Check if connecting succeeded at all
    if (ret != 0) {
        std::cout << "Failed to listen!" << std::endl;
        close(tcp_socket);
        perror("listen");
        return 1;
    }


    if (args_client != NULL) {
        // FD_SET(new_client->client_socket, &read_set);
        client_list.push_back(args_client);

    }

    std::cout << "ENTER COMMAND: " << std::endl;

    bool quit = false;
    while (quit == false) {


        FD_ZERO(&read_set);
        FD_ZERO(&write_set);
        FD_ZERO(&except_set);
        FD_SET(0, &read_set);


        FD_SET(tcp_socket, &read_set);
        max_fds = 0;
        if (tcp_socket + 1 > max_fds) {
            max_fds = tcp_socket + 1;
        }

        //check whether clients have sent data
        for (int i = 0; i < client_list.size(); i++) {
            if (client_list[i] == NULL) {
                continue;
            }

            //if not null then client is actively connected to server

            //when we call select tell us if this particular client has sent us any data
            FD_SET(client_list[i]->client_socket, &read_set);

            if (client_list[i]->client_socket + 1 > max_fds)
                max_fds = client_list[i]->client_socket + 1;

            if (client_list[i]->send_buf_bytes > 0) {
                FD_SET(client_list[i]->client_socket, &write_set);
            }
        }

        //return number of file desctiptors ready to be read
        ret = select(max_fds, &read_set, &write_set, NULL, &tv);


        if (ret == 0) {
            continue;
        }
        //need to set up file desciptors in the read set and then check file descriptors in the read set

        if (FD_ISSET(0, &read_set)) { // Means data is available from command line
            std::string user_input;
            std::cin >> user_input;


            if (user_input == "quit") {
                quit = true;
            } else if (user_input == "LIST_PEERS") {
                if (peer_list.size() == 0)
                    std::cout << "No peers to list" << std::endl;
                else



                std::cout << "Currently have " << client_list.size() << " peers connected." << std::endl;

                for (int i = 0; i < client_list.size(); i++) {
                    if(client_list[i] == NULL)
                        continue;
                    std::cout << "Peer " << i << " client listen address: ";
                    print_address_details(&client_list[i]->client_address);
                    std::cout << std::endl;
                }



            }

            std::cout << "ENTER COMMAND: " << std::endl;



        }
        if (FD_ISSET(tcp_socket, &read_set)) {
            client_address_len = sizeof(struct sockaddr_in);
            client_socket = accept(tcp_socket, (struct sockaddr *) &client_address, &client_address_len);


            if (client_socket == -1) {
                std::cerr << "Failed to accept client!" << std::endl;
                return -1;
            }
            std::cout << "Accepted connection from : ";
            print_address_details(&client_address);
            std::cout << "\n";

            struct Client *new_client;
            new_client = (struct Client *) malloc(sizeof(struct Client));
            new_client->client_socket = client_socket;
            new_client->client_address_len = client_address_len;
            memcpy(&new_client->client_address, &client_address, client_address_len);
            new_client->recv_buf_bytes = 0;
            new_client->send_buf_bytes = 0;
            memset(new_client->recv_buf, 0, RECEIVE_BUF_SIZE);
            memset(new_client->send_buf, 0, RECEIVE_BUF_SIZE);

            client_list.push_back(new_client);


        }

        //receive data from clients
        for (int i = 0; i < client_list.size(); i++) {
            if (client_list[i] == NULL)
                continue;

            //if not null then client is actively connected to server

            //when we call select tell us if this particular client has sent us any data
            if (FD_ISSET(client_list[i]->client_socket, &read_set)) {
                ret = recv(client_list[i]->client_socket, client_list[i]->recv_buf, RECEIVE_BUF_SIZE, 0);
                if (ret == 0) { //client has disconnected
                    std::cout << "Client: ";
                    print_address_details(&client_list[i]->client_address);
                    std::cout << " has disconnected." << std::endl;
                    remove_peer(&client_list[i]->client_address, peer_list);
                    free(client_list[i]);
                    client_list.erase(client_list.begin() + i);
                    client_list[i] = NULL;
                } else if (ret == -1) {
                    free(client_list[i]);
                    client_list[i] = NULL;
                } else {
                    client_list[i]->recv_buf_bytes = ret;
                }
            }


        }

        for (int i = 0; i < client_list.size(); i++) {
            if (client_list[i] == NULL)
                continue;


            //we have data from client not yet processed
            if (client_list[i]->recv_buf_bytes > 0) {
                struct ControlMessage *control_message = (struct ControlMessage *) client_list[i]->recv_buf;
                control_message->control_type = ntohs(control_message->control_type);
                control_message->header.length = ntohs(control_message->header.length);

                //message type too small
                if (control_message->header.length < sizeof(struct P2PHeader))
                    continue;

                struct ConnectMessage *connect_message;
                        //= (struct ConnectMessage *) malloc(
                      //mm  sizeof(struct ConnectMessage));
                struct GossipPeersMessage *gossip_peers_message = (struct GossipPeersMessage *) malloc(
                        sizeof(struct GossipPeersMessage));
                struct FindPeersMessage *find_peers_message;
                PeerInfo *pi = (struct PeerInfo *) malloc(sizeof(struct PeerInfo));


                switch (control_message->control_type) {
                    //A peer tried to connect
                    case CONNECT:
                        if (control_message->header.length != sizeof(struct ConnectMessage)|| client_list[i]->recv_buf_bytes != sizeof(struct ConnectMessage)) {
                            send_invalid_message_size(client_list[i]);
                            break;
                        }
                        connect_message = (struct ConnectMessage *) client_list[i]->recv_buf;



                        memcpy(pi, &connect_message->peer_data, sizeof(struct PeerInfo));
                        if(pi->peer_listen_port > 0 && pi->ipv4_address > 0)
                            peer_list.push_back(pi);


                        connect_message->control_header.control_type = htons(CONNECT_OK);
                        connect_message->control_header.header.type = htons(CONTROL_MSG);
                        connect_message->control_header.header.length = sizeof(struct ConnectMessage);

                        inet_pton(AF_INET, ip_string, &connect_message->peer_data.ipv4_address);
                        connect_message->peer_data.peer_listen_port = htons(std::stoi(port_string));
                        client_list[i]->send_buf_bytes = sizeof(struct ConnectMessage);
                        memcpy(&client_list[i]->send_buf, connect_message, sizeof(struct ConnectMessage));
                        break;
                    case CONNECT_OK:
                        if (control_message->header.length != sizeof(struct ConnectMessage) || client_list[i]->recv_buf_bytes != sizeof(struct ConnectMessage)) {
                            send_invalid_message_size(client_list[i]);
                            break;
                        }
                        connect_message = (struct ConnectMessage *) client_list[i]->recv_buf;

                        memcpy(pi, &connect_message->peer_data, sizeof(struct PeerInfo));

                        if(pi->peer_listen_port > 0 && pi->ipv4_address > 0)
                            peer_list.push_back(pi);


                        break;
                    case FIND_PEERS: {

                        if (control_message->header.length != sizeof(struct FindPeersMessage) ||
                            client_list[i]->recv_buf_bytes != sizeof(struct FindPeersMessage)) {
                            send_invalid_message_size(client_list[i]);
                            break;
                        }
         //               std::cout << "Find peer request.\n";
                        find_peers_message = (struct FindPeersMessage *) client_list[i]->recv_buf;
                        find_peers_message->control_header.control_type = ntohs(
                                find_peers_message->control_header.control_type);
                        find_peers_message->control_header.header.type = ntohs(
                                find_peers_message->control_header.header.type);
                        find_peers_message->control_header.header.length = ntohs(
                                find_peers_message->control_header.header.length);
                        find_peers_message->max_results = ntohs(find_peers_message->max_results);
                        find_peers_message->restrict_results = ntohs(find_peers_message->restrict_results);

//                        if(find_peers_message->restrict_results != AF_INET)
//                            std::cout << "This peer only supports does not support ipv6 connections. Attempting to send ipv4 information now." << std::endl;

                        //check if we have the max number of of peers requested
                        //if we don't then send as many peers as we can
                        uint16_t num_peers = find_peers_message->max_results;

                        if (peer_list.size() == 0) {
                            std::cout << "Peer list is empty cannot gossip!" << std::endl;
                            break;

                        }

                        if (peer_list.size() < num_peers) {
                            num_peers = peer_list.size();
                        }


                        gossip_peers_message->control_header.control_type = htons(GOSSIP_PEERS);
                        gossip_peers_message->control_header.header.type = htons(CONTROL_MSG);

                        gossip_peers_message->num_results = htons(num_peers);

                        gossip_peers_message->control_header.header.length = htons(
                                sizeof(struct GossipPeersMessage) + sizeof(struct PeerInfo) * num_peers);

                        client_list[i]->send_buf_bytes =
                                sizeof(struct GossipPeersMessage) + sizeof(struct PeerInfo) * num_peers;


                        memcpy(&client_list[i]->send_buf, gossip_peers_message, sizeof(struct GossipPeersMessage));


                        for (int j = 0; j < num_peers; j++) {

                            int offset = sizeof(struct GossipPeersMessage) +
                                         sizeof(struct PeerInfo) * j;


                            memcpy(&client_list[i]->send_buf[offset], peer_list[j], sizeof(struct PeerInfo));
                        }


                    }
                        break;

                    case GOSSIP_PEERS: {
                        int max_peer_request_size =
                                sizeof(struct GossipPeersMessage) + sizeof(struct PeerInfo) * max_peer_requested;
                        if (control_message->header.length > max_peer_request_size ||
                            control_message->header.length < sizeof(struct GossipPeersMessage) ||
                            client_list[i]->recv_buf_bytes > max_peer_request_size ||
                            client_list[i]->recv_buf_bytes < sizeof(struct GossipPeersMessage)) {
                            send_invalid_message_size(client_list[i]);
                            break;
                        }


                        gossip_peers_message = (struct GossipPeersMessage *) client_list[i]->recv_buf;
                        gossip_peers_message->control_header.control_type = ntohs(
                                gossip_peers_message->control_header.control_type);
                        gossip_peers_message->control_header.header.type = ntohs(
                                gossip_peers_message->control_header.header.type);
                        gossip_peers_message->control_header.header.length = ntohs(
                                gossip_peers_message->control_header.header.length);
                        gossip_peers_message->num_results = ntohs(gossip_peers_message->num_results);

                        for (int j = 0; j < gossip_peers_message->num_results; j++) {
                            struct PeerInfo *incoming_pi = (struct PeerInfo *) (client_list[i]->recv_buf +
                                                                                sizeof(GossipPeersMessage) +
                                                                                sizeof(struct PeerInfo) * j);

                            //if peer already exists in our list then don't add it again
                            //should use a hashtable or something so this can be done quicker

                            bool peer_exists = false;
                            for (int k = 0; k < peer_list.size(); k++) {
                                if (peer_list[k]->ipv4_address == incoming_pi->ipv4_address &&
                                    peer_list[k]->peer_listen_port == incoming_pi->peer_listen_port) {
                                    peer_exists = true;
                                    break;
                                }
                            }

                            if (peer_exists || (incoming_pi->ipv4_address == listen_addr &&
                                                incoming_pi->peer_listen_port == listen_port))
                                continue;


                            bool client_already_connected = false;
                            for(int i = 0; i < client_list.size(); i++) {
                                if(client_list[i]->client_address.sin_port == incoming_pi->peer_listen_port && client_list[i]->client_address.sin_addr.s_addr == incoming_pi->ipv4_address) {
                                    client_already_connected = true;
                                }
                            }

                            if(client_already_connected)
                                continue;

                            //build new client to connect to
                            struct Client *client_from_peer;
                            client_from_peer = (struct Client *) malloc(sizeof(struct Client));
                            client_from_peer->client_socket = socket(AF_INET, SOCK_STREAM, 0);
                            client_from_peer->client_address_len = sizeof(struct sockaddr_in);
                            client_from_peer->recv_buf_bytes = 0;
                            client_from_peer->send_buf_bytes = sizeof(struct ConnectMessage);

                            struct sockaddr_in remoteaddr;
                            remoteaddr.sin_family = AF_INET;
                            remoteaddr.sin_addr.s_addr = incoming_pi->ipv4_address;
                            remoteaddr.sin_port = incoming_pi->peer_listen_port;


                            ret = connect(client_from_peer->client_socket, (struct sockaddr *) &remoteaddr, sizeof(struct sockaddr_in));
                            if(ret == 0) {
                                std::cout << "Connection to gossiped peer successful" << std::endl;
                            } else {
                                std::cout << "Unable to connect to gossipped peer because peer is either offline or we are already connected." << std::endl;
                                continue;
                            }

                            if(incoming_pi->peer_listen_port > 0 && incoming_pi->ipv4_address > 0)
                                peer_list.push_back(incoming_pi);

                            memcpy(&client_from_peer->client_address, &remoteaddr, sizeof(sockaddr_in));

                            struct ConnectMessage *cm = (struct ConnectMessage *) malloc(sizeof(struct ConnectMessage));
                            cm->control_header.header.type = htons(CONTROL_MSG);
                            cm->control_header.header.length = htons(sizeof(struct ConnectMessage));
                            cm->control_header.control_type = htons(CONNECT);
                            memset(&client_from_peer->recv_buf, 0, RECEIVE_BUF_SIZE);

                            //copy in our listening address / port
                            inet_pton(AF_INET, ip_string, &cm->peer_data.ipv4_address);
                            cm->peer_data.peer_listen_port = htons(std::stoi(port_string));

                            //copy our connect message into the buffer
                            memcpy(&client_from_peer->send_buf, cm, sizeof(struct ConnectMessage));


                            //set the write set and add
                            //      FD_SET(client_from_peer->client_socket, &write_set);
                            client_list.push_back(client_from_peer);
                           // free(cm);
                        }
                        break;


                    }
                    default:
                        //This gets annoying after a while but uncomment if you want
//                        if(control_message->control_type == INCORRECT_MESSAGE_SIZE)
//                            std::cout << "Incorrect message size received, mostly likely because ipv6 addresses were requested in a find peer request and the recipient was expecting a message size that included ipv6 addresses." << std::endl;
//                        else
//                            std::cout << "Can't handle this control type: " << control_message->control_type << std::endl;
                        break;

                }

//                free(find_peers_message);
//                find_peers_message = NULL;
//                free(gossip_peers_message);
//                gossip_peers_message = NULL;
//                free(pi);
//                pi = NULL;
                client_list[i]->recv_buf_bytes = 0;
            }


        }

        for (int i = 0; i < client_list.size(); i++) {
            if (client_list[i] == NULL)
                continue;

            //if not null then client is actively connected to server

            //when we call select tell us if this particular client has sent us any data
            if (FD_ISSET(client_list[i]->client_socket, &write_set)) {
                ret = send(client_list[i]->client_socket, client_list[i]->send_buf, client_list[i]->send_buf_bytes, 0);

                if (ret == client_list[i]->send_buf_bytes) {
                    client_list[i]->send_buf_bytes = 0;
                } else
                    std::cerr << "Failed to send enough bytes to client.\n";
            }


        }

        //  std::cout << "Peer Info with size " << peer_list.size() << ": " << std::endl;



    }


      for (int i = 0; i < client_list.size(); i++) {
          if(client_list[i] != NULL) {
               close(client_list[i]->client_socket);
                free(client_list[i]);
                client_list[i] = NULL;
          }

        }

        client_list.clear();
        close(tcp_socket);
        return 0;


}