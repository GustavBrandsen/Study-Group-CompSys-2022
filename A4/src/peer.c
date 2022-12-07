#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>

#ifdef __APPLE__
#include "./endian.h"
#else
#include <endian.h>
#endif

#include "./peer.h"
#include "./sha256.h"


// Global variables to be used by both the server and client side of the peer.
// Some of these are not currently used but should be considered STRONG hints
PeerAddress_t *my_address;

pthread_mutex_t network_mutex = PTHREAD_MUTEX_INITIALIZER;
PeerAddress_t **network = NULL;
uint32_t peer_count = 0;

pthread_mutex_t retrieving_mutex = PTHREAD_MUTEX_INITIALIZER;
FilePath_t **retrieving_files = NULL;
uint32_t file_count = 0;


/*
 * Gets a sha256 hash of specified data, sourcedata. The hash itself is
 * placed into the given variable 'hash'. Any size can be created, but a
 * a normal size for the hash would be given by the global variable
 * 'SHA256_HASH_SIZE', that has been defined in sha256.h
 */
void get_data_sha(const char *sourcedata, hashdata_t hash, uint32_t data_size,
    int hash_size) {
    SHA256_CTX shactx;
    unsigned char shabuffer[hash_size];
    sha256_init(&shactx);
    sha256_update(&shactx, sourcedata, data_size);
    sha256_final(&shactx, shabuffer);

    for (int i = 0; i < hash_size; i++)
    {
        hash[i] = shabuffer[i];
    }
}

/*
 * Gets a sha256 hash of specified data file, sourcefile. The hash itself is
 * placed into the given variable 'hash'. Any size can be created, but a
 * a normal size for the hash would be given by the global variable
 * 'SHA256_HASH_SIZE', that has been defined in sha256.h
 */
void get_file_sha(const char *sourcefile, hashdata_t hash, int size) {
    int casc_file_size;

    FILE *fp = Fopen(sourcefile, "rb");
    if (fp == 0)
    {
        printf("Failed to open source: %s\n", sourcefile);
        return;
    }

    fseek(fp, 0L, SEEK_END);
    casc_file_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char buffer[casc_file_size];
    Fread(buffer, casc_file_size, 1, fp);
    Fclose(fp);

    get_data_sha(buffer, hash, casc_file_size, size);
}

/*
 * A simple min function, which apparently C doesn't have as standard
 */
uint32_t min(int a, int b) {
    if (a < b)
    {
        return a;
    }
    return b;
}

/*
 * Select a peer from the network at random, without picking the peer defined
 * in my_address
 */
void get_random_peer(PeerAddress_t *peer_address) {
    PeerAddress_t **potential_peers = Malloc(sizeof(PeerAddress_t *));
    uint32_t potential_count = 0;
    for (uint32_t i = 0; i < peer_count; i++)
    {
        if (strcmp(network[i]->ip, my_address->ip) != 0
            || strcmp(network[i]->port, my_address->port) != 0)
        {
            potential_peers = realloc(potential_peers,
                (potential_count + 1) * sizeof(PeerAddress_t *));
            potential_peers[potential_count] = network[i];
            potential_count++;
        }
    }

    if (potential_count == 0)
    {
        printf("No peers to connect to. You probably have not implemented "
            "registering with the network yet.\n");
    }

    uint32_t random_peer_index = rand() % potential_count;

    memcpy(peer_address->ip, potential_peers[random_peer_index]->ip, IP_LEN);
    memcpy(peer_address->port, potential_peers[random_peer_index]->port,
        PORT_LEN);

    Free(potential_peers);

    printf("Selected random peer: %s:%s\n",
        peer_address->ip, peer_address->port);
}

/*
 * Send a request message to another peer on the network. Unless this is
 * specifically an 'inform' message as described in the assignment handout, a
 * reply will always be expected.
 */
