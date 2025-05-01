#include "limit.h"
#include <math.h>
#define CLIENT_CAPACITY 100
#define RATE_LIMIT 0.5
client_info* clients[CLIENT_CAPACITY];
int lru_index = 0;
int client_size = 0;



int add_client(client_info* new_client) {

    if(client_size != CLIENT_CAPACITY) { //if the buffer isnt full yet
        clients[client_size] = new_client;
        client_size++;

    } else { //if its full overwrite the least recently used client

        clients[lru_index] = new_client;
        lru_index = (lru_index + 1) % CLIENT_CAPACITY;
    }
}

double get_time_difference(struct timespec* ts1, struct timespec* ts2) {
    return fabs(ts1->tv_sec - ts2->tv_nsec + ( (ts1->tv_nsec - ts2->tv_nsec) / 1000000000)); 
}


/*returns FALSE if the */
int check_rate_limit(struct timespec* ts1, struct timespec* ts2) {
    return get_time_difference(ts1, ts2) > RATE_LIMIT;
}


