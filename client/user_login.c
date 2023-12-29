#include "user_login.h"
#include "error.h"
#include "proto_lib.h"

void user_login(UserInfo_t *user)
{
    PktHdr_t packet;
    void *pData = NULL;

    for (int i = 0; i < 3; i++) {
        char username[MAX_NAME_LENGTH] = {0};
        char password[MAX_NAME_LENGTH] = {0};

        fprintf(stdout, "Please enter the username: ");
        int ret = fscanf(stdin, "%s", username);
        if (ret == -1) {
            DEBUG_INFO("<fscanf> did not work");
        }
        send_packet(user->sockfd, TOSERV_TYPE_LOGIN, TOSERV_TAG_USERNAME,
                    strlen(username), username);
        fprintf(stdout, "Please enter the password: ");
        ret = fscanf(stdin, "%s", password);
        if (ret == -1) {
            DEBUG_INFO("<fscanf> did not work");
        }
        send_packet(user->sockfd, TOSERV_TYPE_LOGIN, TOSERV_TAG_PASSWORD,
                    strlen(password), password);
        recv_packet(user->sockfd, &(packet.type), &(packet.tag),
                    &(packet.payload_len), &pData);
        if ((packet.type == FROMSERV_TYPE_RESULT) &&
            (packet.tag == FROMSERV_TAG_OKAY)) {
            strncpy(user->username, username, strlen(username));
            DEBUG_INFO("User login sucessfully");
            return;
        } else {
            fprintf(stdout,
                    "login failed because username or password is wrong\n");
        }

        if (i == 2)
            ERROR_INFO("Enter wrong password over 3 times\nExit ...");
    }
    return;
}