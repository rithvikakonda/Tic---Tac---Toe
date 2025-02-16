#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define Port 8087
#define isize 1024
#define csize 4095

char ttt_board[3][3] =  {{'-', '-','-'}, {'-','-','-'}, {'-', '-', '-'}};

int check_winner(){
    for(int i = 0 ; i < 3 ; i++){
        if(ttt_board[i][0] == 'X' && ttt_board[i][1] == 'X' && ttt_board[i][2] == 'X') {
          return 1;
        }
        else if(ttt_board[i][0] == 'O' && ttt_board[i][1] == 'O' && ttt_board[i][2] == 'O'){
           return 1;
        }
    }
    for(int j = 0 ; j < 3 ; j++){
        if(ttt_board[0][j] == 'X' && ttt_board[1][j] == 'X' && ttt_board[2][j] == 'X') {
          return 1;
        }
        else if(ttt_board[0][j] == 'O' && ttt_board[1][j] == 'O' && ttt_board[2][j] == 'O'){
           return 1;
        }
    }

   if(ttt_board[0][0] == 'X' && ttt_board[1][1] == 'X' && ttt_board[2][2] == 'X' ){
    return 1;
   }
   if(ttt_board[0][0] == 'O' && ttt_board[1][1] == 'O' && ttt_board[2][2] == 'O' ){
    return 1;
   }
   if(ttt_board[2][0] == 'O' && ttt_board[1][1] == 'O' && ttt_board[0][2] == 'O' ){
    return 1;
   }
    if(ttt_board[2][0] == 'X' && ttt_board[1][1] == 'X' && ttt_board[0][2] == 'X' ){
    return 1;
   }
//    printf("hello\n");

   return 0;
}

int check_draw(){
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            if(ttt_board[i][j] == '-'){
                return 0;
            }
        }
    }
    // printf("draw entered\n");
    return 1;

}

void clear_board(){
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            ttt_board[i][j] = '-';
        }
    }
    return;
}

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

// void startgame(int c1 , int c2){
//     int present_player = 1;
//     char input[isize];
//     int winner = 0 , matchdraw = 0;

//     clear_board();
//     printf("Game Begins !!!\n");

//     PrintBoard(ttt_board);
//     int curr_socket;

//    // c1 = X and c2 = O
//     while(!winner && !matchdraw){
//         if(present_player == 1){
//             curr_socket = c1;
//         }else{
//             curr_socket = c2;
//         }
//         printf("presentpaleyr: %d\n",present_player);
//         PrintBoard(ttt_board);

//         send(curr_socket,ttt_board,sizeof(ttt_board),0);
//         recv(curr_socket,input,sizeof(input),0);

//         int rw = (input[0] - '0') - 1;
//         int cl = (input[2] - '0') - 1;

//         printf("Player %d selected position : (%d %d)\n",present_player,rw+1,cl+1);


//         if(ttt_board[rw][cl] == '-'){
//             if(present_player == 1){
//                 ttt_board[rw][cl] = 'X';
//             }else{
//                 ttt_board[rw][cl] = 'O';
//             }

//             PrintBoard(ttt_board);

//             winner = check_winner();
//             matchdraw = check_draw();

//             // send(c1,ttt_board,sizeof(ttt_board),0);
//             // send(c2,ttt_board,sizeof(ttt_board),0);

//             if(winner == 1){
//                 char win_mssg[csize];
//                 snprintf(win_mssg,sizeof(win_mssg),"Player %d won the Game !!!",present_player);
//                 send(c1,win_mssg,sizeof(win_mssg),0);
//                 send(c2,win_mssg,sizeof(win_mssg),0);
//             }
//             else if(matchdraw == 1){
//                 char draw_mssg[] = "It's a draw!!";
//                 send(c1,draw_mssg,sizeof(draw_mssg),0);
//                 send(c2,draw_mssg,sizeof(draw_mssg),0);
//             }
//             else{
//                 printf("else part\n");
//                 if(present_player == 1){
//                     present_player = 2;
//                 }else{
//                     present_player = 1;
//                 }
//             }
//         }
//         else{
//             printf("Player %d made an invalid move\n",present_player);
//             char message[csize] = "You made an invalid move , try again !!";
//             send(curr_socket,message,sizeof(message),0);
//         }

//     }

