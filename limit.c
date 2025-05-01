#include "limit.h"
#include <math.h>
#define CLIENT_CAPACITY 100

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

