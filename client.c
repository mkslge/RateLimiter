#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

#define TIMEOUT_DELAY_MS 20
#define true 1
int main(int argc, char* argv[]) {
    //check that ip address was passed in
    if(argc != 2) {
        printf("client executable expects format client <ip-address>");
        return 1;
    } 
    //initalize socket variables
    struct sockaddr_in server_addr;
    int socket_fd;
    socklen_t addr_len = sizeof(server_addr);
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    //configure message
    char* msg = "I love Computer Science\n";
    int msg_length = strlen(msg);

    //set up socket variables
    memset(&server_addr, 0, addr_len);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    
    //send messages forever 
    while(true) {
        sendto(socket_fd, msg, msg_length, 0 
        , (const struct sockaddr * )&server_addr, addr_len);

        //pause for a bit
        usleep(TIMEOUT_DELAY_MS * 1000);
    }



    return 0;
}