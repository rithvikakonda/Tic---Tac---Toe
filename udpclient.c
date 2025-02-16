

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8086
#define ISIZE 1024
#define CSIZE 4095

void PrintBoard(char b[3][3]){
    printf("Current state of Board : \n");
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            printf("  %c |",b[i][j]);
        }
        printf("\n");
        if(i !=2){
            printf(" ---| ---| ---|\n");
        }
    }


}

void PlayGame(int sockfd, struct sockaddr_in *server_addr, socklen_t addr_size) {
    //  printf("cameeee!!\n");
    char t_Board[3][3];
    char move[5];
    char content[CSIZE] = {0};
    char seetype[6];
    // printf("helloooo\n");

    while (1) {
        recvfrom(sockfd, seetype, sizeof(seetype), 0, (struct sockaddr *)server_addr, &addr_size);

        if (strcmp(seetype, "BOARD") == 0) {
            recvfrom(sockfd, t_Board, sizeof(t_Board), 0, (struct sockaddr *)server_addr, &addr_size);
            PrintBoard(t_Board);
        } else if (strcmp(seetype, "MSSGG") == 0) {
            recvfrom(sockfd, content, CSIZE, 0, (struct sockaddr *)server_addr, &addr_size);
            printf("%s\n", content);
            break;
        }

        recvfrom(sockfd, content, sizeof(content), 0, (struct sockaddr *)server_addr, &addr_size);
        printf("%s\n", content);
        memset(content, 0, CSIZE);

        fgets(move, sizeof(move), stdin);
        sendto(sockfd, move, strlen(move), 0, (struct sockaddr *)server_addr, addr_size);
        printf("Your move has been sent.\n");

        memset(content, 0, CSIZE);
        memset(seetype, 0, 6);
        recvfrom(sockfd, seetype, sizeof(seetype), 0, (struct sockaddr *)server_addr, &addr_size);
        // printf("%s\n",seetype);

        if (strcmp(seetype, "BOARD") == 0) {
            recvfrom(sockfd, t_Board, sizeof(t_Board), 0, (struct sockaddr *)server_addr, &addr_size);
            PrintBoard(t_Board);
        }

        recvfrom(sockfd, seetype, sizeof(seetype), 0, (struct sockaddr *)server_addr, &addr_size);
        recvfrom(sockfd, content, CSIZE, 0, (struct sockaddr *)server_addr, &addr_size);

        if (strstr(content, "won") != NULL || strstr(content, "draw") != NULL) {
            printf("%s\n", content);
            break;
        } else if (strstr(content, "invalid move") != NULL) {
            //  printf("iiiiii\n");
            printf("%s\n", content);
        } else {
            // printf("jkhfnk");
            printf("%s\n", content);
        }
    }
}

int WhetherToReplay(int sockfd, struct sockaddr_in *server_addr, socklen_t addr_size) {
    char content[CSIZE] = {0};
    char replayReply[15];

    memset(content, 0, CSIZE);
    //  printf("enteerr\n");
    recvfrom(sockfd, content, CSIZE, 0, (struct sockaddr *)server_addr, &addr_size);
    printf("**%s**", content);

    fgets(replayReply, sizeof(replayReply), stdin);
    sendto(sockfd, replayReply, strlen(replayReply), 0, (struct sockaddr *)server_addr, addr_size);

    memset(content, 0, CSIZE);
    recvfrom(sockfd, content, CSIZE, 0, (struct sockaddr *)server_addr, &addr_size);

    printf("%s\n", content);
    if (strncmp(content, "Both players agreed to play the game again.", 44) == 0) {
        return 1;
    } else {
        return 0;
    }
}

void StartPlayingGame(int sockfd, struct sockaddr_in *server_addr, socklen_t addr_size) {
    int replay = 1;

    while (replay) {
        // printf("here!!\n");
        PlayGame(sockfd, server_addr, addr_size);
        replay = WhetherToReplay(sockfd, server_addr, addr_size);
    }
}

int main() {
    struct sockaddr_in server_addr;
    char content[CSIZE] = {0};
    char move[10];
    char ttt_b[3][3];
    int sockfd;
    socklen_t addr_size = sizeof(server_addr);

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket error!!");
        exit(1);
    }

    // Server address setup
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        printf("Invalid address or Address not supported\n");
        return -1;
    }

    // Send initial connection message
    char init_msg[] = "Player connected";
    sendto(sockfd, init_msg, sizeof(init_msg), 0, (struct sockaddr *)&server_addr, addr_size);

    printf("Connected to server. Waiting for the game to start...\n");
    StartPlayingGame(sockfd, &server_addr, addr_size);

    close(sockfd);
    return 0;
}
