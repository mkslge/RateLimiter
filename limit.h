#include <time.h>s



typedef struct client_info {
    struct timespec* last_message_time;

} client_info;


int add_client(client_info* new_client);





