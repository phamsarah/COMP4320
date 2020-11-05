#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <netdb.h>

#define MAX_SIZE 128

// sockaddr_in is a predefined struct in the in.h library, contains the information about the address coming in
struct sockaddrin serverAddress;

// sockaddr_storage is from the socket.h library, defined to declare storage for automatic variables that is large and aligned enough for the socket address data structure of ANY family
struct sockaddr_storage socketStorage;

double damageProb;
double lossProb;

/**
 * Authors: Sarah Pham, Jonathan Osborne, and Kegan van Ginkel
 * 
 **/


//Returns an int indicating if the packet was damaged, lost, or undamaged.
int gremlin(char *data) {

}

//returns an in representing the checksum of the packet.
//check is to be placed in the header of the packet being seng along with the packets sequence number
int checksum(char *data) {

}

/**
 * send - sends file to server after running it through gremlin function
 * Parameters:
 * file - the given file we want to send
 * clientfd - the socket 
 * 
 */
void send(int clientfd, struct sockaddrin server, char *file) {
    char buffer[MAX_SIZE];
    
}





int main(int argc, char *argv[]) {

    char *ip = "127.0.0.1";
    char *file = "client.txt";
    int port = 8080, clientfd;
    
    //gets probablities of loss and damage from runtime arguments
    damageProb = atof(argv[1]);
    lossProb = atof(argv[2]);

    //gets socket descriptor (-1 if failed)
    clientfd = socket(AF_INET, SOCK_DGRAM,0);
    if (clientfd == -1) {
        perror("ERROR: Socket Failed to Open");
        exit(0);
    }

    printf("Success: Server Socket has been Created!");

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = port;

    //converts IP address in number-and-dots to unsigned long
    server.sin_addr.s_addr = inet_addr(ip);

    //will send file to server after running it through gremlin function
    send(clientfd, server, file);

    close(clientfd);
    return 0;

}