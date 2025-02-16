
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define Port 8086
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

int startgame(struct sockaddr_in *c1, struct sockaddr_in *c2, int fd_server) {
    int present_player = 1;
    char input[isize];
    int winner = 0, matchdraw = 0;

    clear_board();
    printf("Game Begins !!!\n");

    PrintBoard(ttt_board);

    struct sockaddr_in curr_socket, next_socket;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    // c1 = X and c2 = O
    while (!winner && !matchdraw) {
        // Determine the current and next player's socket
        if (present_player == 1) {
            curr_socket = *c1; // Current player
            next_socket = *c2; // Opponent
        } else {
            curr_socket = *c2; // Current player
            next_socket = *c1; // Opponent
        }

        // Send the current state of the board to both players
        sendto(fd_server, "BOARD", 6, 0, (struct sockaddr*)&curr_socket, addr_len);
        sendto(fd_server, ttt_board, sizeof(ttt_board), 0, (struct sockaddr*)&curr_socket, addr_len);

        // Prompt the current player to make a move
        char prompt[] = "It's your turn. Enter your move (row col): ";
        sendto(fd_server, prompt, sizeof(prompt), 0, (struct sockaddr*)&curr_socket, addr_len);

        // Receive input from the current player
        recvfrom(fd_server, input, sizeof(input), 0, (struct sockaddr*)&curr_socket, &addr_len);
        int rw = (input[0] - '0') - 1;
        int cl = (input[2] - '0') - 1;

        printf("Player %d selected position: (%d, %d)\n", present_player, rw + 1, cl + 1);

        if (rw >= 0 && rw <= 2 && cl >= 0 && cl <= 2 && ttt_board[rw][cl] == '-') {
            if (present_player == 1) {
                ttt_board[rw][cl] = 'X';
            } else {
                ttt_board[rw][cl] = 'O';
            }

            PrintBoard(ttt_board);
            sendto(fd_server, "BOARD", 6, 0, (struct sockaddr*)&curr_socket, addr_len);
            sendto(fd_server, ttt_board, sizeof(ttt_board), 0, (struct sockaddr*)&curr_socket, addr_len);

            // Check for winner or draw after each move
            winner = check_winner();
            matchdraw = check_draw();

            if (winner == 1) {
                char win_mssg[csize];
                snprintf(win_mssg, sizeof(win_mssg), "Player %d won the game!", present_player);
                printf("Player %d won the Game !!\n", present_player);
                sendto(fd_server, "MSSGG", 6, 0, (struct sockaddr*)&curr_socket, addr_len);
                sendto(fd_server, win_mssg, sizeof(win_mssg), 0, (struct sockaddr*)&curr_socket, addr_len);
                sendto(fd_server, "MSSGG", 6, 0, (struct sockaddr*)&next_socket, addr_len);
                sendto(fd_server, win_mssg, sizeof(win_mssg), 0, (struct sockaddr*)&next_socket, addr_len);
                return present_player;
            } else if (matchdraw == 1) {
                char draw_mssg[] = "It's a draw!";
                printf("It's A Draw !!\n");
                sendto(fd_server, "MSSGG", 6, 0, (struct sockaddr*)&curr_socket, addr_len);
                sendto(fd_server, draw_mssg, sizeof(draw_mssg), 0, (struct sockaddr*)&curr_socket, addr_len);
                sendto(fd_server, "MSSGG", 6, 0, (struct sockaddr*)&next_socket, addr_len);
                sendto(fd_server, draw_mssg, sizeof(draw_mssg), 0, (struct sockaddr*)&next_socket, addr_len);
                return present_player;
            } else {
                char simple_msg[] = "Your move updated successfully";
                sendto(fd_server, "MSSGG", 6, 0, (struct sockaddr*)&curr_socket, addr_len);
                sendto(fd_server, simple_msg, sizeof(simple_msg), 0, (struct sockaddr*)&curr_socket, addr_len);
                present_player = (present_player == 1) ? 2 : 1;
            }
        } else {
            sendto(fd_server, "BOARD", 6, 0, (struct sockaddr*)&curr_socket, addr_len);
            sendto(fd_server, ttt_board, sizeof(ttt_board), 0, (struct sockaddr*)&curr_socket, addr_len);

            // Handle invalid move
            sendto(fd_server, "MSSGG", 6, 0, (struct sockaddr*)&curr_socket, addr_len);
            char message[csize] = "Invalid move, try again!";
            sendto(fd_server, message, sizeof(message), 0, (struct sockaddr*)&curr_socket, addr_len);
        }
    }
}

