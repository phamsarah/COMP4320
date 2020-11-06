#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>

#define MAX_SIZE 128

/**
 * Authors: Sarah Pham, Jonathan Osborne, and Kegan van Ginkel
 * 
 **/

// sockaddr_in is a predefined struct in the in.h library, contains the information about the address coming in
struct sockaddr_in serverAddress;

// sockaddr_storage is from the socket.h library, defined to declare storage for automatic variables that is large and aligned enough for the socket address data structure of ANY family
struct sockaddr_storage socketStorage;
double damageProbability;
double lossProbability;


/**
 * damagePacket - method that will randomly damage characters, will simulate the damaged packet. We loop through the give number of bytes and randomize the data within the packet
 * Parameters:
 * bytes - the random number of bytes (indices)
 * packet_content - the packet content
 */
void damagePacket(int bytes, char *packet_content){
    
    for(int i = 0; i < bytes; i++){
        int index = rand() % strlen(packet_content);
        // 32 and 30 were randomly chosen numbers
        packet_content[index] = ((rand() % 32) + 30);
    }

    return;
}


/** 
 * calculateChecksum - returns an in representing the checksum of the packet and check is to be placed in the header of the packet being sent along with the packets sequence number
 * Parameters:
 * buffer - the packet contents
*/
int calculateChecksum(char* buffer) {
    int i = 0, sum = 0;

    while(buffer[i] != '\0'){
        sum += buffer[i];
        i++;
    }

    return sum;
}

/** 
 * gremlin - Returns an int indicating if the packet was damaged, lost, or undamaged, simulates the 3 possible scenarious in the transmission line
 * Parameters:
 * packet_content - the data contained within the packet
 * 
 * Returns the following integers based on the transmission error:
 * (1) - packet corruption
 * (2) - packet loss
 * (3) - correct delivery
*/
int gremlin(char* packet_content) {
    int random_loss_probability = rand() % 100;
    int random_damage_probability = rand() % 100;
    int predicted_loss = lossProbability * 100;
    int predicted_damage = damageProbability * 100;
    
    // We compare the given predicted probability from the runtime argument to the random probability
    if(random_loss_probability < predicted_loss) return 2;
    if(random_damage_probability < predicted_damage) {
        int random_bits_damaged = rand() % 100;

        if(random_bits_damaged < 10) damagePacket(3, packet_content);
        else if(random_bits_damaged < 45) damagePacket(2, packet_content);
        else damagePacket(1, packet_content);

        return 1;
    }

    // Return correct delivery if packet has no loss or is not damaged
    return 0;
}

/**
 * send - sends file to server after running it through gremlin function
 * Parameters:
 * file - the given file we want to send
 * clientfd - the socket 
 * 
 */
void sendFile(int clientfd, struct sockaddr_in server, char *file) {
    char buffer[MAX_SIZE];

    int index;
    FILE *f = fopen(file, "r");
    char put[150];
    sprintf(put, "PUT %s", file);
    sendto(clientfd, put, sizeof(put), 2048, (const struct sockaddr *)&serverAddress, sizeof(serverAddress));

    int i = 0;
    char packet_content[MAX_SIZE-14] = {0};
    int serverLength;

    while(fgets(packet_content, sizeof(packet_content), f) != NULL){
        char packet_content_holder[MAX_SIZE] = {0};

        int checksum = calculateChecksum(packet_content);
        int grem = gremlin(packet_content);

        // Printing to the packet_content_holder, the index, checksum, and packet content
        // packet_content_holder also has a zero padded header
        sprintf(packet_content_holder, "%05d|%07i|%s", i, checksum, packet_content);

        //increments the count when the grem function returns 2, which is for damaged packets
        if (grem == 2){
            i++;
            continue;
        }

        sendto(clientfd, packet_content_holder, MAX_SIZE, 2048, (const struct sockaddr *)&serverAddress, sizeof(serverAddress));
        printf("File sent: %s\n", packet_content_holder);

        serverLength = sizeof(serverAddress);
        bzero(buffer, MAX_SIZE);

        char bufferHolder[20];
        int index = recvfrom(clientfd, (char *) bufferHolder, 48, MSG_WAITALL, (struct sockaddr *)&serverAddress, (socklen_t *)&serverLength);
        bufferHolder[index] = '\0';
        printf("Received File: %s \n", bufferHolder);
        i++;

        bzero(packet_content_holder, MAX_SIZE);

    }

    char null = '\0';
    sendto(clientfd, &null, 1, 2048, (const struct sockaddr *)&serverAddress, sizeof(serverAddress));

    index = recvfrom(clientfd, (char *)buffer, MAX_SIZE, MSG_WAITALL, (struct sockaddr *)&serverAddress, (socklen_t *)&serverLength);
    printf("%s\n", buffer);

    fclose(f);

    return;
}


int main(int argc, char *argv[]) {

    char *ip = "127.0.0.1";
    char *file = "client.txt";
    int port = 8080, clientfd;
    
    // Retrieves probablities of loss and damage from runtime arguments
    damageProbability = atof(argv[1]);
    lossProbability = atof(argv[2]);

    // Retrieves socket descriptor (-1 if failed)
    clientfd = socket(AF_INET, SOCK_DGRAM,0);
    if (clientfd == -1) {
        perror("ERROR: Socket Failed to Open");
        exit(0);
    }

    printf("Success: Server Socket has been Created!");

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = port;

    // Converts IP address in number-and-dots to unsigned long
    server.sin_addr.s_addr = inet_addr(ip);

    // Will send file to server after running it through gremlin function
    sendFile(clientfd, server, file);

    close(clientfd);
    return 0;

}