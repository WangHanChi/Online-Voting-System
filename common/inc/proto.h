#pragma once

#include <stdint.h>
#include <stdlib.h>

typedef enum {
    TOSERV_TYPE_LOGIN = 0,
    TOSERV_TYPE_CREATE,
    TOSERV_TYPE_VIEW,
    TOSERV_TYPE_SELECT,
    TOSERV_TYPE_LOGOUT,
    FROMSERV_TYPE_ACK,
    FROMSERV_TYPE_RESULT
} PktType_t;

typedef enum {
    TOSERV_TAG_USERNAME = 0,
    TOSERV_TAG_PASSWORD,
    TOSERV_TAG_TITLE,
    TOSERV_TAG_OPTIONS,
    TOSERV_TAG_DURATION,
    TOSERV_TAG_EVENT,
    TOSERV_TAG_INPORG,
    TOSERV_TAG_RESULT,
    FROMSERV_TAG_OKAY,
    FROMSERV_TAG_FAIL,
    FROMSERV_TAG_EVENTS,
    FROMSERV_TAG_OPTIONS,
    FROMSERV_TAG_HISTORY,
} PktTag_t;

typedef struct {
    int type;
    uint32_t tag;
    size_t payload_len;
} PktHdr_t;
