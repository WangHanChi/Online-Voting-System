#include <signal.h>
#include "command.h"
#include "error.h"
#include "proto_lib.h"
#include "text.h"
#include "user_login.h"

UserInfo_t *user;

void sigint_handler(int signum)
{
    send_packet(user->sockfd, TOSERV_TYPE_DISCONN, TOSERV_TAG_DISCONN, 0, NULL);
    shutdown(user->sockfd, SHUT_RDWR);
    fprintf(stderr, "Disconnect ....\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        ERROR_INFO(
            "Usage: ./client <IP address of the server> <port number>\n");
    }
    signal(SIGINT, sigint_handler);

    int sockfd;
    char sendline[MAX_MESSAGE_LENGTH];
    struct sockaddr_in ser_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        ERROR_INFO("Problem in creating the socket\n");
    }

    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = inet_addr(argv[1]);
    ser_addr.sin_port = htons(atoi(argv[2]));

    // Connection of the client to the socket
    if (connect(sockfd, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) < 0) {
        ERROR_INFO("Problem in connecting to the server\n");
    }

    user = (UserInfo_t *)malloc(sizeof(UserInfo_t));
    user->sockfd = sockfd;
    user_login(user);
    user->server_addr = (char *)malloc(sizeof(argv[1]));
    strncpy(user->server_addr, argv[1], sizeof(argv[1]));

    int ret = system("clear");
    if (ret == -1) {
        DEBUG_INFO("clear the terminal windows did not work");
    }

    fprintf(stdout, "%s", banner);
    fprintf(stdout, "%s", first_level_menu);
    fprintf(stdout, "\e[32;01m%s\e[33;01m>\e[0m ", user->username);
    PktHdr_t packet;
    void *pData = NULL;

    while (fgets(sendline, MAX_MESSAGE_LENGTH, stdin) != NULL) {
        send_packet(sockfd, TOSERV_TYPE_CMD, TOSERV_TAG_CMD, strlen(sendline),
                    sendline);
        recv_packet(user->sockfd, &(packet.type), &(packet.tag),
                    &(packet.payload_len), &pData);
        if ((packet.type < TYPEMAX) && (packet.tag < TAGMAX)) {
            strncpy(user->sendline, sendline, sizeof(user->sendline));
            client_select_cmd(user);
        } else {
            fprintf(stdout, "The packet may lost\n");
        }
        free(pData);
        pData = NULL;
    }
    return 0;
}
