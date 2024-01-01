#include "command.h"
#include "error.h"
#include "proto_lib.h"

/* print help information */
void client_cmd_help(UserInfo_t *user)
{
    // clang-format off
    fprintf(stdout, "+------------------------- command manual ---------------------------+\n");
    fprintf(stdout, "|   command         |                description                     |\n");
    fprintf(stdout, "|-------------------+------------------------------------------------|\n");
    fprintf(stdout, "| (0) Help          | Show command manual                            |\n");
    fprintf(stdout, "| (1) Create Vote   | Create the vote event                          |\n");
    fprintf(stdout, "| (2) View          | View the vote which is in progress and vote it |\n");
    fprintf(stdout, "| (3) history       | View the historical results                    |\n");
    fprintf(stdout, "| (4) Logout        | Exit the Online-Voting-System                  |\n");
    fprintf(stdout, "+--------------------------------------------------------------------+\n");
    // clang-format on
    return;
}

void client_cmd_create_vote(UserInfo_t *user)
{
    char inputbuf[MAX_MESSAGE_LENGTH] = {0};
    uint8_t num_candidates;
    uint32_t duration;
    int ret = 0;
    PktHdr_t packet;
    void *pData = NULL;

    // sned username to Server
    send_packet(user->sockfd, TOSERV_TYPE_CREATE, TOSERV_TAG_USERNAME,
                strlen(user->username), user->username);
    recv_packet(user->sockfd, &(packet.type), &(packet.tag),
                &(packet.payload_len), &pData);
    if ((packet.type != FROMSERV_TYPE_ACK) ||
        (packet.tag != FROMSERV_TAG_OKAY)) {
        fprintf(
            stdout,
            "Something wrong when sending username to Server in Create Vote\n");
    }
    free(pData);
    pData = NULL;

    // get and send Title to Server
    while (1) {
        memset(inputbuf, 0, MAX_MESSAGE_LENGTH);
        fprintf(stdout, "Please Enter the title of vote\n");
        if (fgets(inputbuf, MAX_NAME_LENGTH, stdin) == NULL) {
            fprintf(stderr, "Error reading input\n");
            continue;
        }
        // Check if the input is non-empty and within the desired length
        if (strlen(inputbuf) > 0 && strlen(inputbuf) <= MAX_NAME_LENGTH) {
            break;
        } else {
            fprintf(stdout,
                    "Invalid input. Title should be non-empty and up to %d "
                    "characters.\n",
                    MAX_NAME_LENGTH);
        }
    }
    send_packet(user->sockfd, TOSERV_TYPE_CREATE, TOSERV_TAG_TITLE,
                strlen(inputbuf), inputbuf);
    recv_packet(user->sockfd, &(packet.type), &(packet.tag),
                &(packet.payload_len), &pData);
    if ((packet.type != FROMSERV_TYPE_ACK) ||
        (packet.tag != FROMSERV_TAG_OKAY)) {
        fprintf(
            stdout,
            "Something wrong when sending username to Server in Create Vote\n");
    }
    free(pData);
    pData = NULL;

    // get and send the number of Options to Server
    while (1) {
        num_candidates = 0;
        fprintf(stdout, "Please Enter the number of candidates\n");
        if (fgets(inputbuf, sizeof(inputbuf), stdin) == NULL) {
            fprintf(stderr, "Error reading input\n");
            continue;
        }
        ret = sscanf(inputbuf, "%hhu", &num_candidates);
        if ((ret != 1) || (num_candidates < 1) ||
            (num_candidates > MAX_CANDIDATES)) {
            fprintf(stderr, "Invalid input. Number should be in the 1 ~ %d\n",
                    MAX_CANDIDATES);
        } else
            break;
    }
    send_packet(user->sockfd, TOSERV_TYPE_CREATE, TOSERV_TAG_OPTIONS,
                sizeof(num_candidates), &num_candidates);
    recv_packet(user->sockfd, &(packet.type), &(packet.tag),
                &(packet.payload_len), &pData);
    if ((packet.type != FROMSERV_TYPE_ACK) ||
        (packet.tag != FROMSERV_TAG_OKAY)) {
        fprintf(
            stdout,
            "Something wrong when sending OPTIONS to Server in Create Vote\n");
    }
    free(pData);
    pData = NULL;

    // get and send the names of per Option to Server
    for (int i = 0; i < num_candidates; ++i) {
        while (1) {
            memset(inputbuf, 0, MAX_MESSAGE_LENGTH);
            fprintf(stdout, "Please Enter the name of OPTION <%d>\n", i);
            if (fgets(inputbuf, MAX_NAME_LENGTH, stdin) == NULL) {
                fprintf(stderr, "Error reading input\n");
                continue;
            }
            // Check if the input is non-empty and within the desired length
            if (strlen(inputbuf) > 0 && strlen(inputbuf) <= MAX_NAME_LENGTH) {
                break;
            } else {
                fprintf(stdout,
                        "Invalid input. Name should be non-empty and up to %d "
                        "characters.\n",
                        MAX_NAME_LENGTH);
            }
        }
        send_packet(user->sockfd, TOSERV_TYPE_CREATE, TOSERV_TAG_OPTIONNAME,
                    strlen(inputbuf), inputbuf);
        recv_packet(user->sockfd, &(packet.type), &(packet.tag),
                    &(packet.payload_len), &pData);
        if ((packet.type != FROMSERV_TYPE_ACK) ||
            (packet.tag != FROMSERV_TAG_OKAY)) {
            fprintf(stdout,
                    "Something wrong when sending option name <%d> to Server "
                    "in Create Vote\n",
                    i);
        }
        free(pData);
        pData = NULL;
    }

    // get and send the Duration of the vote to Server
    while (1) {
        duration = 0;
        fprintf(stdout, "Please Enter the duration of vote in Mins\n");
        if (fgets(inputbuf, sizeof(inputbuf), stdin) == NULL) {
            fprintf(stderr, "Error reading input\n");
            continue;
        }
        ret = sscanf(inputbuf, "%u", &duration);
        if ((ret != 1) || (duration <= 0)) {
            fprintf(stderr,
                    "Invalid input. Duration should be longer than 0\n");
        } else {
            break;
        }
    }
    send_packet(user->sockfd, TOSERV_TYPE_CREATE, TOSERV_TAG_DURATION,
                sizeof(duration), &duration);
    recv_packet(user->sockfd, &(packet.type), &(packet.tag),
                &(packet.payload_len), &pData);
    if ((packet.type != FROMSERV_TYPE_ACK) ||
        (packet.tag != FROMSERV_TAG_OKAY)) {
        fprintf(
            stdout,
            "Something wrong when sending Duration to Server in Create Vote\n");
    }
    free(pData);
    pData = NULL;

    fprintf(stdout, "Create the Vote event completed!\n");
}

