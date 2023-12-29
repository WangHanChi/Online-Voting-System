#include "handler.h"
#include "error.h"
#include "login.h"
#include "proto_lib.h"


void server_handler_help(int connfd, char *token)
{
    fprintf(stdout, "server_handler_help WIP~\n");
}

void server_handler_create_vote(int connfd, char *token)
{
    fprintf(stdout, "server_handler_create_vote WIP~\n");
    char recvbuffer[MAX_MESSAGE_LENGTH];
    recv(connfd, recvbuffer, MAX_MESSAGE_LENGTH, 0);
    fprintf(stdout, "The login user is %s\n", recvbuffer);
}

void server_handler_view_inporg(int connfd, char *token)
{
    fprintf(stdout, "server_handler_view_inporg WIP~\n");
}

void server_handler_view_result(int connfd, char *token)
{
    fprintf(stdout, "server_handler_view_result WIP~\n");
}

void server_handler_quit(int connfd, char *token)
{
    fprintf(stdout, "The client has quit~\n");
}

int verify_login(int cli_sockfd, char *user)
{
    char username[MAX_NAME_LENGTH] = {0};
    char password[MAX_NAME_LENGTH] = {0};
    PktHdr_t packet;
    void *pData = NULL;
    bool authenticated = false;

    for (int i = 0; i < 3; i++) {
        memset(username, 0, MAX_NAME_LENGTH);
        memset(password, 0, MAX_NAME_LENGTH);

        recv_packet(cli_sockfd, &(packet.type), &(packet.tag),
                    &(packet.payload_len), &pData);
        if ((packet.type == TOSERV_TYPE_LOGIN) &&
            (packet.tag == TOSERV_TAG_USERNAME)) {
            strncpy(username, pData, packet.payload_len);
            DEBUG_INFO("recv username -> %s\n", username);
        }

        recv_packet(cli_sockfd, &(packet.type), &(packet.tag),
                    &(packet.payload_len), &pData);
        if ((packet.type == TOSERV_TYPE_LOGIN) &&
            (packet.tag == TOSERV_TAG_PASSWORD)) {
            strncpy(password, pData, packet.payload_len);
            DEBUG_INFO("recv password -> %s\n\n", password);
        }

        authenticated = authenticate_user(username, password);
        DEBUG_INFO("Ture or false -> %d\n", authenticated);
        if (authenticated) {
            send_packet(cli_sockfd, FROMSERV_TYPE_RESULT, FROMSERV_TAG_OKAY, 0,
                        NULL);
            strncpy(user, username, strlen(username));
            return 1;
        } else {
            send_packet(cli_sockfd, FROMSERV_TYPE_RESULT, FROMSERV_TAG_FAIL, 0,
                        NULL);
        }
        if (i == 2)
            fprintf(stderr, "Enter wrong password over 3 times\n");
    }
    return 0;
}

/* server commands list
 * (command, code)
 */
#define SERVER_HANDLER_LIST     \
    _(HELP, help)               \
    _(CREATE_VOTE, create_vote) \
    _(VIEW_INPORG, view_inporg) \
    _(VIEW_RESULT, view_result) \
    _(QUIT, quit)

typedef enum {
#define _(HANDLER, handler) SERVER_HANDLER_##HANDLER,
    SERVER_HANDLER_LIST
#undef _
        HANDLER_MAX
} server_handler_list;

const char *handler_str[] = {
#define _(HANDLER, handler) #HANDLER,
    SERVER_HANDLER_LIST
#undef _
};

typedef void (*func)(int, char *);

const func func_table[] = {
#define _(HANDLER, handler) server_handler_##handler,
    SERVER_HANDLER_LIST
#undef _
};

void server_select_cmd(int connfd, char *sendline)
{
    char *token, *dummy = sendline;
    token = strtok(dummy, " \n");
    if (token != NULL) {
        int cmd_index = atoi(token);
        fprintf(stdout, "cmd_index is %d\n", cmd_index);
        func_table[cmd_index](connfd, sendline);
    } else {
        printf("No command provided.\n");
    }
}