#include "handler.h"
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include "error.h"
#include "history.h"
#include "login.h"
#include "proto_lib.h"

#define UPDATE_PERIOD 100000
static uint32_t UUID = 0;

typedef struct {
    pthread_t worker;
    pthread_mutex_t md_mutex;
    pthread_mutex_t ht_mutex;
    FILE *history_fp;
    VotingSystem_t *metadata;
} _Handler;

static void *_handle_worker(void *hdl_obj)
{
    fprintf(stdout, "Hander init success\n");
    _Handler *obj = (_Handler *)hdl_obj;
    VotingSystem_t *metadata = (VotingSystem_t *)obj->metadata;
    time_t prev_time, cur_time;
    prev_time = time(NULL);
    while (1) {
        cur_time = time(NULL);
        int time_diff = (int)(cur_time - prev_time);
        pthread_mutex_lock(&obj->md_mutex);
        for (int num = 0; num < metadata->num_events; ++num) {
            metadata->events[num].duration -= time_diff;
            if (metadata->events[num].duration <= 0) {
                pthread_mutex_lock(&obj->ht_mutex);
                obj->history_fp = fopen(HISTORY_FILENAME, "a");
                char *data = malloc(sizeof(char) * 1024);

                fprintf(obj->history_fp,
                        "============================================\n");
                fprintf(obj->history_fp, "Title : %s\n",
                        metadata->events[num].title);
                fprintf(obj->history_fp, "The result is as following:\n");
                int top = 0;
                for (int i = 0; i < metadata->events[num].num_options; ++i) {
                    fprintf(obj->history_fp, "%s : %u\n",
                            metadata->events[num].option_name[i],
                            metadata->events[num].votes[i]);
                    if (metadata->events[num].votes[i] >
                        metadata->events[num].votes[top]) {
                        top = i;
                    }
                }
                fprintf(obj->history_fp, "\nThe highest result is: %s\n",
                        metadata->events[num].option_name[top]);
                fprintf(obj->history_fp,
                        "============================================\n\n");
                fclose(obj->history_fp);
                pthread_mutex_unlock(&obj->ht_mutex);
                memmove(&metadata->events[num], &metadata->events[num + 1],
                        (metadata->num_events - num - 1) * sizeof(VoteEvent_t));
                metadata->num_events--;
                num--;
            }
        }
        pthread_mutex_unlock(&obj->md_mutex);
        // fprintf(stdout, "passed %ld seconds\n", cur_time - prev_time);
        prev_time = cur_time;
        usleep(UPDATE_PERIOD);
    }
    return NULL;
}

void server_handler_help(int connfd, void *hdl_obj)
{
    fprintf(stdout, "server_handler_help WIP~\n");
}

