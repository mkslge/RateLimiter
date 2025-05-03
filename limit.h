#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>


#define MAX_REQUEST_SIZE 2047
#define MAX_UDP_REQUEST_SIZE 8008
#define SOCKET int

typedef struct client_info {
    socklen_t address_length;
    struct sockaddr_storage address;
    struct sockaddr_in udp_addr;
    SOCKET socket;
    char request[MAX_REQUEST_SIZE+1];
    char udp_request[MAX_UDP_REQUEST_SIZE];
    int received;
    int message_size;
    struct timespec* ts;
} client_info;


 client_info* client_constructor(client_info* client);

int compare_clients(client_info* client1, client_info* client2);

int get_client_index(client_info* client);

int add_client(client_info* client);

struct timespec* update_client_time(client_info* client);

double time_difference(struct timespec *time1, struct timespec *time2);

int rate_limit_required(client_info* client);

client_info* get_recent_client(int index);

client_info* create_client(struct sockaddr_in sock, int socket_fd);




