#include "proto_lib.h"
#include "proto.h"
#include "unix_utils.h"

int recv_packet(int fd, int *type, int *tag, size_t *data_len, void **data)
{
    int terminate = 0;
    PktHdr_t header;
    ssize_t ret = retry_recv(fd, (void *)&header, sizeof(header), 0);
    if (ret == sizeof(header)) {
        *type = header.type;
        *tag = header.tag;
        *data_len = header.payload_len;
        *data = NULL;
        if (header.payload_len) {
            *data = (void *)malloc(header.payload_len);
            if (*data) {
                ret = retry_recv(fd, *data, header.payload_len, 0);
                if (ret != (ssize_t)header.payload_len) {  // Should not happen
                    free(*data);  // Release unnecessary memory
                    abort();
                }
            } else {
                *data = NULL;  // Set NULL when malloc failed
            }
        } else
            *data = NULL;  // Set NULL when payload_len is 0
    } else {
        terminate = 1;
    }
    return terminate;
}

int send_packet(int fd, int type, int tag, size_t data_len, void *data)
{
    int ret = 0;
    PktHdr_t header;
    header.type = type;
    header.tag = tag;
    header.payload_len = data_len;
    ret = retry_send(fd, (void *)&header, sizeof(header), 0);
    if (ret == sizeof(header)) {
        ret = 0;
        if (data_len && data) {
            ret = retry_send(fd, (void *)data, data_len, 0);
        }
    }
    return ret;
}