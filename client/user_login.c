#include "user_login.h"
#include "error.h"

void user_login(int sockfd, char *username)
{
    for (int i = 0; i < 3; i++) {
        char password[100] = {0};
        char Isverify[10] = {0};

        fprintf(stdout, "Please enter the username: ");
        int ret = fscanf(stdin, "%s", username);
        if (ret == -1) {
            DEBUG_INFO("<fscanf> did not work");
        }
        fprintf(stdout, "Please enter the password: ");
        ret = fscanf(stdin, "%s", password);
        if (ret == -1) {
            DEBUG_INFO("<fscanf> did not work");
        }
        send(sockfd, username, strlen(username), 0);
        DELAY_MS(500);
        send(sockfd, password, strlen(password), 0);
        recv(sockfd, Isverify, sizeof(Isverify), 0);
        printf("%s", Isverify);

        if (!strncmp("pass", Isverify, 4)) {
            printf("Correct\n");
            return;
        } else {
            printf("recv wrong because password\n");
        }

        if (i == 2)
            ERROR_INFO("Enter wrong password over 3 times\n");
    }
    return;
}