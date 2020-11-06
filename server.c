#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <err.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <sys/types.h>


#define MAX_SIZE 128

/**
 * Authors: Sarah Pham, Jonathan Osborne, and Kegan van Ginkel
 * 
 **/

int sockfd, newSocket;

// sockaddr_in is a predefined struct in the in.h library, contains the information about the address coming in
struct sockaddr_in serverAddress, peerAddr;

// sockaddr_storage is from the socket.h library, defined to declare storage for automatic variables that is large and aligned enough for the socket address data structure of ANY family
struct sockaddr_storage socketStorage;

// socklen_t is an unsigned 32 bit type from socket.h, we use this because it is recommended to not use values larger than 2^(32)-1
socklen_t socketAddressSize;

int connectToClient(){
    /** 
     * Socket method returns a nonnegative number integer if successful. Creates an unbound socket in a communications domain, 
     * and returns a file descriptor that can be used in later function calls that operate on sockets
     * 
     *   PF_INET = protocol family that refers to anything in the protocol, usually sockets/ports
     *   SOCK_DGRAM = is a datagram based protocol used by UDP; send one datagram and receive one reply then connection terminates
     *   0 = protocol of 0 causes socket() to use an unspecified default protocol appropriate for the requested socket type
    **/ 
   //getaddrinfo(NULL, "3490", &serverAddress);

   char *ip = "127.0.0.1";
   int portNo = 8080;
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        // errx produces an error message
        errx(1, "Socket call error");
        printf("Error calling socket");
        exit(EXIT_FAILURE);
    }

    printf("Success: Server Socket %i Call\n", sockfd);


    serverAddress.sin_family = AF_INET; // Setting family to INET
    serverAddress.sin_port = htons(portNo); // Binding a port number to the socket
    serverAddress.sin_addr.s_addr = INADDR_ANY; // sin_addr is the socket ip address, we set the current host IP address: INADDR_ANY. 

    //bzero((char *) &serverAddress, sizeof (serverAddress));
    /**
     * bind method in socket.h library assigns a local socket addr to 
     * a socket identified by descriptor socket, which has no local socket addr assigned
     * - basically binds a name to a socket
     * - returns 0 if sucessful and -1 otherwise
     * 
     * Parameters:
     * socket - specifies the file descriptor of the socket to be bound
     * address - points to a sockaddr structure containing address to be bound
     * address_len - specifies length of sockaddr structure
     **/
    if(bind(sockfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0){
        errx(1, "Socket address binding error\n");
        printf("socket binding error\n");
        exit(EXIT_FAILURE);
    }

    printf("Success: Socket Address Binding\n");

    socketAddressSize = sizeof(socketStorage); // We initialize a new socket that is the size of the socket storage

    return 0;
}

/**
 * parseStrToToken - method that will parse any given string to a sequence of tokens, loops through the string that is split by any given deliminator
 * Parameters:
 * pointer - value of end of token, tells us when to break out of the loop
 * buffer - our string buffer 
*/
char* parseStrToToken(int pointer, char *buffer, char* deliminator){
    int i = 0;
    char *temp = NULL;

    // strtok_r is used to break up the string by delimiters
    char *token;
    token = strtok_r(buffer, deliminator , &temp); 

    while(token != NULL){
        if(i == pointer) break;
        i++;

        token = strtok_r(NULL, deliminator, &temp);
    }

    return token;
}

/**
 * calculateChecksum will calculate the sum of the data inside the packet and will return true if the sum is equal to the checksum
 * Parameters:
 * bufferHolder - the packet including all the data
 * checksum - sum of all bytes within the packet
 */
int calculateChecksum(char* bufferHolder, int checksum){
    int i = 0, sum = 0;

    while(bufferHolder[i] != '\0'){
        sum += bufferHolder[i];
        i++;
    }

    return checksum == sum; 
}

/**
*   writeToFile creates a file and transmits the message to another socket
*   Parameters:
*   serverSocket - our server socket
*   socketStorage - our size of our socket
*/
void writeToFile(int serverSocket, struct sockaddr_storage socketStorage){
    FILE *file;
    struct sockaddr_in temp_address;

    
    char buffer[MAX_SIZE];
    char bufferHolder[MAX_SIZE - 12];
    int character = 1, checksum, numOfPackets;

    

    socklen_t addressSize = sizeof(socketStorage);

    // recvfrom receives a message from a socket, retrieves source address of received data
    character = recvfrom(serverSocket, buffer, MAX_SIZE, MSG_WAITALL, (struct sockaddr *)&temp_address, &addressSize);
    printf("%s\n", buffer);
    char* newFile = parseStrToToken(1, buffer, " ");
    file = fopen(newFile, "w");

    // bzero erases data in the MAX_SIZE bytes of the memory starting at buffer location by writing zeros
    bzero(buffer, MAX_SIZE);

    if(file == NULL) errx(1, "File creation error");

    // We loop through the buffer until we reach a null terminator \0
    while(character != '\0'){
        character = recvfrom(serverSocket, buffer, MAX_SIZE, MSG_WAITALL, (struct sockaddr *) &socketStorage, &addressSize);
        if(buffer[0] == '\0') break;
    }

    // sendto will send a message on a socket, from library socket.h
    sendto(serverSocket, (const char *)buffer, MAX_SIZE, 2048, (const struct sockaddr *)&socketStorage, addressSize);


    char holder_1[MAX_SIZE];
    char holder_2[MAX_SIZE];
    
    sprintf(holder_1, "%s", buffer);
    sprintf(holder_2, "%s", buffer);

    sprintf(bufferHolder, "%s", parseStrToToken(2, buffer, "|"));
    fprintf(file, "%s", bufferHolder);

    checksum = atoi(parseStrToToken(1, holder_1, "|"));
    numOfPackets = atoi(parseStrToToken(0, holder_2, "|"));

    if(!calculateChecksum(bufferHolder, checksum)) printf("This packet has an error!");

    bzero(buffer, MAX_SIZE);

    return;
}




int main(void){
    connectToClient();
    printf("Server Socket: %i", sockfd);
    writeToFile(sockfd, socketStorage);

    return 0;
}