void client_cmd_view_inporg(UserInfo_t *user)
{
    PktHdr_t packet;
    void *pData = NULL;
    uint8_t num_events = 0;

    send_packet(user->sockfd, TOSERV_TYPE_VIEW, TOSERV_TAG_INPORG, 0, NULL);
    recv_packet(user->sockfd, &(packet.type), &(packet.tag),
                &(packet.payload_len), &pData);

    if ((packet.type == FROMSERV_TYPE_ACK) &&
        (packet.tag == FROMSERV_TAG_OKAY)) {
        num_events = *(uint8_t *)pData;
    }
    free(pData);
    pData = NULL;

    // recv the event information
    for (int i = 0; i < num_events; ++i) {
        recv_packet(user->sockfd, &(packet.type), &(packet.tag),
                    &(packet.payload_len), &pData);
        fprintf(stdout, "%s", (char *)pData);
        free(pData);
        pData = NULL;
        send_packet(user->sockfd, TOSERV_TYPE_VIEW, TOSERV_TAG_OKAY, 0, NULL);
    }

    recv_packet(user->sockfd, &(packet.type), &(packet.tag),
                &(packet.payload_len), &pData);

    if ((packet.type != FROMSERV_TYPE_ACK) ||
        (packet.tag != FROMSERV_TAG_OKAY)) {
        fprintf(stdout,
                "Something wrong when tearing down the sending options\n");
    }
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
    char *token, *dummy = user->sendline;
    token = strtok(dummy, " \n");
    if (token != NULL) {
        uint8_t cmd_index = 0;
        int ret = sscanf(token, "%hhu", &cmd_index);
        if ((ret != 1) || (cmd_index < 0) || (cmd_index > CMD_MAX)) {
            fprintf(stderr,
                    "No command provided. CMD Number should be in the 1 ~ %d\n",
                    CMD_MAX);
            func_table[0](user);
        } else {
            func_table[cmd_index](user);
        }
    } else {
        printf("No command provided.\n");
        func_table[0](user);
    }

    fprintf(stdout, "\e[32;01m%s\e[33;01m>\e[0m ", user->username);
}