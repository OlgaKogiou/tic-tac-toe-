#include <unistd.h>
#include <fcntl.h> 
#include <sys/stat.h> 
#include<ctype.h> 
#include<sys/types.h> 
#include<sys/socket.h> 
#include<netinet/in.h> 
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#define SIZE sizeof(struct sockaddr_in) 

int play();
void check(char playBoard[3][3]);
static void handler(int signum);
void mapBoard(char playBoard[][3]);
void playerWinhandler(int signum);
int main(void)
{
    play();
}

int play(void)
{ 
    int fd[2], pd[2]; 
	int sockfd,player=0, num=0, n=0; 
	int row,column,choice;
	char clientWrite[1], c; 
	char a[2];
	char pid[4], str[2][4];
	char clientRead[3][3];
	char numberBoard [3][3] = {							// to display positions to choose from
	 		         		{'1','2','3'},
		        			{'4','5','6'},
		       				{'7','8','9'}
	       		          }; 
    char playBoard [3][3] =   {							// to display the actual game status
			         		{' ',' ',' '},
						{' ',' ',' '},
		       				{' ',' ',' '}
	                          };
	struct sockaddr_in server = {AF_INET, 8001};     //TCP/IP socket, network byte order
	server.sin_addr.s_addr = inet_addr("127.0.0.1"); //local host
    static struct sigaction act; 
	act.sa_handler = SIG_IGN; //ignore signal
	sigfillset(&(act.sa_mask)); //αρχικοποιηση γεματου συνολου
	sigaction(SIGTSTP, &act, 0); //χειριστης σηματος, ποια σηματα μπλοκαρονται όταν εκτελειται ο χειριστης σηματος
    signal(SIGINT,handler);
    
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Socket Call Failed");
		exit(1);
	}

    if ( connect (sockfd, (struct sockaddr *)&server, SIZE) == -1) 
	{ 
		perror("Connect Call Failed"); 
		exit(1); 
	}
	num= getpid();
    sprintf(pid,"%d",num);
    send(sockfd,pid,strlen(pid)+1,0);
    for(;;){
        recv(sockfd,a ,sizeof(a) ,0);
        if(strcmp(a,"1")==0){
            c='X';
            player=1;
            fd[0]=open("pipe1",O_RDONLY);
            read(fd[0], clientRead , sizeof(clientRead));
            memcpy(playBoard, clientRead, sizeof(playBoard));
            close(fd[0]);
        }
        else if(strcmp(a ,"2")==0){
            c='O';
            player=2;
            fd[1]=open("pipe2",O_RDONLY);
            read(fd[1], clientRead , sizeof(clientRead));
            memcpy(playBoard, clientRead, sizeof(playBoard));
            close(fd[1]);
        }
            mapBoard(playBoard);
            printf("\nCurrent Play Board\n\n");
            mapBoard(numberBoard);	
            printf("\n    Number Board\n\n");
            for(;;)
            {
                printf("\nPlayer %d,Please enter the number of the square where you want to place your '%c' \n", player, c);
                scanf("%s",clientWrite);

                choice = atoi(clientWrite);
                row = --choice/3;
                column = choice%3;
		
                if(choice<0 || choice>9 || playBoard [row][column]>'9'|| playBoard [row][column]=='X' || playBoard [row][column]=='O')
                    printf("Invalid Input. Please Enter again.\n\n");
		
                else
                {
                    playBoard[row][column] =c;
                    check(playBoard);
                    break;
                }			
                
            }
            if(c=='O'){
                fd[1]=open("pipe2",O_WRONLY);
                write(fd[1], clientWrite, sizeof(clientWrite));
                close(fd[1]);
            }
            else if(c=='X'){
                fd[0]=open("pipe1",O_WRONLY);
                write(fd[0], clientWrite, sizeof(clientWrite));
                close(fd[0]);
            }
            system("clear");
            //mapBoard(playBoard);
            //printf("\nCurrent Play Board,,,,,, \n\n");

    }
	
}

static void handler(int signum)
{   if(signum== SIGINT){
        printf("My server has Killed me!!!\n"); 
        exit(0);
    }
}

void check(char playBoard[3][3])
{
	  int i;
	  char key = ' ';

	  // Check Rows
	  for (i=0; i<3;i++)
	  if (playBoard [i][0] == playBoard [i][1] && playBoard [i][0] == playBoard [i][2] && playBoard [i][0] != ' ') key = playBoard [i][0];	
	  // check Columns
	  for (i=0; i<3;i++)
	  if (playBoard [0][i] == playBoard [1][i] && playBoard [0][i] == playBoard [2][i] && playBoard [0][i] != ' ') key = playBoard [0][i];
	  // Check Diagonals
	  if (playBoard [0][0] == playBoard [1][1] && playBoard [1][1] == playBoard [2][2] && playBoard [1][1] != ' ') key = playBoard [1][1];
	  if (playBoard [0][2] == playBoard [1][1] && playBoard [1][1] == playBoard [2][0] && playBoard [1][1] != ' ') key = playBoard [1][1];
      if (key == 'X')
	  {

		   printf("Player 1 Wins\n\n");
           
	  }    

	  if (key == 'O')
	  {

		   printf("Player 2 Wins\n\n");
           
	  }

}	
void mapBoard(char playBoard[3][3])
{
        printf(" _________________\n");
        printf("|     |     |     | \n");
        printf("|  %c  |  %c  |  %c  |\n",playBoard[0][0],playBoard[0][1],playBoard[0][2]);
        printf("|_____|_____|_____|\n");
        printf("|     |     |     |\n");
        printf("|  %c  |  %c  |  %c  |\n",playBoard[1][0],playBoard[1][1],playBoard[1][2]);
        printf("|_____|_____|_____|\n");
        printf("|     |     |     |\n");
        printf("|  %c  |  %c  |  %c  |\n",playBoard[2][0],playBoard[2][1],playBoard[2][2]);
        printf("|_____|_____|_____|\n");
		
}