void server_handler_create_vote(int connfd, void *hdl_obj)
{
    _Handler *obj = (_Handler *)hdl_obj;
    VotingSystem_t *metadata = (VotingSystem_t *)(obj->metadata);
    VoteEvent_t event = {0};
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
    strncpy(event.title, pData, packet.payload_len);
    trim_string(event.title);
    fprintf(stdout, "The title is %s\n", event.title);
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
    event.num_options = *(uint8_t *)pData;
    fprintf(stdout, "The number of option is %hhu\n", event.num_options);
    send_packet(connfd, FROMSERV_TYPE_ACK, FROMSERV_TAG_OKAY, 0, NULL);
    free(pData);
    pData = NULL;

    // recv the names of per Option from client
    for (int i = 0; i < event.num_options; ++i) {
        recv_packet(connfd, &(packet.type), &(packet.tag),
                    &(packet.payload_len), &pData);
        if ((packet.type != TOSERV_TYPE_CREATE) ||
            (packet.tag != TOSERV_TAG_OPTIONNAME)) {
            fprintf(stdout,
                    "Something wrong when receiving name of option from Client "
                    "in Create "
                    "Vote\n");
        }
        strncpy(event.option_name[i], pData, packet.payload_len);
        trim_string(event.option_name[i]);
        fprintf(stdout, "The the name of OPTION <%d> is %s\n", i,
                event.option_name[i]);
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
    event.duration = *(uint32_t *)pData * 60;
    fprintf(stdout, "The Duration is %u\n", event.duration);
    send_packet(connfd, FROMSERV_TYPE_ACK, FROMSERV_TAG_OKAY, 0, NULL);
    free(pData);
    pData = NULL;

    fprintf(stdout, "Completed\n");
    pthread_mutex_lock(&obj->md_mutex);
    metadata->events[metadata->num_events].id = UUID++;
    memcpy((void *)&metadata->events[metadata->num_events], (void *)&event,
           sizeof(VoteEvent_t));
    ++metadata->num_events;
    pthread_mutex_unlock(&obj->md_mutex);
}

void server_handler_view_inporg(int connfd, void *hdl_obj)
{
    _Handler *obj = (_Handler *)hdl_obj;
    VotingSystem_t *metadata = (VotingSystem_t *)(obj->metadata);
    PktHdr_t packet;
    void *pData = NULL;
    recv_packet(connfd, &(packet.type), &(packet.tag), &(packet.payload_len),
                &pData);
    if ((packet.type == TOSERV_TYPE_VIEW) &&
        (packet.tag == TOSERV_TAG_INPORG)) {
        send_packet(connfd, FROMSERV_TYPE_ACK, FROMSERV_TAG_OKAY,
                    sizeof(metadata->num_events), &metadata->num_events);
    }

    if (metadata->num_events == 0)
        return;

    char *data;

    pthread_mutex_lock(&obj->md_mutex);  // This part should be modified!
    for (int num = 0; num < metadata->num_events; ++num) {
        data = malloc(sizeof(char) * 1024);
        sprintf(data, "============================================\n");
        sprintf(data, "%sTitle : %s\n", data, metadata->events[num].title);
        sprintf(data, "%sThe option is as following:\n", data);
        for (int i = 0; i < metadata->events[num].num_options; ++i) {
            sprintf(data, "%s(%d) : %s\t\t| Vote : %u\n", data, i,
                    metadata->events[num].option_name[i],
                    metadata->events[num].votes[i]);
        }
        uint32_t duration = metadata->events[num].duration;
        if (duration < 60) {
            sprintf(data, "%sThe remaining time is %u s\n", data, duration);
        } else {
            sprintf(data, "%sThe remaining time is %u Mins\n", data,
                    duration / 60);
        }
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
    pthread_mutex_unlock(&obj->md_mutex);

    send_packet(connfd, FROMSERV_TYPE_ACK, FROMSERV_TAG_OKAY, 0, NULL);

    // get user want to vote or not
    recv_packet(connfd, &(packet.type), &(packet.tag), &(packet.payload_len),
                &pData);
    if ((packet.type == TOSERV_TYPE_SELECT) &&
        (packet.tag == TOSERV_TAG_VOTE)) {
        uint8_t select_event = *(uint8_t *)pData;
        uint32_t event_id = metadata->events[select_event].id;
        DEBUG_INFO("event_id is %u\n", event_id);
        free(pData);
        pData = NULL;
        send_packet(connfd, FROMSERV_TYPE_ACK, FROMSERV_TAG_OKAY,
                    sizeof(metadata->events[select_event].num_options),
                    &(metadata->events[select_event].num_options));
        send_packet(connfd, FROMSERV_TYPE_ACK, FROMSERV_TAG_EVENTSID,
                    sizeof(select_event), &select_event);

        recv_packet(connfd, &(packet.type), &(packet.tag),
                    &(packet.payload_len), &pData);
        if ((packet.type == TOSERV_TYPE_SELECT) &&
            (packet.tag == TOSERV_TAG_VOTE)) {
            uint8_t select_option = *(uint8_t *)pData;
            free(pData);
            pData = NULL;
            recv_packet(connfd, &(packet.type), &(packet.tag),
                        &(packet.payload_len), &pData);
            if ((packet.type == TOSERV_TYPE_SELECT) &&
                (packet.tag == TOSERV_TAG_EVENTID)) {
                pthread_mutex_lock(
                    &obj->md_mutex);  // This part should be modified!
                if (event_id == *(uint32_t *)pData) {
                    metadata->events[select_event].votes[select_option]++;
                    free(pData);
                    pData = NULL;
                } else {
                    char timeout_info[] =
                        "\nThis event CANNOT be voted because of timeout!!\n";
                    DEBUG_INFO("%s", timeout_info);
                    send_packet(connfd, FROMSERV_TYPE_ACK, FROMSERV_TAG_TIMEOUT,
                                strlen(timeout_info), timeout_info);
                }
                pthread_mutex_unlock(
                    &obj->md_mutex);  // This part should be modified!
            }
        } else {
            fprintf(stderr, "Type or Tag is wrong when receiving options\n");
            if (packet.payload_len > 0) {
                free(pData);
                pData = NULL;
                send_packet(connfd, FROMSERV_TYPE_ACK, FROMSERV_TAG_FAIL, 0,
                            NULL);
            }
        }
    } else if ((packet.type == TOSERV_TYPE_SELECT) &&
               (packet.tag == TOSERV_TAG_NOTVOTE)) {
        send_packet(connfd, FROMSERV_TYPE_ACK, FROMSERV_TAG_OKAY, 0, NULL);
    } else
        fprintf(stderr, "Type or Tag is wrong when receiving options\n");
}

void server_handler_view_result(int connfd, void *hdl_obj)
{
    fprintf(stdout, "server_handler_view_result WIP~\n");
    _Handler *obj = (_Handler *)hdl_obj;
    // PktHdr_t packet;
    // void *pData = NULL;
    // recv_packet(connfd, &(packet.type), &(packet.tag), &(packet.payload_len),
    //             &pData);
    // if ((packet.type == TOSERV_TYPE_VIEW) &&
    //     (packet.tag == TOSERV_TAG_INPORG)) {
    //     send_packet(connfd, FROMSERV_TYPE_ACK, FROMSERV_TAG_OKAY,
    //                 sizeof(metadata.num_events), &metadata.num_events);
    // }

    // char *data;
    // for (int num = 0; num < metadata.num_events; ++num) {
    //     data = malloc(sizeof(char) * 1024);
    //     sprintf(data, "============================================\n");
    //     sprintf(data, "%sTitle : %s\n", data, metadata.events[num].title);
    //     sprintf(data, "%sThe option is as following:\n", data);
    //     for (int i = 0; i < metadata.events[num].num_options; ++i) {
    //         sprintf(data, "%s(%d) : %s\n", data, i,
    //                 metadata.events[num].option_name[i]);
    //     }
    //     sprintf(data, "%sThe remaining time is %u Mins\n", data,
    //             metadata.events[num].duration);
    //     sprintf(data, "%s============================================\n\n",
    //             data);
    //     send_packet(connfd, FROMSERV_TYPE_ACK, FROMSERV_TAG_OKAY,
    //     strlen(data),
    //                 data);
    //     free(data);
    //     recv_packet(connfd, &(packet.type), &(packet.tag),
    //                 &(packet.payload_len), &pData);
    //     if ((packet.type != TOSERV_TYPE_VIEW) ||
    //         (packet.tag != TOSERV_TAG_OKAY)) {
    //         fprintf(stdout, "Something wrong in send option <%d>\n", num);
    //     }
    // }
}

void server_handler_quit(int connfd, void *hdl_obj)
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

typedef void (*func)(int, void *);

const func func_table[] = {
#define _(HANDLER, handler) server_handler_##handler,
    SERVER_HANDLER_LIST
#undef _
};

void server_select_cmd(int connfd, char *sendline, void *hdl_obj)
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
            func_table[cmd_index](connfd, hdl_obj);
        }
    } else {
        printf("No command provided.\n");
    }
}

