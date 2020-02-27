#include<ctype.h> 
#include<sys/types.h> 
#include<sys/socket.h>
#include<sys/wait.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include<netinet/in.h> 
#include<signal.h> 
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include <unistd.h> 
#define SIZE sizeof(struct sockaddr_in) 


void check (char playBoard[3][3]); 
void mapBoard(char clientRead[3][3]);
void catcher(int sig);
int pid[2];
int id[2];
int main(void) 
{   pid_t pd1, pd2;
    sigset_t s1,s2;
    int newsockfd[2]; //the sockets I use to accept
	int sockfd, usr=0,row = 0,column = 0, choice=0, signum, num=0;
    int fp[2]; //the pipe between the child processes
	int fd[2]; //the two pipes
	char x[2][4], serverRead[5], a[2], c, n[5];
    char pd[2][4];
	char clientRead[3][3];
    char playBoard [3][3] =   {
			         		{' ',' ',' '},
						{' ',' ',' '},
		       				{' ',' ',' '}
	                          };
    
	struct sockaddr_in server = {AF_INET, 8001, INADDR_ANY}; //internet socket, port, all interfaces
    mkfifo("pipe1" ,S_IRWXU);
    mkfifo("pipe2",S_IRWXU);
    static struct sigaction act,act2; 
	act.sa_handler = catcher; //χειριστης
    signal(SIGINT,catcher);
    sigemptyset(&s1); //αρχικοποιηση κενου συνολου
    sigemptyset(&s2);
    sigaddset(&s1, SIGUSR1); //προσθηκη σηματος
    sigaddset(&s2, SIGUSR2);
    sigprocmask(SIG_BLOCK,&s1,NULL); // μπλοκάρισμα των σημάτων του set
    sigprocmask(SIG_BLOCK,&s2,NULL);
    
	sigfillset(&(act.sa_mask)); //αρχικοποιηση γεματου συνολου
	sigaction(SIGPIPE, &act, 0); 
	sigaction(SIGTSTP, &act, 0); //αναστολή εκτέλεσης από τερματικό, τα σηματα που μπλοκαρονται οταν εκτελειται ο χειριστης σηματος
	sigaction(SIGINT, &act, 0); //διακοπή από το πληκτρολόγιο
    

    if((sockfd= socket(AF_INET, SOCK_STREAM, 0)) == -1) 
		{ 
            perror("Socket Call Failed"); 
            exit(1); 
		} 

	if(bind(sockfd , (struct sockaddr *)&server, SIZE) == -1) 
		{ 
		perror("Bind Call Failed"); 
		exit(1); 
		}
	
	printf("Waiting for Players to join in TicTacToe Club.. \n");
    
    while(usr<2)
	{
		if ( listen(sockfd, 1) == -1 ) 
		{ 
			perror("Listen Call Failed\n"); 
			exit(1) ; 
		}
		newsockfd[usr] = accept(sockfd, NULL, NULL);
		recv(newsockfd[usr],x[usr],4,0);
        pid[usr]=atoi(x[usr]); //the first ID of the process
        printf("The pid is %d\n", pid[usr]);
        printf("No. of Players who joined the Club: %d\n",(usr+1));
        usr++;
    }
    usr=0;
    pipe(fp);
    pd1=fork();
    if(pd1==0){
            for(;;){
            sigwait(&s2, &signum);
            
            read(fp[0],playBoard, sizeof(playBoard));
            id[0]=getpid();
            printf("Player 1\n");
            strcpy(a , "1");
            send(newsockfd[0],a ,strlen(a)+1,0);
            fd[0]=open("pipe1",O_WRONLY);
            write(fd[0], playBoard, sizeof(playBoard));
            close(fd[0]);
            fd[0]=open("pipe1",O_RDONLY);
            read(fd[0], serverRead, sizeof(serverRead));
            choice = atoi(serverRead);
            printf("Server side the Integer received is: %d\n", choice);
            close(fd[0]);
            row = --choice/3;
            column = choice%3;
            playBoard[row][column] ='X';
            
            write(fp[1], playBoard, sizeof(playBoard));
            
            check(playBoard);
            
            kill(pd1, SIGUSR1);
            
        }
    }
       pd2=fork();
        if(pd2==0){
            for(;;){
                if(num>0){
                    read(fp[0], playBoard, sizeof(playBoard));
                }
                printf("Player 2\n");
                strcpy(a , "2");
                send(newsockfd[1],a ,strlen(a)+1,0);
                fd[1]=open("pipe2",O_WRONLY);
                write(fd[1], playBoard, sizeof(playBoard));
                close(fd[1]);
                fd[1]=open("pipe2",O_RDONLY);
                read(fd[1], serverRead, sizeof(serverRead));
                choice = atoi(serverRead);
                printf("Server side the Integer received is: %d\n", choice);
                close(fd[1]);
                row = --choice/3;
                column = choice%3;
                playBoard[row][column] ='O';
                id[1]=getpid();
                write(fp[1], playBoard, sizeof(playBoard));
                
                check(playBoard);
                num++;
                kill(pd2, SIGUSR2);
                sigwait(&s1, &signum);
            }
            
        }
        else{
            pd2=wait(NULL);
            close(newsockfd[0]);
            close(newsockfd[1]);
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
           unlink("pipe1");
            unlink("pipe2");
            kill(pid[0], SIGQUIT);
            kill(pid[1], SIGQUIT);
		   exit (1); 
	  }    

	  if (key == 'O')
	  {

		   printf("Player 2 Wins\n\n");
           unlink("pipe1");
            unlink("pipe2");
            kill(pid[0], SIGQUIT);
            kill(pid[1], SIGQUIT);
		   exit (1);
	  }

}
void catcher(int sig)
{   unlink("pipe1");
    unlink("pipe2");
    kill(pid[0], SIGINT);
    kill(pid[1], SIGINT);
    exit(1);
}



