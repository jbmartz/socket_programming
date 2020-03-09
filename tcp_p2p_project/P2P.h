//
// Created by Nathan Evans.
//

#ifndef TCP_PROJECT2_P2P_H
#define TCP_PROJECT2_P2P_H

#include <stdint.h>

#define DEBUG 0
#define RECEIVE_BUF_SIZE 2048

struct Client {
    int client_socket;
    struct sockaddr_in client_address;
    socklen_t client_address_len;
    char send_buf[RECEIVE_BUF_SIZE];
    char recv_buf[RECEIVE_BUF_SIZE];


    int send_buf_bytes;
    int recv_buf_bytes;

};
/**
 * Header included with every P2P message sen   t.
 * Includes a type, and length.
 * The length includes the header and all data sent with it.
 */
struct P2PHeader {
    uint16_t type; // Type of message, one of P2PMessageTypes
    uint16_t length; // Length of full message, including this header
} __attribute__ ((packed));

enum P2PMessageTypes {
    CONTROL_MSG = 333,
    DATA_MSG,
    ERROR_MSG
};

struct ControlMessage {
    struct P2PHeader header;
    uint16_t control_type; // Type of control message, one of P2PControlTypes
} __attribute__ ((packed));

struct ErrorMessage {
    struct P2PHeader header;
    uint16_t error_type; // Type of error message, one P2PErrorTypes
} __attribute__ ((packed));

enum P2PErrorTypes {
    INCORRECT_MESSAGE_TYPE = 20,
    INCORRECT_MESSAGE_SIZE
};

enum P2PControlTypes {
    CONNECT = 1223,
    CONNECT_OK,
    DISCONNECT,
    FIND_PEERS,
    GOSSIP_PEERS
};

enum P2PDataTypes {
    FILE_LIST_REQUEST = 4321,
    FILE_LIST_RESPONSE,
    GET_FILE,
    GET_FILE_RESPONSE
};

struct DataMessage {
    struct P2PHeader header;
    uint16_t data_type; // Type of data message, one P2PDataTypes
} __attribute__ ((packed));;


struct PeerInfo {
    uint16_t peer_listen_port; // Listen port of this peer
    uint32_t ipv4_address; // ipv4 address of this peer
    uint32_t ipv6_address[4]; // ipv6 address of this peer
} __attribute__ ((packed));

/** BEGIN CONTROL MESSAGE STRUCTURES **/

/**
 * Connect message, sent from one peer to a non-connected
 * other peer in the network. Upon connecting, peers may exchange
 * data messages.
 *
 * ConnectMessage is just a convenience struct; the connect message
 * is just the ControlMessage followed by a PeerInfo
 */
struct ConnectMessage {
    struct ControlMessage control_header; // Control message header
    struct PeerInfo peer_data;
} __attribute__ ((packed));

// Disconnect message is just a ControlMessage with type DISCONNECT
// No additional struct/data is needed.

/**
 * Message sent to find new peers to connect to.
 */
struct FindPeersMessage {
    struct ControlMessage control_header; // Control message header
    uint16_t max_results; // Maximum results to return
    // if set to AF_INET, only return v4 address,
    // if set to AF_INET6 only return v6 addresses,
    // if set to 0, return both types
    uint16_t restrict_results;
}__attribute__ ((packed));

/**
 * Message sent as a response to a FIND_PEERS message with known peers.
 */
struct GossipPeersMessage {
    struct ControlMessage control_header; // Control message header
    uint16_t num_results; // Number of results returned in this message
}__attribute__ ((packed)); // num_results PeerInfo's follow this message

/** END CONTROL MESSAGE STRUCTURES **/

/** BEGIN DATA MESSAGE STRUCTURES **/

struct FileListRequest {
    struct DataMessage data_header;
    uint16_t client_request_id;
}__attribute__ ((packed)); // Actual nickname and message follow this struct

// Message from server->client containing the file list results
struct FileListResult {
    uint16_t client_request_id; // Result ID of list request
    uint16_t num_results; // Number of results (FileInfo's) that follow
} __attribute__ ((packed)); /* Fileinfo data follows, may be more than one */

// Actual data about a file, returned for a list request
struct FileInfo {
    uint16_t filename_len; // Length of filename string (in bytes, no null terminator!)
    uint32_t filesize_bytes; // Length of file, in bytes
} __attribute__ ((packed)); // Filename follows this part of the message!

/** END DATA MESSAGE STRUCTURES **/

#endif //TCP_PROJECT2_P2P_H
