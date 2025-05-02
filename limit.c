#include "limit.h"
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define RECENT_CLIENT_CAPACITY 100
#define RATE_LIMIT_THRESHOLD 0.5


/*CIRCULAR BUFFER*/
client_info* recent_clients[RECENT_CLIENT_CAPACITY];

//pointers that keep track of least recently used client and size
int lru_index = 0;
int recent_client_size = 0;





/*Makes a copy of a client info struct*/
client_info* client_constructor(client_info* client) {
    if(!client) {
        return NULL;
    }
    client_info* new_client = (client_info*)malloc(sizeof(client_info));

    //copy variables
    new_client->address_length = client->address_length;
    new_client->address = client->address;
    new_client->udp_addr = client->udp_addr;
    new_client->socket = client->socket;
    new_client->received = client->received;
    new_client->message_size = client->message_size;

    //copy arrays
    memcpy(new_client->request, client->request, MAX_REQUEST_SIZE + 1);
    memcpy(new_client->udp_request, client->udp_request, MAX_UDP_REQUEST_SIZE);

    //copy timestruct
    if(client->ts) { //check that timespec exists before we allocate variables
        new_client->ts = malloc(sizeof(struct timespec));
        new_client->ts->tv_nsec = client->ts->tv_nsec;
        new_client->ts->tv_sec = client->ts->tv_sec;
    } else {
        new_client->ts = NULL;
    }

    return new_client;
}




/*Returns whether or not 2 clients are the same,
right now the code just checks whether or not 
sockets use the same IP, this can be changed tho*/
int compare_clients(client_info* client1, client_info* client2 ) {
    if(!client1 || !client2) {
        return 0;
    }

    if(client1->udp_addr.sin_family != client2->udp_addr.sin_family) {
        //if one is ipv4 and the other is ipv6 they are not the same
        return 0;
    }

    if(client1->udp_addr.sin_family == AF_INET) { //checks if both clients are IPV4

        //special struct just for ipv4
        struct sockaddr_in* addr1 = (struct sockaddr_in*)&client1->udp_addr;
        struct sockaddr_in* addr2 = (struct sockaddr_in*)&client2->udp_addr;

        //compare ipv4 addresses

        if(addr1->sin_addr.s_addr == addr2->sin_addr.s_addr) {
            return 1;  
        }
        
        //check if theyre same on 127.0.0.1
        if(addr1->sin_addr.s_addr == htonl(INADDR_LOOPBACK) ||
            addr2->sin_addr.s_addr == htonl(INADDR_LOOPBACK)) {
            return 1;  
        }

    } else if(client1->udp_addr.sin_family == AF_INET6){ //both must be IPV6

        //special struct just for ipv6
        struct sockaddr_in6* addr1 = (struct sockaddr_in6*)&client1->udp_addr;
        struct sockaddr_in6* addr2 = (struct sockaddr_in6*)&client2->udp_addr;
        
        //now we compare ipv6
        if(!memcmp(&addr1->sin6_addr, &addr2->sin6_addr, sizeof(struct in6_addr))) {
            return 1; 
        }

        //check 127.0.0.1
        struct in6_addr loopback = IN6ADDR_LOOPBACK_INIT;
        if(!memcmp(&addr1->sin6_addr, &loopback, sizeof(loopback)) ||
            !memcmp(&addr2->sin6_addr, &loopback, sizeof(loopback))) {
            return 1; 
        }
    }
    return 0;
}

/* Returns index in recent_clients array if the client is already there
/* or -1 otherwise
*/
int get_client_index(client_info* client) {
    //if its null it doesnt exist
    if (!client) {
        return -1;
    }

    //goes through every client in our list
    for (int i = 0; i < recent_client_size; i++) {
        if(compare_clients(recent_clients[i], client)) {
            //if they're the same we return the index at which its found
            return i;
        }
    }
    //otherwise we return -1 as an indicator that it wasnt found ):
    return -1;
}


/*This function adds a client to the recent clients buffer and returns whether or not it
was successfull*/
int add_client(client_info* client) {
    if (!client) { //if its null its not successful unfortnuately
        return 0;
    }

    //if the buffer has reached the maximum size we overwrite the least recently used client
    if(recent_client_size == RECENT_CLIENT_CAPACITY) {
        recent_clients[lru_index] = client_constructor(client);
        lru_index = (lru_index + 1) % RECENT_CLIENT_CAPACITY; //super beautiful

    } else { //otherwise we just add it to our buffer
        recent_clients[recent_client_size] = client_constructor(client);
        recent_client_size++;
    }
    
    
    return 1;
}

/*Updates client time to new time (cause they just sent another message) and returns new time*/
struct timespec* update_client_time(client_info* client) {
    //make new timespec
    
    client->ts = malloc(sizeof(struct timespec));
    clock_gettime(CLOCK_REALTIME, client->ts);

    
    
    //check where our client is in our buffer
    int client_index = get_client_index(client);

    //if its found we create a new copy (otherwise all this stuff gets messed up)
    if(client_index != -1) {
        recent_clients[client_index] = client_constructor(client);
    } else {
        return NULL;
    }

    //return what we got!
    return recent_clients[client_index]->ts;
}

//returns whether or not the client has sent a message within last threshold
int rate_limit_required(client_info* client) {
    //get current time so we can compare to when the last message was sent
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);

    //where is our client in our buffer 
    int idx = get_client_index(client);

    //get the time difference and check if its lower than the allowed threshold
    double diff = time_difference(recent_clients[idx]->ts, &now);

    return diff <= RATE_LIMIT_THRESHOLD;
}

/*Gets the time difference between two timespecs (assumes theyre not null)*/
double time_difference(struct timespec *time1, struct timespec *time2) {
    if(!time1 || !time2) {
        return 9999999;
    }
    //take seconds
    double seconds1 = time1->tv_sec;
    double seconds2 = time2->tv_sec;

    //and nanoseconds
    double nano_seconds_1 = time1->tv_nsec;
    double nano_seconds_2 = time2->tv_nsec;

    //get difference
    double sec_difference = seconds1 - seconds2;
    double nanosec_difference = nano_seconds_1 - nano_seconds_2;


    //adjust for carry over 
    if(nanosec_difference < 0) {
        sec_difference -= 1;
        nanosec_difference += 1000000000;
    }

    //convert nanoseconds to seconds
    double time_difference = sec_difference + (nanosec_difference / 1000000000.0);

    return fabs(time_difference);
}

/*grabs the client given the index in the buffer*/
client_info* get_recent_client(int index) {
    return recent_clients[index];
}