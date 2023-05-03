#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <cstring>
#include <string.h>

#define MAX_MESSAGE_LENGTH 127

int sSocket;

void *readServer(void *arg);
void *sendToServer(void *arg);

int main(int argc, char *argv[]) {

     // Check that correct number of arguments are provided
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <ip_address> <port_number>" << std::endl;
        return 1;
    }
    
    // socket creation
    sSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sSocket == -1) {
        perror("Failed to create socket");
        return -1;
    }
    printf("Socket creation successfull\n");
    

   struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(4070);

    int r = connect(sSocket, (struct sockaddr *)&server_addr, sizeof server_addr);
    if (r == -1) {
        perror("Connection failed");
        return -1;
    }
    
     // Send and receive messages
    char buffer[256];
    while (true) {
        std::cout << "Enter message: ";
        memset(buffer, 0, sizeof(buffer));
        std::cin.getline(buffer, sizeof(buffer));
        send(sSocket, buffer, strlen(buffer), 0);
        if (strcmp(buffer, "exit") == 0) {
            break;
        }
        memset(buffer, 0, sizeof(buffer));
        recv(sSocket, buffer, sizeof(buffer), 0);
        std::cout << "Server: " << buffer << std::endl;
    }
    
    printf("Server connected\n");

    pthread_t readThread, sendThread;
    pthread_create(&readThread, NULL, readServer, NULL);
    pthread_create(&sendThread, NULL, sendToServer, NULL);

    pthread_join(readThread, NULL);
    pthread_join(sendThread, NULL);

    close(sSocket);
    return 0;
}

void *readServer(void *arg) {
    char message[MAX_MESSAGE_LENGTH+1];
    while (1) {
        int r = recv(sSocket, message, MAX_MESSAGE_LENGTH, 0);
        if (r > 0) {
            message[r] = '\0';
            printf("%s\n", message);
        } else {
            perror("Failed to receive message from server");
            break;
        }
    }
    return NULL;
}

void *sendToServer(void *arg) {
    char message[MAX_MESSAGE_LENGTH+1];
    while (1) {
        //printf("\nEnter Message > ");
        fgets(message, MAX_MESSAGE_LENGTH+1, stdin);
        int len = strlen(message);
        if (len > 0) {
            if (message[len-1] == '\n') {
                message[len-1] = '\0';
            }
            if (send(sSocket, message, strlen(message), MSG_NOSIGNAL) == -1) {
                perror("Failed to send message to server");
                break;
            }
        }
    }
    return NULL;
}
