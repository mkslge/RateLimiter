#include <stdlib.h>
#include <stdio.h>
#include<string.h>
#include <time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>

int main() {
    struct sockaddr_in socket_addr;
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if(socket_fd < 0) {
        printf("Socket creation failed\n");
        return 1;
    }

    memset(&socket_addr, 0, sizeof(socket_addr));
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_port = htons(8080); //default
    socket_addr.sin_addr.s_addr = INADDR_ANY; //listens on all interfaces

    if(bind(socket_fd, (const struct sockaddr_in*)&socket_addr, sizeof(socket_addr))  < 0) {
        printf("Binding failed\n");
        close(socket_fd);
        return 1;
    }

    char client_buffer[1024];
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);


    while(1) {
        //receive message
        ssize_t received_bytes = recvfrom(socket_fd, (char *)client_buffer, sizeof(client_buffer), 
        0, (struct sockaddr_in *)&client_addr, &client_len);

        if(received_bytes < 0) {
            printf("Received failed\n");
            close(socket_fd);
            return 1;
        }

        client_buffer[received_bytes] = '\0';
        printf("Received %s from %s:d\n", client_buffer, 
        inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));


        sendto(socket_fd, "Received by server\n", strlen("Received by server\n"), 0, (const struct sockaddr *)&client_addr, client_len);
        
    }
    
    close(socket_fd);
    return 0;
}