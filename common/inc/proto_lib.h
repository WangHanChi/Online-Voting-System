#include <stdlib.h>
#include "unix_utils.h"

int recv_packet(int fd,
                int *type,
                int *tag,
                size_t *payload_len,
                void **payload);
int send_packet(int fd, int type, int tag, size_t payload_len, void *payload);