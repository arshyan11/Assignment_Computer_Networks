#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define END_OF_FILE "EOF"

int main() {
    int server_fd, new_socket;//server_fd stores the unique id(socket address) of server,new_socket stores the unique id(socket address) of client
    struct sockaddr_in address;// This variable is a structure of type sockaddr_in, which is used to store information about the server's address (such as IP address and port). 
    int addrlen = sizeof(address);//use to store the size of address structure.
    char buffer[BUFFER_SIZE] = {0};
    
    // Open new file to write the received video data
    FILE *file = fopen("received_video.mp4", "wb");//the video is converted in bytes and stored in variable wb
    if (!file) {
        perror("Error opening file");
        return -1;
    } 

    // Create socket of tcpmand uses ipv4
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {//Used to create a TCP socket is created using socket(AF_INET, SOCK_STREAM, 0). This sets up the server to use the IPv4 protocol and TCP.
        perror("Socket failed");
        return -1;
    }

    address.sin_family = AF_INET;//stores the IP address of ipv4
    address.sin_addr.s_addr = INADDR_ANY;//stores the ip address of server
    address.sin_port = htons(PORT);//sets the port number for the server to listen on, using the htons() function to convert it to the appropriate byte order.

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {//The bind() function attaches the socket to the IP address INADDR_ANY (which means any available network interface) and port 8080.
        perror("Bind failed");
        return -1;
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0) {//waiting for connection,maximum limits of client are 3
        perror("Listen failed");
        return -1;
    }

    // Accept the sender connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        return -1;
    }

    // Receive file segments and write them to the new file
    while (1) {
        int valread = recv(new_socket, buffer, BUFFER_SIZE, 0);//recv(new_socket, buffer, BUFFER_SIZE, 0) reads up to BUFFER_SIZE bytes from the client and stores the data in the buffer.

        // Check for EOF signal
        if (valread > 0 && strstr(buffer, END_OF_FILE) != NULL) {
            break;
        }
        
        fwrite(buffer, 1, valread, file);//The data is then written to the file using fwrite(buffer, 1, valread, file). Here, valread is the number of bytes received.
        memset(buffer, 0, BUFFER_SIZE);//clear the buffer
    }

    // Close file and socket
    fclose(file);
    close(new_socket);
    close(server_fd);

    printf("File received successfully.\n");
    return 0;
}