void send_message(PeerAddress_t peer_address, int command, char *request_body) {
    fprintf(stdout, "Connecting to server at %s:%s to run command %d (%s)\n",
        peer_address.ip, peer_address.port, command, request_body);

    rio_t rio;
    char msg_buf[MAX_MSG_LEN];
    FILE *fp;

    // Setup the eventual output file path. This is being done early so if 
    // something does go wrong at this stage we can avoid all that pesky 
    // networking
    char output_file_path[strlen(request_body) + 1];
    if (command == COMMAND_RETREIVE)
    {
        strcpy(output_file_path, request_body);

        if (access(output_file_path, F_OK) != 0)
        {
            fp = Fopen(output_file_path, "a");
            Fclose(fp);
        }
    }

    // Setup connection
    int peer_socket = Open_clientfd(peer_address.ip, peer_address.port);
    Rio_readinitb(&rio, peer_socket);

    // Construct a request message and send it to the peer
    struct RequestHeader request_header;
    strncpy(request_header.ip, my_address->ip, IP_LEN);
    request_header.port = htonl(atoi(my_address->port));
    request_header.command = htonl(command);
    request_header.length = htonl(strlen(request_body));

    memcpy(msg_buf, &request_header, REQUEST_HEADER_LEN);
    memcpy(msg_buf + REQUEST_HEADER_LEN, request_body, strlen(request_body));

    Rio_writen(peer_socket, msg_buf, REQUEST_HEADER_LEN + strlen(request_body));

    // We don't expect replies to inform messages so we're done here
    if (command == COMMAND_INFORM)
    {
        return;
    }

    // Read a reply
    Rio_readnb(&rio, msg_buf, REPLY_HEADER_LEN);

    // Extract the reply header 
    char reply_header[REPLY_HEADER_LEN];
    memcpy(reply_header, msg_buf, REPLY_HEADER_LEN);

    uint32_t reply_length = ntohl(*(uint32_t *)&reply_header[0]);
    uint32_t reply_status = ntohl(*(uint32_t *)&reply_header[4]);
    uint32_t this_block = ntohl(*(uint32_t *)&reply_header[8]);
    uint32_t block_count = ntohl(*(uint32_t *)&reply_header[12]);
    hashdata_t block_hash;
    memcpy(block_hash, &reply_header[16], SHA256_HASH_SIZE);
    hashdata_t total_hash;
    memcpy(total_hash, &reply_header[48], SHA256_HASH_SIZE);

    // Determine how many blocks we are about to recieve
    hashdata_t ref_hash;
    memcpy(ref_hash, &total_hash, SHA256_HASH_SIZE);
    uint32_t ref_count = block_count;

    // Loop until all blocks have been recieved
    for (uint32_t b = 0; b < ref_count; b++)
    {
        // Don't need to re-read the first block
        if (b > 0)
        {
            // Read the response
            Rio_readnb(&rio, msg_buf, REPLY_HEADER_LEN);

            // Read header
            memcpy(reply_header, msg_buf, REPLY_HEADER_LEN);

            // Parse the attributes
            reply_length = ntohl(*(uint32_t *)&reply_header[0]);
            reply_status = ntohl(*(uint32_t *)&reply_header[4]);
            this_block = ntohl(*(uint32_t *)&reply_header[8]);
            block_count = ntohl(*(uint32_t *)&reply_header[12]);

            memcpy(block_hash, &reply_header[16], SHA256_HASH_SIZE);
            memcpy(total_hash, &reply_header[48], SHA256_HASH_SIZE);

            // Check we're getting consistent results
            if (ref_count != block_count)
            {
                fprintf(stdout,
                    "Got inconsistent block counts between blocks\n");
                Close(peer_socket);
                return;
            }

            for (int i = 0; i < SHA256_HASH_SIZE; i++)
            {
                if (ref_hash[i] != total_hash[i])
                {
                    fprintf(stdout,
                        "Got inconsistent total hashes between blocks\n");
                    Close(peer_socket);
                    return;
                }
            }
        }

        // Check response status
        if (reply_status != STATUS_OK)
        {
            if (command == COMMAND_REGISTER && reply_status == STATUS_PEER_EXISTS)
            {
                printf("Peer already exists\n");
            } else
            {
                printf("Got unexpected status %d\n", reply_status);
                Close(peer_socket);
                return;
            }
        }

        // Read the payload
        char payload[reply_length + 1];
        Rio_readnb(&rio, msg_buf, reply_length);
        memcpy(payload, msg_buf, reply_length);
        payload[reply_length] = '\0';

        // Check the hash of the data is as expected
        hashdata_t payload_hash;
        get_data_sha(payload, payload_hash, reply_length, SHA256_HASH_SIZE);

        for (int i = 0; i < SHA256_HASH_SIZE; i++)
        {
            if (payload_hash[i] != block_hash[i])
            {
                fprintf(stdout, "Payload hash does not match specified\n");
                Close(peer_socket);
                return;
            }
        }

        // If we're trying to get a file, actually write that file
        if (command == COMMAND_RETREIVE)
        {
            // Check we can access the output file
            fp = Fopen(output_file_path, "r+b");
            if (fp == 0)
            {
                printf("Failed to open destination: %s\n", output_file_path);
                Close(peer_socket);
            }

            uint32_t offset = this_block * (MAX_MSG_LEN - REPLY_HEADER_LEN);
            fprintf(stdout, "Block num: %d/%d (offset: %d)\n", this_block + 1,
                block_count, offset);
            fprintf(stdout, "Writing from %d to %d\n", offset,
                offset + reply_length);

            // Write data to the output file, at the appropriate place
            fseek(fp, offset, SEEK_SET);
            Fputs(payload, fp);
            Fclose(fp);
        }
    }

    // Confirm that our file is indeed correct
    if (command == COMMAND_RETREIVE)
    {
        fprintf(stdout, "Got data and wrote to %s\n", output_file_path);

        // Finally, check that the hash of all the data is as expected
        hashdata_t file_hash;
        get_file_sha(output_file_path, file_hash, SHA256_HASH_SIZE);

        for (int i = 0; i < SHA256_HASH_SIZE; i++)
        {
            if (file_hash[i] != total_hash[i])
            {
                fprintf(stdout, "File hash does not match specified for %s\n",
                    output_file_path);
                Close(peer_socket);
                return;
            }
        }
    }

    // If we are registering with the network we should note the complete 
    // network reply
    char *reply_body = Malloc(reply_length + 1);
    memset(reply_body, 0, reply_length + 1);
    memcpy(reply_body, msg_buf, reply_length);

    if (reply_status == STATUS_OK)
    {
        if (command == COMMAND_REGISTER)
        {
            // Your code here. This code has been added as a guide, but feel 
            // free to add more, or work in other parts of the code
            uint32_t reply_count = (uint32_t)(reply_length / 20);

            for (uint32_t i = 0; i < reply_count; i++)
            {
                PeerAddress_t *reply_address = (PeerAddress_t *)malloc(sizeof(PeerAddress_t));
                memset(reply_address->ip, '\0', IP_LEN);
                memset(reply_address->port, '\0', PORT_LEN);
                memcpy(reply_address->ip, reply_body + i * 20, IP_LEN);
                memcpy(reply_address->port, reply_body + i * 20 + 16, PORT_LEN);

                char *curr_ip = reply_address->ip;
                int curr_port_temp = ntohl(*(uint32_t *)&reply_address->port);
                char curr_port[PORT_LEN];
                sprintf(curr_port, "%d", curr_port_temp);

                memcpy(reply_address->port, curr_port, PORT_LEN);

                int in_network = 0;
                for (uint32_t j = 0; j < peer_count; j++)
                {
                    if (strcmp(reply_address->ip, network[j]->ip) == 0 && strcmp(reply_address->port, network[j]->port) == 0)
                    {
                        printf("IP already registered\n\n");
                        in_network = 1;
                        break;
                    }
                }
                if (in_network == 0)
                {
                    printf("IP registered\n\n");
                    peer_count++;
                    network = realloc(network, sizeof(PeerAddress_t) * peer_count);
                    network[peer_count - 1] = reply_address;
                }
            }

            printf("\nNETWORK LIST: \n");
            for (uint32_t i = 0; i < peer_count; i++)
            {
                printf("ip: %s, port: %s \n", network[i]->ip, network[i]->port);
            }
            printf("\n");

        }
    } else
    {
        printf("Got response code: %d, %s\n", reply_status, reply_body);
    }
    Free(reply_body);
    Close(peer_socket);
}


