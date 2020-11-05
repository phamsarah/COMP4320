#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <netdb.h>

#define SIZE 128

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

//sends file to server after running it through gremlin function
void send(char *filename, int clientfd, struct sockaddrin server) {

}





int main(int argc, char *argv[]) {

    char *ip = "127.0.0.1";
    int port = 8080;
    int clientfd;

    FILE *f;
    char *filename = "client.txt";

    //gets probablities of loss and damage from runtime arguments
    damageProb = atof(argv[1]);
    lossProb = atof(argv[2]);

    //gets socket descriptor (-1 if failed)
    clientfd = socket(AF_INET, SOCK_DGRAM,0);
    if (clientfd == -1) {
        perror("ERROR Socket failed to open succesfully");
        exit(0);
    }


    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = port;

    //converts IP address in number-and-dots to unsigned long
    server.sin_addr.s_addr = inet_addr(ip);

    //will send file to server after runnign it through gremlin function
    send(filename, clientfd, server);

}