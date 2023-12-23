#include "handler.h"
#include "error.h"
#include "login.h"


void server_handler_help(int connfd, int ser_port, char *token)
{
    fprintf(stdout, "server_handler_help WIP~\n");
}

void server_handler_create_vote(int connfd, int ser_port, char *token)
{
    fprintf(stdout, "server_handler_create_vote WIP~\n");
    char recvbuffer[MAXLINE];
    recv(connfd, recvbuffer, MAXLINE, 0);
    fprintf(stdout, "The login user is %s\n", recvbuffer);
}

void server_handler_view_inporg(int connfd, int ser_port, char *token)
{
    fprintf(stdout, "server_handler_view_inporg WIP~\n");
}

void server_handler_view_result(int connfd, int ser_port, char *token)
{
    fprintf(stdout, "server_handler_view_result WIP~\n");
}

void server_handler_quit(int connfd, int ser_port, char *token)
{
    fprintf(stdout, "The client has quit~\n");
}

int verify_login(int cli_sockfd, char *user)
{
    char username[100] = {0};
    char password[100] = {0};

    bool authenticated = false;
    for (int i = 0; i < 3; i++) {
        memset(username, '0', strlen(username));
        memset(password, '0', strlen(password));

        recv(cli_sockfd, username, sizeof(username), 0);
        DEBUG_INFO("recv username -> %s\n", username);
        recv(cli_sockfd, password, sizeof(password), 0);
        DEBUG_INFO("recv password -> %s\n\n", password);
        authenticated = authenticate_user(username, password);
        DEBUG_INFO("Ture or false -> %d\n", authenticated);
        if (authenticated) {
            char verify[10] = "pass\n";
            DEBUG_INFO("before send\n");
            send(cli_sockfd, verify, strlen(verify), 0);
            DEBUG_INFO("already send\n");
            memcpy(user, username, strlen(username));
            return 1;
        } else {
            char verify[10] = "fail\n";
            send(cli_sockfd, verify, strlen(verify), 0);
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

typedef void (*func)(int, int, char *);

const func func_table[] = {
#define _(HANDLER, handler) server_handler_##handler,
    SERVER_HANDLER_LIST
#undef _
};

void server_select_cmd(int connfd, int ser_port, char *sendline)
{
    static int flag = 0;
    char *token, *dummy = sendline;
    token = strtok(dummy, " \n");
    if (token != NULL) {
        int cmd_index = atoi(token);
        fprintf(stdout, "cmd_index is %d\n", cmd_index);
        func_table[cmd_index](connfd, ser_port, sendline);
    } else {
        if (!flag) {
            flag = 1;
        } else {
            printf("No command provided.\n");
        }
    }
}