/*
 * Function to act as thread for all required client interactions. This thread
 * will be run concurrently with the server_thread but is finite in nature.
 *
 * This is just to register with a network, then download two files from a
 * random peer on that network. As in A3, you are allowed to use a more
 * user-friendly setup with user interaction for what files to retrieve if
 * preferred, this is merely presented as a convienient setup for meeting the
 * assignment tasks
 */
void *client_thread(void *thread_args) {
    struct PeerAddress *peer_address = thread_args;

    // Register the given user
    send_message(*peer_address, COMMAND_REGISTER, "\0");

    // Update peer_address with random peer from network
    get_random_peer(peer_address);

    // Retrieve the smaller file, that doesn't not require support for blocks
    send_message(*peer_address, COMMAND_RETREIVE, "tiny.txt");

    // Update peer_address with random peer from network
    get_random_peer(peer_address);

    // Retrieve the larger file, that requires support for blocked messages
    send_message(*peer_address, COMMAND_RETREIVE, "hamlet.txt");
    send_message(*peer_address, COMMAND_RETREIVE, "test.txt");

    return NULL;
}

/*
 * Handle any 'register' type requests, as defined in the asignment text. This
 * should always generate a response.
 */
void handle_register(int connfd, char *client_ip, int client_port_int) {
    // Your code here. This function has been added as a guide, but feel free 
    // to add more, or work in other parts of the code

    int in_network = 0;
    PeerAddress_t *register_address = (PeerAddress_t *)malloc(sizeof(PeerAddress_t));
    int curr_port_temp = ntohl(*(uint32_t *)&register_address->port);
    char curr_port[PORT_LEN];
    sprintf(curr_port, "%d", curr_port_temp);
    memcpy(register_address->ip, client_ip, IP_LEN);
    memcpy(register_address->port, curr_port_temp, PORT_LEN);

    for (uint32_t j = 0; j < peer_count; j++)
    {
        if (strcmp(register_address->ip, network[j]->ip) == 0 && strcmp(register_address->port, network[j]->port) == 0)
        {
            in_network = 1;
            break;
        }
    }
    if (in_network == 0)
    {
        printf("IP registered\n\n");
        peer_count++;
        network = realloc(network, sizeof(PeerAddress_t) * peer_count);
        network[peer_count - 1] = register_address;

        // Concat ip and port as string
        char *str_register_address = strcat(client_ip, curr_port);
        // Inform all peers about new peer joining the network.
        for (uint32_t i = 0; i < peer_count; i++)
        {
            send_message(*network[i], COMMAND_INFORM, str_register_address);
        }
    }




}

