#pragma once

#include <stdint.h>
#include <unistd.h>

ssize_t retry_send(int sock_fd, const void *buf, size_t len, int flags);
ssize_t retry_recv(int sock_fd, void *buf, size_t len, int flags);
