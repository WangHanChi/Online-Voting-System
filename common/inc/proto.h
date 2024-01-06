#pragma once

#include <stdint.h>
#include <stdlib.h>

#define MAX_NAME_LENGTH 50
#define MAX_MESSAGE_LENGTH 80
#define MAX_VOTE_EVENTS 10
#define MAX_OPTIONS 10

typedef enum {
    TOSERV_TYPE_CMD = 0,
    TOSERV_TYPE_LOGIN,
    TOSERV_TYPE_CREATE,
    TOSERV_TYPE_VIEW,
    TOSERV_TYPE_SELECT,
    TOSERV_TYPE_HOSTORY,
    TOSERV_TYPE_LOGOUT,
    FROMSERV_TYPE_ACK,
    FROMSERV_TYPE_RESULT,
    TYPEMAX,
} PktType_t;

typedef enum {
    TOSERV_TAG_CMD = 0,
    TOSERV_TAG_USERNAME,
    TOSERV_TAG_PASSWORD,
    TOSERV_TAG_TITLE,
    TOSERV_TAG_OPTIONS,
    TOSERV_TAG_OPTIONNAME,
    TOSERV_TAG_DURATION,
    TOSERV_TAG_EVENT,
    TOSERV_TAG_EVENTID,
    TOSERV_TAG_INPORG,
    TOSERV_TAG_RESULT,
    TOSERV_TAG_HOSTORY,
    TOSERV_TAG_OKAY,
    TOSERV_TAG_FAIL,
    TOSERV_TAG_VOTE,
    TOSERV_TAG_NOTVOTE,
    FROMSERV_TAG_OKAY,
    FROMSERV_TAG_FAIL,
    FROMSERV_TAG_TIMEOUT,
    FROMSERV_TAG_EVENTS,
    FROMSERV_TAG_EVENTSID,
    FROMSERV_TAG_OPTIONS,
    FROMSERV_TAG_HISTORY,
    TAGMAX,
} PktTag_t;

typedef struct {
    int type;
    uint32_t tag;
    size_t payload_len;
} PktHdr_t;

/* userinfo struct */
typedef struct {
    char username[MAX_NAME_LENGTH];
    char sendline[MAX_MESSAGE_LENGTH];
    int sockfd;
    char *server_addr;
} UserInfo_t;

/* voting struct */
typedef struct {
    char title[MAX_NAME_LENGTH];
    uint8_t num_options;
    char option_name[MAX_OPTIONS][MAX_NAME_LENGTH];
    uint32_t votes[MAX_OPTIONS];
    int duration;
    uint32_t id;
} VoteEvent_t;

/* metadata of voting events */
typedef struct {
    VoteEvent_t events[MAX_VOTE_EVENTS];
    uint8_t num_events;
} VotingSystem_t;
