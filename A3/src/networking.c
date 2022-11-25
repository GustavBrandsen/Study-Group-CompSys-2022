#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#ifdef __APPLE__
#include "./endian.h"
#else
#include <endian.h>
#endif

#include "./networking.h"
#include "./sha256.h"

char server_ip[IP_LEN];
char server_port[PORT_LEN];
char my_ip[IP_LEN];
char my_port[PORT_LEN];

int c;

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
 * Combine a password and salt together and hash the result to form the
 * 'signature'. The result should be written to the 'hash' variable. Note that
 * as handed out, this function is never called. You will need to decide where
 * it is sensible to do so.
 */
void get_signature(char *password, char *salt, hashdata_t *hash) {
    char to_hash[(PASSWORD_LEN + SALT_LEN) + 1];

    memcpy(&to_hash[0], password, PASSWORD_LEN);

    memcpy(&to_hash[PASSWORD_LEN], salt, SALT_LEN);
    to_hash[(PASSWORD_LEN + SALT_LEN)] = '\0';

    get_data_sha(to_hash, (uint8_t *)hash, strlen(to_hash), SHA256_HASH_SIZE);
    
    

    // TODO Put some code in here so that to_hash contains the password and
    // salt and is then hashed

    // Your code here. This function has been added as a guide, but feel free
    // to add more, or work in other parts of the code
}

/*
 * Register a new user with a server by sending the username and signature to
 * the server
 */
void register_user(char *username, char *password, char *salt) {
    hashdata_t signature;
    get_signature(password, salt, &signature);
   

    int clientfd;
    char *host, *port, buf[MAXLINE];
    rio_t rio;

    host = server_ip;
    port = server_port;

    clientfd = Open_clientfd(host, port);
    Rio_readinitb(&rio, clientfd);
    RequestHeader_t reqHeader;
    memcpy(reqHeader.username, username, USERNAME_LEN);
    memcpy(reqHeader.salted_and_hashed, signature, SHA256_HASH_SIZE);
    reqHeader.length = 0;

    memcpy(buf, &reqHeader, REQUEST_HEADER_LEN);

    Rio_writen(clientfd, &reqHeader, REQUEST_HEADER_LEN);

    struct ResponseHeader responseHeader;
    Rio_readnb(&rio, &responseHeader, RESPONSE_HEADER_LEN);

    responseHeader.length = ntohl(*(uint32_t *)&responseHeader.length);
    responseHeader.statusCode = ntohl(*(uint32_t *)&responseHeader.statusCode);
    responseHeader.blockNumber = ntohl(*(uint32_t *)&responseHeader.blockNumber);
    responseHeader.block_count = ntohl(*(uint32_t *)&responseHeader.block_count);
    struct Response response;
    Rio_readnb(&rio, &response.payload, responseHeader.length);

    
    response.payload[responseHeader.length] = '\0';
    Fputs(response.payload, stdout);
    fflush(stdout);
    printf("\n");

    hashdata_t localBlockHash;
    get_data_sha(&response.payload, &localBlockHash, responseHeader.length, SHA256_HASH_SIZE);
    for (int i = 0; i < SHA256_HASH_SIZE; i++) {
        if (localBlockHash[i] != responseHeader.blockHash[i]) {
            printf("Signature packet is not hashed correctly");
            break;
        }
    }


    if (responseHeader.statusCode == 1) {

    } else {
        printf("Error statuscode");
    }
    
    // Your code here. This function has been added as a guide, but feel free
    // to add more, or work in other parts of the code
}

/*
 * Get a file from the server by sending the username and signature, along with
 * a file path. Note that this function should be able to deal with both small
 * and large files.
 */
