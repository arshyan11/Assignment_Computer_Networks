#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080//port numbeer of client 
#define BUFFER_SIZE 1024//Intialising the buffer size
#define END_OF_FILE "EOF"//end of file signal basically a delimeter

int main() {
    int sock = 0;//unique socket id
    struct sockaddr_in serv_addr;//Structure of data type sockadder use to store addresses of client
    char buffer[BUFFER_SIZE] = {0};//initializing the buffer
    
    // Open video file
    FILE *file = fopen("myvideo.mp4", "rb");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {//creating a socket which uses tcp protocol and ipv4(Af_INET)
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;//Sores the address of ipv4 of client
    serv_addr.sin_port = htons(PORT);//stores the port number of cliet in bytes

    // Convert IPv4 address from text to binary
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connect to receiver
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Send file in segments
    size_t nread;
    while ((nread = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {//reads the data from the file and then stored it in buffer
        send(sock, buffer, nread, 0);//Sending the the buffer to the server
        memset(buffer, 0, BUFFER_SIZE);//clearing the buffer
    }

    // Send EOF signal
    send(sock, END_OF_FILE, strlen(END_OF_FILE), 0);//send a signal of file ending

    // Close file and socket
    fclose(file);
    close(sock);

    printf("File sent successfully.\n");
    return 0;
}