int handler_init(void **hdl_obj)
{
    int ret = 0;
    int init_step = 0;
    *hdl_obj = calloc(1, sizeof(_Handler));
    if (*hdl_obj) {
        _Handler *obj = (_Handler *)*hdl_obj;
        do {
            ret = pthread_mutex_init(&obj->md_mutex, 0);
            if (ret) {
                break;
            }
            init_step++;
            ret = pthread_mutex_init(&obj->ht_mutex, 0);
            if (ret) {
                break;
            }
            init_step++;
            obj->metadata = (VotingSystem_t *)calloc(1, sizeof(VotingSystem_t));
            if (!obj->metadata) {
                ret = 1;
                break;
            }
            init_step++;
            obj->history_fp = fopen(HISTORY_FILENAME, "w");
            if (obj->history_fp == NULL) {
                fprintf(stderr, "History file does not exist\n");
                ret = 1;
                break;
            }
            fclose(obj->history_fp);
            init_step++;
            ret = pthread_create(&obj->worker, NULL, _handle_worker, obj);
            if (ret)
                break;
            init_step++;
        } while (0);
    } else {
        ret = 1;
    }
    if (ret) {
        if (*hdl_obj) {
            _Handler *obj = (_Handler *)*hdl_obj;
            switch (init_step) {
            case 4:
                obj->history_fp = NULL;
            case 3:
                free(obj->metadata);
            case 2:
                pthread_mutex_destroy(&obj->ht_mutex);
            case 1:
                pthread_mutex_destroy(&obj->md_mutex);
            default:
                free(obj);
            }
        }
        *hdl_obj = NULL;
    }
    return ret;
}

void hander_delete(void *hdl_obj)
{
    if (hdl_obj != NULL) {
        _Handler *obj = (_Handler *)hdl_obj;
        assert(obj != NULL);
        pthread_join(obj->worker, NULL);
        pthread_mutex_destroy(&obj->md_mutex);
        pthread_mutex_destroy(&obj->ht_mutex);
        free(obj->metadata);
        obj->history_fp = NULL;
        free(hdl_obj);
    }
}