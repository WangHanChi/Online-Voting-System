#include "command.h"
#include "error.h"

/* print help information */
void client_cmd_help(UserInfo_t *user)
{
    // clang-format off
    fprintf(stdout, "+------------------- command manual ------------------------+\n");
    fprintf(stdout, "|   command   |                description                  |\n");
    fprintf(stdout, "|-------------+---------------------------------------------|\n");
    fprintf(stdout, "| help        | Show command manual                         |\n");
    fprintf(stdout, "| WIP         | Work in progress                            |\n");
    fprintf(stdout, "+-----------------------------------------------------------+\n");
    // clang-format on
    return;
}

void client_cmd_create_vote(UserInfo_t *user)
{
    fprintf(stdout, "WIP!\n");
    send(user->sockfd, user->username, strlen(user->username), 0);
    printf("Send complete\n");
}

void client_cmd_view_inporg(UserInfo_t *user)
{
    fprintf(stdout, "WIP!\n");
}

void client_cmd_view_result(UserInfo_t *user)
{
    fprintf(stdout, "WIP!\n");
}

/* quit */
void client_cmd_log_out(UserInfo_t *user)
{
    fprintf(stdout, "Close connection!\n");
    free(user);
    exit(0);
}

/*
 * client commands list
 * (command, code)
 */
#define CLIENT_CMD_LIST         \
    _(HELP, help)               \
    _(CREATE_VOTE, create_vote) \
    _(VIEW_INPORG, view_inporg) \
    _(VIEW_RESULT, view_result) \
    _(LOG_OUT, log_out)


typedef enum {
#define _(CMD, cmd) CLIENT_CMD_##CMD,
    CLIENT_CMD_LIST
#undef _
        CMD_MAX
} client_cmd_code_list;

const char *cmd_str[] = {
#define _(CMD, cmd) #CMD,
    CLIENT_CMD_LIST
#undef _
};

typedef void (*func)(UserInfo_t *);

const func func_table[] = {
#define _(CMD, cmd) client_cmd_##cmd,
    CLIENT_CMD_LIST
#undef _
};

void client_select_cmd(UserInfo_t *user)
{
    static int flag = 0;
    char *token, *dummy = user->sendline;

    token = strtok(dummy, " \n");

    if (token != NULL) {
        int cmd_index = atoi(token);
        func_table[cmd_index](user);
    } else {
        if (!flag) {
            flag = 1;
        } else {
            printf("No command provided.\n");
        }
    }

    fprintf(stdout, "\e[32;01m%s\e[33;01m>\e[0m ", user->username);
}