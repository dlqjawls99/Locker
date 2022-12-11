#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "locker.h"
#define DEFAULT_PROTOCOL 0

 int main (int argc, char* argv[])
 {
   int sfd, port, result;

   char *host;
   struct sockaddr_in serverAddr;
   struct hostent *hp;

   char Outmsg1[100];
   char Inmsg2[1000];
   char Outmsg3[100];
   char Inmsg4[100];
   
   char Checkmsg1[100];
   char Checkmsg2[100];
   char Checkmsg3[100] = "This Locker is Used\n";
   char Checkmsg4[100] = "Password Incorrect-30 Second Lock and not Use\n";
   char Checkmsg5[100] = "This Locker is used by someone OR TimeLimit exist\n";
   int Select;
   int Choice;
   char Password1[100];
   char Password2[100];
   char Password3[100];
   char LockerText[100];
   char Textout[100];
   
   char TimeLimit[1000];
   
   host = argv[1];
   port = atoi(argv[2]);
   sfd = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);

   if((hp = gethostbyname(host)) == NULL)
   	perror("gethostbyname error");
   bzero((char *) &serverAddr, sizeof(serverAddr));
   serverAddr.sin_family = AF_INET;
   bcopy((char *)hp->h_addr_list[0], (char *)&serverAddr.sin_addr.s_addr, hp->h_length);
   serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
   serverAddr.sin_port = htons(port);
  
   do { //connect
      result = connect(sfd, (struct sockaddr*)&serverAddr,  sizeof(serverAddr));
      if (result == -1) sleep(1);
   } while (result == -1);
  
  while(1){
  printf("1. Cherk the Locker\n");
  printf("2. Use the Locker \n");
  printf("3. EXIT\n");
  printf("Select Command: ");
  scanf("%s", Outmsg1);
  getchar();
  write(sfd, Outmsg1, 101);
  Select = atoi(Outmsg1);
  printf("%d execute\n", Select);
  
   if(Select == 1){
   	read(sfd, Inmsg2, 1001);
   	printf("%s\n", Inmsg2);
   }
   
   else if(Select == 2){
   	printf("Select use Locker: ");
   	scanf("%d", &Choice);
   	getchar();
   	sprintf(Checkmsg1, "%d.Locker can use\n", Choice);
   	
   	sprintf(Checkmsg2, "Locker not exist\n");
   	
   	sprintf(Outmsg3, "%d" , Choice);
   	
   	write(sfd, Outmsg3, 101);
   	read(sfd, Inmsg4, 101); //"Locker not exist\n" , "%d.Locker can use\n" , "Locker Used" , "This Locker is used by someone\n"
   	printf("%s\n", Inmsg4);
   		
   		if(strcmp(Inmsg4, Checkmsg5) == 0){
   			read(sfd, TimeLimit, 1001);
   			printf("%s", TimeLimit);
 
   		}
   		
   		if(strcmp(Inmsg4, Checkmsg1) == 0){
   			printf("First password: ");
   			scanf("%s", Password1);
   			getchar();
   			write(sfd, Password1, 101);
   			printf("Second password: ");
   			scanf("%s", Password2);
   			getchar();
   			write(sfd, Password2, 101);
   			
   			if(strcmp(Password1, Password2) == 0){
   				printf("Write Text: ");
   				scanf("%s", LockerText);
   				getchar();
   				write(sfd, LockerText, 101);
   			}
   			
   			else{
   			 	printf("Two Password Different\n");

   			}
   		}
   		
   		if(strcmp(Inmsg4, Checkmsg2) == 0){//Locker not exist
   		}
   		
   		
   		if(strcmp(Inmsg4, Checkmsg3) == 0){//Locker Used
   		printf("Locker Password: ");
   		scanf("%s", Password3);
   		getchar();
   		write(sfd, Password3, 101);
   		read(sfd, Textout, 101); // "Password incollect" or "Locker TEXT"
   			
   			if(strcmp(Textout, Checkmsg4) == 0){ //"Password incollect"
   				printf("%s", Textout);

   			}
   			else{
   				printf("Text: %s\n", Textout); //"Locker TEXT"

   			}
   		

   		} 
   }
   
   else if(Select == 3){ //Client Exit
   	close(sfd);
   	exit(0);
   }
  
   else{ //IF Wrong COmmand Num
   	printf("Wrong Command Number\n");

	}
}
}//main