// }
int startgame(int c1 , int c2) {
    int present_player = 1;
    char input[isize];
    int winner = 0, matchdraw = 0;

    clear_board();
    printf("Game Begins !!!\n");

    PrintBoard(ttt_board);

    int curr_socket, next_socket;
    // send(c1, ttt_board, sizeof(ttt_board), 0);
    // send(c2, ttt_board, sizeof(ttt_board), 0);


    // c1 = X and c2 = O
    while (!winner && !matchdraw) {
        // Determine the current and next player's socket
        if (present_player == 1) {
            curr_socket = c1; // Current player
            next_socket = c2; // Opponent
        } else {
            curr_socket = c2; // Current player
            next_socket = c1; // Opponent
        }

        // Send the current state of the board to both players
        send(curr_socket,"BOARD",6,0);
        send(curr_socket, ttt_board, sizeof(ttt_board), 0);
        // send(c2, ttt_board, sizeof(ttt_board), 0);

        // Prompt the current player to make a move
        char prompt[] = "It's your turn. Enter your move (row col): ";
        send(curr_socket, prompt, sizeof(prompt), 0);

        // Receive input from the current player
        recv(curr_socket, input, sizeof(input), 0);
        int rw = (input[0] - '0') - 1;
        int cl = (input[2] - '0') - 1;

        printf("Player %d selected position: (%d, %d)\n", present_player, rw + 1, cl + 1);

        if ( rw >= 0 && rw <=2 && cl >= 0 && cl<= 2 && ttt_board[rw][cl] == '-' ) {
            if (present_player == 1) {
                ttt_board[rw][cl] = 'X';
            } else {
                ttt_board[rw][cl] = 'O';
            }

            PrintBoard(ttt_board);
            send(curr_socket, "BOARD", 6, 0);
            send(curr_socket, ttt_board, sizeof(ttt_board), 0);


            // Check for winner or draw after each move
            winner = check_winner();
            matchdraw = check_draw();

            if (winner == 1) {
                char win_mssg[csize];
                snprintf(win_mssg, sizeof(win_mssg), "Player %d won the game!", present_player);
                printf("Player %d won the Game !!\n",present_player);
                send(c1, "MSSGG", 6, 0);
                send(c1, win_mssg, sizeof(win_mssg), 0);
                send(c2, "MSSGG", 6, 0);
                send(c2, win_mssg, sizeof(win_mssg), 0);
                return present_player;
                break;
            } else if (matchdraw == 1) {
                char draw_mssg[] = "It's a draw!";
                printf("It's A Draw !!\n");
                 send(c1, "MSSGG", 6, 0);
                send(c1, draw_mssg, sizeof(draw_mssg), 0);
                 send(c2, "MSSGG", 6, 0);
                send(c2, draw_mssg, sizeof(draw_mssg), 0);
                return present_player;
                break;
            } else {
                char simple_msg[] = "Your move updated successfully";
                send(curr_socket, "MSSGG", 6, 0);
                send(curr_socket,simple_msg,sizeof(simple_msg),0);               // If no winner or draw, alternate to the next player
                present_player = (present_player == 1) ? 2 : 1;
            }
        } else {
            send(curr_socket, "BOARD", 6, 0);
            send(curr_socket, ttt_board, sizeof(ttt_board), 0);

            // Handle invalid move
            send(curr_socket, "MSSGG", 6, 0);
            char message[csize] = "Invalid move, try again!";
            send(curr_socket, message, sizeof(message), 0);
        }
    }
}


int askForReplay(int c){
    char ques[] = "Do you want to play again ?(yes/no) : ";
    char ans[csize] ;
    // printf("sending!\n");
    send(c,ques,sizeof(ques),0);
    // printf("sent\n");
    recv(c,ans,sizeof(ans),0);

    if(strncmp(ans,"yes",3) == 0){
        return 1;
    }else{
        return 0;
    }

    return 0;
}

void letsplaygame(int c1 , int c2){

    int Rc1 = 1 , Rc2 = 1;

    while(Rc1 == 1 && Rc2 == 1){
       int x =  startgame(c1,c2);

       if(x == 0 || x == 1){

        Rc1 = askForReplay(c1);
        Rc2 = askForReplay(c2);

       }else if (x == 2){
         Rc2 = askForReplay(c2);
         Rc1 = askForReplay(c1);
       }

        if(Rc1 == 1 && Rc2 == 1){
            send(c1,"Both players agreed to play the game again.",44,0);
            send(c2,"Both players agreed to play the game again.",44,0);
            
        }
        else if(Rc1!=1 && Rc2!=1){
            send(c1,"Both players do not want to play the game again.",49,0);
            send(c2,"Both players do not want to play the game again.",49,0);

        }
        else if(Rc1 != 1 && Rc2 == 1){
            send(c1,"You choose not to continue the Game.",37,0);
            send(c2,"Player 1 does not want to continue the Game.",45,0);

        }
        else if(Rc1 == 1 && Rc2 != 1){
            send(c2,"You choose not to continue the Game.",37,0);
            send(c1,"Player 2 does not want to continue the Game.",45,0);

        }
    }

}




int main(){
   
    struct sockaddr_in server_addr;
    socklen_t addr_size;
    addr_size = sizeof(server_addr);
    int fd_server , c1 , c2  , flag;
    fd_server = socket(AF_INET, SOCK_STREAM, 0) ; // TCP
    if(fd_server < 0){
        perror("Socket error !!");
        exit(1);
    }
    printf("TCP server socket created.\n");
    memset(&server_addr, '\0', sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(Port);

    flag = bind(fd_server,(struct sockaddr*)&server_addr,addr_size);

    if(flag < 0){
        perror("Bind error !!");
        exit(1);
    }

    printf("Bind to the port number: %d\n", Port);

    if(listen(fd_server,2)<0){
        perror ("Listen Failed!!");
        exit(1);
    }

    printf("Waiting for players to join...\n");

   c1 = accept(fd_server,(struct sockaddr*)&server_addr,(socklen_t*)&addr_size);
   if(c1 < 0){
        perror("client1 accept failed!!");
        exit(1);
   }
   printf("Player 1 connected Successfully !!!\n");
   c2 = accept(fd_server,(struct sockaddr*)&server_addr,(socklen_t*)&addr_size);
    if(c2 < 0){
        perror("client1 accept failed!!");
        exit(1);
   }
   printf("Player 2 connected Successfully !!!\n");

//    ttt_board[3][3] = {{' ', ' '' '}, {' ',' ',' '}, {' ', ' ', ' '}};

   letsplaygame(c1,c2);

   close(c1);
   close(c2);
   close(fd_server);

   return 0;


 }


