int askForReplay(struct sockaddr_in *c, int fd_server) {
    char ques[] = "Do you want to play again ?(yes/no) : ";
    char ans[csize];
    socklen_t addr_len = sizeof(struct sockaddr_in);

    // printf("sending!\n");
     sleep(1);
    sendto(fd_server, ques, sizeof(ques), 0, (struct sockaddr *)c, addr_len);
    // printf("sent\n");
      sleep(1);
    recvfrom(fd_server, ans, sizeof(ans), 0, (struct sockaddr *)c, &addr_len);
    if (strncmp(ans, "yes", 3) == 0) {
        return 1;
    } else {
        return 0;
    }

    return 0;
}
void letsplaygame(struct sockaddr_in *c1_addr, struct sockaddr_in *c2_addr, int fd_server) {
    int Rc1 = 1, Rc2 = 1;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    
    while (Rc1 == 1 && Rc2 == 1) {
        // Start the game and store the result
        int x = startgame(c1_addr, c2_addr, fd_server);

        if (x == 0 || x == 1) {
            // Ask for replay from both players
            Rc1 = askForReplay(c1_addr, fd_server);
            Rc2 = askForReplay(c2_addr, fd_server);
        } else if (x == 2) {
            // Reverse asking order for replay if needed
            Rc2 = askForReplay(c2_addr, fd_server);
            Rc1 = askForReplay(c1_addr, fd_server);
        }

        // If both players agree to continue
        if (Rc1 == 1 && Rc2 == 1) {
            char both_continue_msg[] = "Both players agreed to play the game again.";
            sendto(fd_server, both_continue_msg, strlen(both_continue_msg), 0, (struct sockaddr *)c1_addr, addr_len);
            sendto(fd_server, both_continue_msg, strlen(both_continue_msg), 0, (struct sockaddr *)c2_addr, addr_len);
        }
        // If both players do not want to continue
        else if (Rc1 != 1 && Rc2 != 1) {
            char both_stop_msg[] = "Both players do not want to play the game again.";
            sendto(fd_server, both_stop_msg, strlen(both_stop_msg), 0, (struct sockaddr *)c1_addr, addr_len);
            sendto(fd_server, both_stop_msg, strlen(both_stop_msg), 0, (struct sockaddr *)c2_addr, addr_len);
        }
        // If player 1 chooses not to continue but player 2 wants to
        else if (Rc1 != 1 && Rc2 == 1) {
            char p1_stop_msg[] = "You choose not to continue the Game.";
            char p1_stop_notify_msg[] = "Player 1 does not want to continue the Game.";
            sendto(fd_server, p1_stop_msg, strlen(p1_stop_msg), 0, (struct sockaddr *)c1_addr, addr_len);
            sendto(fd_server, p1_stop_notify_msg, strlen(p1_stop_notify_msg), 0, (struct sockaddr *)c2_addr, addr_len);
        }
        // If player 2 chooses not to continue but player 1 wants to
        else if (Rc1 == 1 && Rc2 != 1) {
            char p2_stop_msg[] = "You choose not to continue the Game.";
            char p2_stop_notify_msg[] = "Player 2 does not want to continue the Game.";
            sendto(fd_server, p2_stop_msg, strlen(p2_stop_msg), 0, (struct sockaddr *)c2_addr, addr_len);
            sendto(fd_server, p2_stop_notify_msg, strlen(p2_stop_notify_msg), 0, (struct sockaddr *)c1_addr, addr_len);
        }
    }
}

int main() {
    int fd_server;
    struct sockaddr_in server_addr, client1_addr, client2_addr;
    socklen_t addr_len = sizeof(server_addr);

    // Creating socket
    fd_server = socket(AF_INET, SOCK_DGRAM, 0); // Use UDP (SOCK_DGRAM)
    if (fd_server < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(Port);

    // Binding server socket
    if (bind(fd_server, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %d\n", Port);

    // Wait for client 1 to connect
    char buffer[csize];
    recvfrom(fd_server, buffer, sizeof(buffer), 0, (struct sockaddr *)&client1_addr, &addr_len);
    printf("Client 1 Connected!\n");

    // Wait for client 2 to connect
    recvfrom(fd_server, buffer, sizeof(buffer), 0, (struct sockaddr *)&client2_addr, &addr_len);
    printf("Client 2 Connected!\n");

    // Start the game
    letsplaygame(&client1_addr, &client2_addr, fd_server);
    printf("Game Stopped !!\n ");

    close(fd_server);

    return 0;
}
