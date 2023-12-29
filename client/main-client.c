#include "command.h"
#include "error.h"
#include "text.h"
#include "user_login.h"

int main(int argc, char **argv)
{
    if (argc != 3) {
        ERROR_INFO(
            "Usage: ./client <IP address of the server> <port number>\n");
    }
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

    UserInfo_t *user = (UserInfo_t *)malloc(sizeof(UserInfo_t));
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

    while (fgets(sendline, MAX_MESSAGE_LENGTH, stdin) != NULL) {
        send(sockfd, sendline, MAX_MESSAGE_LENGTH, 0);
        strncpy(user->sendline, sendline, sizeof(user->sendline));
        client_select_cmd(user);
    }
    return 0;
}