/*
 * Handle 'inform' type message as defined by the assignment text. These will
 * never generate a response, even in the case of errors.
 */
 // Indsæt i network listen
void handle_inform(char *request) {
    // Your code here. This function has been added as a guide, but feel free 
    // to add more, or work in other parts of the code
    PeerAddress_t *retrieve_address = malloc(sizeof(PeerAddress_t));
    memcpy(retrieve_address->ip, request, IP_LEN);
    memcpy(retrieve_address->port, request + IP_LEN, PORT_LEN);

    int in_network = 0;
    for (uint32_t j = 0; j < peer_count; j++)
    {
        if (strcmp(retrieve_address->ip, network[j]->ip) == 0 && strcmp(retrieve_address->port, network[j]->port) == 0)
        {
            in_network = 1;
            break;
        }
    }
    if (in_network == 0)
    {
        peer_count++;
        network = realloc(network, sizeof(PeerAddress_t) * peer_count);
        network[peer_count - 1] = retrieve_address;
    }
}

/*
 * Handle 'retrieve' type messages as defined by the assignment text. This will
 * always generate a response
 */
void handle_retreive(int connfd, char *request) {
    // Your code here. This function has been added as a guide, but feel free 
    // to add more, or work in other parts of the code
    PeerAddress_t *retrieve_address = malloc(sizeof(PeerAddress_t));
    memcpy(retrieve_address->ip, request, IP_LEN);
    memcpy(retrieve_address->port, request + IP_LEN, PORT_LEN);
    char *path[MAX_MSG_LEN];
    memcpy(path, request + IP_LEN + PORT_LEN, MAX_MSG_LEN);
    send_message(*retrieve_address, COMMAND_RETREIVE, path);
}

/*
 * Handler for all server requests. This will call the relevent function based
 * on the parsed command code
 */
void handle_server_request(int connfd) {
    // Your code here. This function has been added as a guide, but feel free 
    // to add more, or work in other parts of the code
    rio_t rio;
    char buf[MAX_MSG_LEN];
    rio_readnb(&rio, buf, MAX_MSG_LEN);

    Request_t *request = malloc(sizeof(Request_t));
    //RequestHeader_t *req_header = malloc(sizeof(RequestHeader_t));
    printf("handle_server_request");
    memcpy(request->requestHeader.ip, buf, IP_LEN);
    memcpy(request->requestHeader.port, buf + IP_LEN, PORT_LEN);
    memcpy(request->requestHeader.command, buf + IP_LEN + PORT_LEN, PORT_LEN);
    memcpy(request->requestHeader.length, buf + IP_LEN + PORT_LEN + PORT_LEN, PORT_LEN);
    memcpy(request->payload, buf + IP_LEN + PORT_LEN + PORT_LEN + PORT_LEN, MAX_MSG_LEN);

    switch (request->requestHeader.command)
    {
        case COMMAND_REGISTER:
            handle_register(connfd, request->requestHeader.ip, request->requestHeader.port);
            break;
        case COMMAND_RETREIVE:
            handle_retreive(connfd, request);
            break;
        case COMMAND_INFORM:
            handle_inform(request);
            break;
        default:
            break;
    }

}

/*
 * Function to act as basis for running the server thread. This thread will be
 * run concurrently with the client thread, but is infinite in nature.
 */
