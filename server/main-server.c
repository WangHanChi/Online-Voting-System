#include <pthread.h>
#include <sys/sysinfo.h>
#include "error.h"
#include "handler.h"
#include "login.h"
#include "proto_lib.h"
#include "syslog.h"

#define MAX_THREADS (2 * get_nprocs())

typedef struct {
    int cli_sockfd;
    char *pbuf;
} ThreadArgs;

void *handle_client(void *args)
{
    ThreadArgs *threadArgs = (ThreadArgs *)args;
    int cli_sockfd = threadArgs->cli_sockfd;
    char *buf = threadArgs->pbuf;
    int n = 0;

    fprintf(stdout, "Thread created for dealing with client requests\n");
    fprintf(stdout, "Now is dealing with <longin>\n");

    char user[MAX_NAME_LENGTH] = {0};
    int verify = verify_login(cli_sockfd, user);
    if (!verify)
        return NULL;

    PktHdr_t packet;
    void *pData = NULL;

    while (recv_packet(cli_sockfd, &(packet.type), &(packet.tag),
                       &(packet.payload_len), &pData) == 0) {
        if ((packet.type < TYPEMAX) && (packet.tag <= TAGMAX)) {
            send_packet(cli_sockfd, FROMSERV_TYPE_ACK, FROMSERV_TAG_OKAY, 0,
                        NULL);
            strncpy(buf, pData, packet.payload_len);
            DEBUG_INFO("String received from client: %s", buf);
            syslog_record(buf, user);
            server_select_cmd(cli_sockfd, buf);
        } else {
            fprintf(stdout, "The packet may lost\n");
        }
        free(pData);
        pData = NULL;
    }

    close(cli_sockfd);
    free(args);
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        ERROR_INFO("Usage: ./server <port number>\n");
    }

    int sockfd, cli_sockfd;
    struct sockaddr_in ser_addr, cli_addr;
    socklen_t ser_len, cli_len;
    char buf[MAX_MESSAGE_LENGTH];
    int on = 1;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        ERROR_INFO("Sokcet Error!\n");
    }

    // for "Address already in use" error message
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) == -1) {
        ERROR_INFO("Setsockopt error\n");
    }

    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (atoi(argv[1]) <= 1024) {
        ERROR_INFO("Port number must be greater than 1024\n");
    }
    ser_addr.sin_port = htons(atoi(argv[1]));
    ser_len = sizeof(ser_addr);
    if ((bind(sockfd, (struct sockaddr *)&ser_addr, ser_len)) < 0) {
        ERROR_INFO("Bind Error!\n");
    }
    if (listen(sockfd, LISTENQ) < 0) {
        ERROR_INFO("Linsten Error!\n");
    }
    cli_len = sizeof(cli_addr);

    int ret = system("clear");
    if (ret == -1) {
        DEBUG_INFO("do system method <clear> failed\n");
    }
    // Initialize thread pool
    pthread_t thread_pool[MAX_THREADS];
    int thread_count = 0;

    syslog_init();

    while (1) {
        memset(buf, 0, sizeof(buf));
        printf("\e[36;01mserver\e[33;01m>\e[0m ");
        if ((cli_sockfd = accept(sockfd, (struct sockaddr *)&cli_addr,
                                 (socklen_t *)&cli_len)) < 0) {
            ERROR_INFO("Accept Error!\n");
        }

        fprintf(stderr, "Waiting for request ...\n");

        // Prepare arguments for the thread
        ThreadArgs *threadArgs = (ThreadArgs *)malloc(sizeof(ThreadArgs));
        threadArgs->cli_sockfd = cli_sockfd;
        threadArgs->pbuf = buf;

        // Check if the thread pool is full
        if (thread_count < MAX_THREADS) {
            // Create a new thread
            pthread_create(&thread_pool[thread_count], NULL, handle_client,
                           (void *)threadArgs);
            thread_count++;
        } else {
            // Wait for a thread to finish and reuse it
            pthread_join(thread_pool[thread_count % MAX_THREADS], NULL);
            pthread_create(&thread_pool[thread_count % MAX_THREADS], NULL,
                           handle_client, (void *)threadArgs);
            thread_count++;
        }
    }

    // Clean up and join remaining threads in the pool
    for (int i = 0; i < thread_count; i++) {
        pthread_join(thread_pool[i], NULL);
    }
    return 0;
}