void get_file(char *username, char *password, char *salt, char *to_get) {
    hashdata_t signature;
    get_signature(password, salt, &signature);
 
    int clientfd;
    char *host, *port, buf[MAXLINE];
    rio_t rio;

    host = server_ip;
    port = server_port;

    clientfd = Open_clientfd(host, port);
    Rio_readinitb(&rio, clientfd);
    RequestHeader_t requestHeader;
    Request_t request;
    memcpy(requestHeader.username, username, USERNAME_LEN);
    memcpy(requestHeader.salted_and_hashed, signature, SHA256_HASH_SIZE);
    requestHeader.length = htonl(strlen(to_get));

    request.header = requestHeader;
  
    memcpy(request.payload, to_get, strlen(to_get));

    

    Rio_writen(clientfd, &request, REQUEST_HEADER_LEN + REQUEST_BODY_LEN);


    //Insert first block into file
    Response_t *response = malloc(sizeof(Response_t));
    Rio_readnb(&rio, &buf, MAX_MSG_LEN);
    memcpy(&response->responseHeader, &buf, RESPONSE_HEADER_LEN);

    response->responseHeader.length = ntohl(*(uint32_t *)&response->responseHeader.length);
    response->responseHeader.statusCode = ntohl(*(uint32_t *)&response->responseHeader.statusCode);
    response->responseHeader.blockNumber = ntohl(*(uint32_t *)&response->responseHeader.blockNumber);
    response->responseHeader.block_count = ntohl(*(uint32_t *)&response->responseHeader.block_count);

    uint32_t blockCount = response->responseHeader.block_count;

    if (response->responseHeader.length < MAX_PAYLOAD) {
        char payload[response->responseHeader.length + 1];
        memcpy(payload, &buf[RESPONSE_HEADER_LEN], response->responseHeader.length);
        FILE *fd;

        payload[response->responseHeader.length] = "\0";
    
        fd = fopen(to_get, "w");
        fputs(payload, fd);

        fclose(fd);

        hashdata_t localBlockHash;
	    get_data_sha(payload, &localBlockHash, response->responseHeader.length, SHA256_HASH_SIZE);
	    for (int i = 0; i < SHA256_HASH_SIZE; i++) {
	       if (localBlockHash[i] != response->responseHeader.blockHash[i]) {
                printf("Packet %u is not hashed correct \n", response->responseHeader.blockNumber);
               
                break;
            }
        }
        
    } else {
        memcpy(response->payload, &buf[RESPONSE_HEADER_LEN], response->responseHeader.length);
        FILE *fd;

        fd = fopen(to_get, "w");

        fputs(response->payload, fd);

        fclose(fd);
    }

    

    if (response->responseHeader.statusCode == 1) {
        printf("Block %i written correctly \n", response->responseHeader.blockNumber);
    } else {
        printf("Error statuscode");
    }

    free(response);

    for (uint32_t i = 1; i < blockCount; i++) {
        Response_t *response = malloc(sizeof(Response_t));
        Rio_readnb(&rio, &buf, MAX_MSG_LEN);
        memcpy(&response->responseHeader, &buf, RESPONSE_HEADER_LEN);

        response->responseHeader.length = ntohl(*(uint32_t *)&response->responseHeader.length);
        response->responseHeader.statusCode = ntohl(*(uint32_t *)&response->responseHeader.statusCode);
        response->responseHeader.blockNumber = ntohl(*(uint32_t *)&response->responseHeader.blockNumber);
        response->responseHeader.block_count = ntohl(*(uint32_t *)&response->responseHeader.block_count);

        if (response->responseHeader.length < MAX_PAYLOAD) {
            char payload[response->responseHeader.length + 1];
	    
            memcpy(payload, &buf[RESPONSE_HEADER_LEN], response->responseHeader.length);
            FILE *fd;
	        payload[response->responseHeader.length] = '\0';
            
            fd = fopen(to_get, "a");
            fputs(payload, fd);
	    
            fclose(fd);
	    hashdata_t localBlockHash;
	    get_data_sha(payload, &localBlockHash, response->responseHeader.length, SHA256_HASH_SIZE);
	    for (int i = 0; i < SHA256_HASH_SIZE; i++) {
	       if (localBlockHash[i] != response->responseHeader.blockHash[i]) {
                printf("Packet %u is not hashed correct \n", response->responseHeader.blockNumber);
               
                break;
            }
        }
	    
	    
	 
        } else {
            memcpy(response->payload, &buf[RESPONSE_HEADER_LEN], response->responseHeader.length);
            FILE *fd;

            fd = fopen(to_get, "a");
	    

            fputs(response->payload, fd);

            fclose(fd);
	        hashdata_t localBlockHash;
	        get_data_sha(&response->payload, &localBlockHash, response->responseHeader.length, SHA256_HASH_SIZE);
	        for (int i = 0; i < SHA256_HASH_SIZE; i++) {
	            if (localBlockHash[i] != response->responseHeader.blockHash[i]) {
                    printf("Packet %u is not hashed correctly \n", response->responseHeader.blockNumber);
                   
                break;
            }
        }


        }

        if (response->responseHeader.statusCode == 1) {
            printf("Block %i written correctly \n", response->responseHeader.blockNumber);
        } else {
            printf("Error statuscode");
        }


        
        free(response);

    }


    Close(clientfd); //line:netp:echoclient:close

    
    
       
   
    hashdata_t localTotalBlockHash;
    get_file_sha(to_get, (uint8_t *)localTotalBlockHash, SHA256_HASH_SIZE);
    for (int i = 0; i < SHA256_HASH_SIZE; i++) {
        if (localTotalBlockHash[i] != response->responseHeader.totalHash[i]) {
                printf("The file '%s' is not hashed correctly \n", to_get);
                break;
                //
            }
        }

        
    



    // Your code here. This function has been added as a guide, but feel free
    // to add more, or work in other parts of the code
}