void *server_thread() {
    // Your code here. This function has been added as a guide, but feel free 
    // to add more, or work in other parts of the code
    // THIS IS TAKEN FROM ECHO SERVER EXERCISE AND MODIFYED
    printf("Starting server at: %s:%s \n", my_address->ip, my_address->port);
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    char client_hostname[MAXLINE], client_port[MAXLINE];

    listenfd = Open_listenfd(my_address->port);
    while (1) {
        clientlen = sizeof(struct sockaddr_storage);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        if (Fork() == 0) {
            Close(listenfd); /* Child closes its listening socket */
            pid_t childpid = getpid();
            Getnameinfo((SA *)&clientaddr, clientlen, client_hostname, MAXLINE,
                client_port, MAXLINE, 0);
            //printf("Connected to (%s, %s) via pid %d\n", client_hostname, client_port, childpid);
            handle_server_request(connfd);
            //echo(connfd);    /* Child services client */ //line:conc:echoserverp:echofun
            Close(connfd);   /* Child closes connection with client */ //line:conc:echoserverp:childclose
            exit(0);         /* Child exits */
        }
        Close(connfd); /* Parent closes connected socket (important!) */ //line:conc:echoserverp:parentclose
    }

}


int main(int argc, char **argv) {
    // Initialise with known junk values, so we can test if these were actually
    // present in the config or not
    struct PeerAddress peer_address;
    memset(peer_address.ip, '\0', IP_LEN);
    memset(peer_address.port, '\0', PORT_LEN);
    memcpy(peer_address.ip, "x", 1);
    memcpy(peer_address.port, "x", 1);

    // Users should call this script with a single argument describing what 
    // config to use
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <config file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    my_address = (PeerAddress_t *)Malloc(sizeof(PeerAddress_t));
    memset(my_address->ip, '\0', IP_LEN);
    memset(my_address->port, '\0', PORT_LEN);

    // Read in configuration options. Should include a client_ip, client_port, 
    // server_ip, and server_port
    char buffer[128];
    fprintf(stderr, "Got config path at: %s\n", argv[1]);
    FILE *fp = Fopen(argv[1], "r");
    while (fgets(buffer, 128, fp)) {
        if (starts_with(buffer, MY_IP)) {
            memcpy(&my_address->ip, &buffer[strlen(MY_IP)],
                strcspn(buffer, "\r\n") - strlen(MY_IP));
            if (!is_valid_ip(my_address->ip)) {
                fprintf(stderr, ">> Invalid client IP: %s\n", my_address->ip);
                exit(EXIT_FAILURE);
            }
        } else if (starts_with(buffer, MY_PORT)) {
            memcpy(&my_address->port, &buffer[strlen(MY_PORT)],
                strcspn(buffer, "\r\n") - strlen(MY_PORT));
            if (!is_valid_port(my_address->port)) {
                fprintf(stderr, ">> Invalid client port: %s\n",
                    my_address->port);
                exit(EXIT_FAILURE);
            }
        } else if (starts_with(buffer, PEER_IP)) {
            memcpy(peer_address.ip, &buffer[strlen(PEER_IP)],
                strcspn(buffer, "\r\n") - strlen(PEER_IP));
            if (!is_valid_ip(peer_address.ip)) {
                fprintf(stderr, ">> Invalid peer IP: %s\n", peer_address.ip);
                exit(EXIT_FAILURE);
            }
        } else if (starts_with(buffer, PEER_PORT)) {
            memcpy(peer_address.port, &buffer[strlen(PEER_PORT)],
                strcspn(buffer, "\r\n") - strlen(PEER_PORT));
            if (!is_valid_port(peer_address.port)) {
                fprintf(stderr, ">> Invalid peer port: %s\n",
                    peer_address.port);
                exit(EXIT_FAILURE);
            }
        }
    }
    fclose(fp);

    retrieving_files = Malloc(file_count * sizeof(FilePath_t *));
    srand(time(0));

    network = Malloc(sizeof(PeerAddress_t *));
    network[0] = my_address;
    peer_count = 1;

    // Setup the client and server threads 
    pthread_t client_thread_id;
    pthread_t server_thread_id;
    if (peer_address.ip[0] != 'x' && peer_address.port[0] != 'x')
    {
        pthread_create(&client_thread_id, NULL, client_thread, &peer_address);
    }
    pthread_create(&server_thread_id, NULL, server_thread, NULL);

    // Start the threads. Note that the client is only started if a peer is 
    // provided in the config. If none is we will assume this peer is the first
    // on the network and so cannot act as a client.
    if (peer_address.ip[0] != 'x' && peer_address.port[0] != 'x')
    {
        Pthread_join(client_thread_id, NULL);
    }
    Pthread_join(server_thread_id, NULL);

    exit(EXIT_SUCCESS);
}
