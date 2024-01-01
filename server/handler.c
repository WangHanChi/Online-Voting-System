#include "handler.h"
#include "error.h"
#include "login.h"
#include "proto_lib.h"

VotingSystem_t metadata = {0};

void server_handler_help(int connfd, char *token)
{
    fprintf(stdout, "server_handler_help WIP~\n");
}

void server_handler_create_vote(int connfd, char *token)
{
    PktHdr_t packet;
    uint8_t num_candidates;
    uint32_t duration;
    void *pData = NULL;

    // recv username from client
    recv_packet(connfd, &(packet.type), &(packet.tag), &(packet.payload_len),
                &pData);
    if ((packet.type != TOSERV_TYPE_CREATE) ||
        (packet.tag != TOSERV_TAG_USERNAME)) {
        fprintf(stdout,
                "Something wrong when receiving username from Client in Create "
                "Vote\n");
    }
    fprintf(stdout, "The login user is %s\n", (char *)pData);
    send_packet(connfd, FROMSERV_TYPE_ACK, FROMSERV_TAG_OKAY, 0, NULL);
    free(pData);
    pData = NULL;

    // recv Title from client
    recv_packet(connfd, &(packet.type), &(packet.tag), &(packet.payload_len),
                &pData);
    if ((packet.type != TOSERV_TYPE_CREATE) ||
        (packet.tag != TOSERV_TAG_TITLE)) {
        fprintf(stdout,
                "Something wrong when receiving Title from Client in Create "
                "Vote\n");
    }
    strncpy(metadata.events[metadata.num_events].title, pData,
            packet.payload_len);
    trim_string(metadata.events[metadata.num_events].title);
    fprintf(stdout, "The title is %s\n",
            metadata.events[metadata.num_events].title);
    send_packet(connfd, FROMSERV_TYPE_ACK, FROMSERV_TAG_OKAY, 0, NULL);
    free(pData);
    pData = NULL;


    // recv number of Options from client
    recv_packet(connfd, &(packet.type), &(packet.tag), &(packet.payload_len),
                &pData);
    if ((packet.type != TOSERV_TYPE_CREATE) ||
        (packet.tag != TOSERV_TAG_OPTIONS)) {
        fprintf(stdout,
                "Something wrong when receiving OPTIONS from Client in Create "
                "Vote\n");
    }
    metadata.events[metadata.num_events].num_options = *(uint8_t *)pData;
    fprintf(stdout, "The number of option is %hhu\n",
            metadata.events[metadata.num_events].num_options);
    send_packet(connfd, FROMSERV_TYPE_ACK, FROMSERV_TAG_OKAY, 0, NULL);
    free(pData);
    pData = NULL;

    // recv the names of per Option from client
    for (int i = 0; i < metadata.events[metadata.num_events].num_options; ++i) {
        recv_packet(connfd, &(packet.type), &(packet.tag),
                    &(packet.payload_len), &pData);
        if ((packet.type != TOSERV_TYPE_CREATE) ||
            (packet.tag != TOSERV_TAG_OPTIONNAME)) {
            fprintf(stdout,
                    "Something wrong when receiving name of option from Client "
                    "in Create "
                    "Vote\n");
        }
        strncpy(metadata.events[metadata.num_events].option_name[i], pData,
                packet.payload_len);
        trim_string(metadata.events[metadata.num_events].option_name[i]);
        fprintf(stdout, "The the name of OPTION <%d> is %s\n", i,
                metadata.events[metadata.num_events].option_name[i]);
        send_packet(connfd, FROMSERV_TYPE_ACK, FROMSERV_TAG_OKAY, 0, NULL);
        free(pData);
        pData = NULL;
    }


    // recv duration from client
    recv_packet(connfd, &(packet.type), &(packet.tag), &(packet.payload_len),
                &pData);
    if ((packet.type != TOSERV_TYPE_CREATE) ||
        (packet.tag != TOSERV_TAG_DURATION)) {
        fprintf(stdout,
                "Something wrong when receiving Duration from Client in Create "
                "Vote\n");
    }
    metadata.events[metadata.num_events].duration = *(uint32_t *)pData;
    fprintf(stdout, "The Duration is %u\n",
            metadata.events[metadata.num_events].duration);
    send_packet(connfd, FROMSERV_TYPE_ACK, FROMSERV_TAG_OKAY, 0, NULL);
    free(pData);
    pData = NULL;

    fprintf(stdout, "Completed\n");
    ++metadata.num_events;
}

void server_handler_view_inporg(int connfd, char *token)
{
    fprintf(stdout, "server_handler_view_inporg WIP~\n");

    PktHdr_t packet;
    void *pData = NULL;
    recv_packet(connfd, &(packet.type), &(packet.tag), &(packet.payload_len),
                &pData);
    if ((packet.type == TOSERV_TYPE_VIEW) &&
        (packet.tag == TOSERV_TAG_INPORG)) {
        send_packet(connfd, FROMSERV_TYPE_ACK, FROMSERV_TAG_OKAY,
                    sizeof(metadata.num_events), &metadata.num_events);
    }

    char *data;
    for (int num = 0; num < metadata.num_events; ++num) {
        data = malloc(sizeof(char) * 1024);
        sprintf(data, "============================================\n");
        sprintf(data, "%sTitle : %s\n", data, metadata.events[num].title);
        sprintf(data, "%sThe option is as following:\n", data);
        for (int i = 0; i < metadata.events[num].num_options; ++i) {
            sprintf(data, "%s(%d) : %s\n", data, i,
                    metadata.events[num].option_name[i]);
        }
        sprintf(data, "%sThe remaining time is %u Mins\n", data,
                metadata.events[num].duration);
        sprintf(data, "%s============================================\n\n",
                data);
        send_packet(connfd, FROMSERV_TYPE_ACK, FROMSERV_TAG_OKAY, strlen(data),
                    data);
        free(data);
        recv_packet(connfd, &(packet.type), &(packet.tag),
                    &(packet.payload_len), &pData);
        if ((packet.type != TOSERV_TYPE_VIEW) ||
            (packet.tag != TOSERV_TAG_OKAY)) {
            fprintf(stdout, "Something wrong in send option <%d>\n", num);
        }
    }

    send_packet(connfd, FROMSERV_TYPE_ACK, FROMSERV_TAG_OKAY, 0, NULL);
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
        free(pData);
        pData = NULL;
        recv_packet(cli_sockfd, &(packet.type), &(packet.tag),
                    &(packet.payload_len), &pData);
        if ((packet.type == TOSERV_TYPE_LOGIN) &&
            (packet.tag == TOSERV_TAG_PASSWORD)) {
            strncpy(password, pData, packet.payload_len);
            DEBUG_INFO("recv password -> %s\n\n", password);
        }
        free(pData);
        pData = NULL;
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
        uint8_t cmd_index = 0;
        int ret = sscanf(token, "%hhu", &cmd_index);
        if ((ret != 1) || (cmd_index < 0) || (cmd_index > HANDLER_MAX)) {
            fprintf(stderr,
                    "No command provided. CMD Number should be in the 1 ~ %d\n",
                    HANDLER_MAX);
        } else {
            fprintf(stdout, "cmd_index is %d\n", cmd_index);
            func_table[cmd_index](connfd, sendline);
        }
    } else {
        printf("No command provided.\n");
    }
}