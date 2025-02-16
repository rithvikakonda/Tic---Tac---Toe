#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


#define Port 8087
#define isize 1024
#define csize 4095
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

void PlayGame(int fd_client){
    // printf("cameeee!!\n");

    char t_Board[3][3];
    char move[5];
    char content[csize] = {0} ;
    // printf("helloooo\n");
    // recv(fd_client,t_Board,sizeof(t_Board),0);
    // PrintBoard(t_Board);

    while(1){
        char seetype[6];

        recv(fd_client,seetype,sizeof(seetype),0);
        // printf("seetypeeee: %s\n",seetype);

        if(strcmp(seetype,"BOARD")==0){
            recv(fd_client,t_Board,sizeof(t_Board),0);
            PrintBoard(t_Board);

        }
        else if(strcmp(seetype,"MSSGG")==0){
            recv(fd_client,content,csize,0);
            printf("%s\n",content);
            break;
        }

        // recv(fd_client,t_Board,sizeof(t_Board),0);
        // if (strstr(t_Board, "won") != NULL || strstr(t_Board, "draw") != NULL) {
        //     printf("%s\n", t_Board);
        //     // return ;
        //     break;
        // }


        // PrintBoard(t_Board);
        // fflush(stdout);
        recv(fd_client,content,sizeof(content),0);
        printf("%s\n",content);
        memset(content, 0, csize);
        // printf("Enter your move (row col): ");
        fgets(move, sizeof(move), stdin);

       
        // recv(fd_client,t_Board,sizeof(t_Board),0);
        //  printf("Now updated Board : \n");



        send(fd_client,move,strlen(move),0);
        printf("Your move has sent.\n");
        memset(content, 0, csize);
        memset(seetype,0,6);
        recv(fd_client,seetype,sizeof(seetype),0);
        //  printf("checking!!!: \n");
        printf("%s\n",seetype);

        if(strcmp(seetype,"BOARD")==0){
            recv(fd_client,t_Board,sizeof(t_Board),0);
            PrintBoard(t_Board);

        }
          recv(fd_client,seetype,sizeof(seetype),0);
        // if(strcmp(seetype,"MSSG")==0){
        recv(fd_client, content, csize, 0);
        // printf("%s\n",content);
        if (strstr(content, "won") != NULL || strstr(content, "draw") != NULL) {
            printf("%s\n", content);
            // return ;
            break;
        }

       else  if(strstr(content,"invalid move") !=NULL){
            printf("%s\n",content);
        }  
        else{
            printf("%s\n", content);
        }  
        // }   
    }

    return;




    }






int WhetherToReplay(int fd_client){
    char content[csize] = {0};
    char replayReply[15];

    memset(content,0,csize);
    // printf("enteerr\n");
    recv(fd_client,content,csize,0);
    printf("**%s**",content);
     
    fgets(replayReply,sizeof(replayReply),stdin);

    send(fd_client,replayReply,strlen(replayReply),0);

    memset(content,0,csize);

    recv(fd_client , content, sizeof(content),0);

    printf("%s\n",content);


    if(strncmp(content,"Both players agreed to play the game again.",44) == 0){
        return 1;
    }else{
        return 0;
    }

}

void StartPlayingGame(int fd_client){

    int replay = 1;

    while(replay){
        // printf("here!!\n");
        PlayGame(fd_client);
        replay = WhetherToReplay(fd_client);
    }



}

int main(){
    struct sockaddr_in server_addr;
    // socklen_t addr_size;

    char content[csize] = {0};
    char move[10];
    char ttt_b[3][3];

    // addr_size = sizeof(server_addr);
    int fd_client ;
    fd_client = socket(AF_INET, SOCK_STREAM, 0) ; // TCP
    if(fd_client < 0){
        perror("Socket error !!");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(Port);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        printf("Invalid address or Address not supported \n");
        return -1;
    }

    if(connect(fd_client ,(struct sockaddr*)&server_addr,sizeof(server_addr)) < 0){
        printf("Connection Failed\n");
    }
    printf("Connected to server. Waiting for the game to start...\n");

    StartPlayingGame(fd_client);

    close(fd_client);
    return 0;


    // while(1){
    //     recv(fd_client, ttt_b, sizeof(ttt_b), 0);
    //     PrintBoard(ttt_b);
    //     printf("Enter your move (row col): ");
    //     fgets(move, sizeof(move), stdin);

    //     send(fd_client, move, strlen(move), 0);

    //     memset(content, 0, csize);
    //     recv(fd_client, content, csize, 0);
    //     if (strstr(content, "won") != NULL || strstr(content, "draw") != NULL) {
    //         printf("%s\n", content);
    //         break;
    //     }

    //     if(strstr(content,"invalid move") !=NULL){
    //         printf("%s\n",content);
    //     }       
    // }

















}