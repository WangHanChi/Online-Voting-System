#include "unix_utils.h"
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>

ssize_t retry_send(int sock_fd, const void *buf, size_t len, int flags)
{
    ssize_t sent_len = 0;
    while (sent_len != (ssize_t)len) {
        const void *send_buf = (void *)((uintptr_t)buf + sent_len);
        ssize_t ret = send(sock_fd, send_buf, len - sent_len, flags);
        if (ret > 0)
            sent_len += ret;
        else {
            if ((ret == -1) && ((errno == EINTR) || (errno == EAGAIN)))
                continue;
            else {
                sent_len = ret;
                break;
            }
        }
    }
    return sent_len;
}

ssize_t retry_recv(int sock_fd, void *buf, size_t len, int flags)
{
    ssize_t received_len = 0;
    while (received_len != (ssize_t)len) {
        void *recv_buf = (void *)((uintptr_t)buf + received_len);
        ssize_t ret = recv(sock_fd, recv_buf, len - received_len, flags);
        if (ret > 0)
            received_len += ret;
        else {
            if ((ret == -1) && ((errno == EINTR) || (errno == EAGAIN)))
                continue;
            else {
                received_len = ret;
                break;
            }
        }
    }
    return received_len;
}