int main(int argc, char **argv) {
    // Users should call this script with a single argument describing what
    // config to use
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <config file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Read in configuration options. Should include a client_directory,
    // client_ip, client_port, server_ip, and server_port
    char buffer[128];
    fprintf(stderr, "Got config path at: %s\n", argv[1]);
    FILE *fp = Fopen(argv[1], "r");
    while (fgets(buffer, 128, fp)) {
        if (starts_with(buffer, CLIENT_IP)) {
            memcpy(my_ip, &buffer[strlen(CLIENT_IP)],
                strcspn(buffer, "\r\n") - strlen(CLIENT_IP));
            if (!is_valid_ip(my_ip)) {
                fprintf(stderr, ">> Invalid client IP: %s\n", my_ip);
                exit(EXIT_FAILURE);
            }
        } else if (starts_with(buffer, CLIENT_PORT)) {
            memcpy(my_port, &buffer[strlen(CLIENT_PORT)],
                strcspn(buffer, "\r\n") - strlen(CLIENT_PORT));
            if (!is_valid_port(my_port)) {
                fprintf(stderr, ">> Invalid client port: %s\n", my_port);
                exit(EXIT_FAILURE);
            }
        } else if (starts_with(buffer, SERVER_IP)) {
            memcpy(server_ip, &buffer[strlen(SERVER_IP)],
                strcspn(buffer, "\r\n") - strlen(SERVER_IP));
            if (!is_valid_ip(server_ip)) {
                fprintf(stderr, ">> Invalid server IP: %s\n", server_ip);
                exit(EXIT_FAILURE);
            }
        } else if (starts_with(buffer, SERVER_PORT)) {
            memcpy(server_port, &buffer[strlen(SERVER_PORT)],
                strcspn(buffer, "\r\n") - strlen(SERVER_PORT));
            if (!is_valid_port(server_port)) {
                fprintf(stderr, ">> Invalid server port: %s\n", server_port);
                exit(EXIT_FAILURE);
            }
        }
    }
    fclose(fp);

    fprintf(stdout, "Client at: %s:%s\n", my_ip, my_port);
    fprintf(stdout, "Server at: %s:%s\n", server_ip, server_port);

    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    char user_salt[SALT_LEN + 1];

    fprintf(stdout, "Enter a username to proceed: ");
    scanf("%16s", username);
    while ((c = getchar()) != '\n' && c != EOF);
    // Clean up username string as otherwise some extra chars can sneak in.
    for (int i = strlen(username); i < USERNAME_LEN; i++)
    {
        username[i] = '\0';
    }

    fprintf(stdout, "Enter your password to proceed: ");
    scanf("%16s", password);
    while ((c = getchar()) != '\n' && c != EOF);
    // Clean up password string as otherwise some extra chars can sneak in.
    for (int i = strlen(password); i < PASSWORD_LEN; i++)
    {
        password[i] = '\0';
    }

    // Note that a random salt should be used, but you may find it easier to
    // repeatedly test the same user credentials by using the hard coded value
    // below instead, and commenting out this randomly generating section.
    for (int i = 0; i < SALT_LEN; i++)
    {
        user_salt[i] = 'a' + (random() % 26);
    }
    user_salt[SALT_LEN] = '\0';
    //strncpy(user_salt,
    //    "0123456789012345678901234567890123456789012345678901234567890123\0",
    //    SALT_LEN+1);

    fprintf(stdout, "Using salt: %s\n", user_salt);

    // The following function calls have been added as a structure to a
    // potential solution demonstrating the core functionality. Feel free to
    // add, remove or otherwise edit.


    // Register the given user
    register_user(username, password, user_salt);
    
    

    // Retrieve the smaller file, that doesn't not require support for blocks
    get_file(username, password, user_salt, "tiny.txt");
    // Retrieve the larger file, that requires support for blocked messages
    get_file(username, password, user_salt, "hamlet.txt");
    

    exit(EXIT_SUCCESS);
}
