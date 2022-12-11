#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "locker.h"
#define DEFAULT_PROTOCOL 0

int t = 30;

 int main (int argc, char* argv[])
 {
   int sfd, cfd, port, clientlen;
   
   int fd;
   int i, n;
   
   
   char Time[100];
   
   char Nullmsg[1000] = "\0";
   char Empty[100] = "Empty";
   char Use[100] = "Use";
   
   char Outmsg1[100];
   char Inmsg1[100];
   
   char Lockermsg[100] = "";
   char Outmsg2[1000] = "\0";
   
   char Inmsg3[100];
   char Outmsg4[100];
   
   char Password1[100];
   char Password2[100];
   char Password3[100];
   
   char LockerText[100];
   char Textout[100];
   char TimeLimit[1000];
   
   int Select;
   int Choice;
   struct sockaddr_in serveraddr, clientaddr;
   struct hostent *hp;
   char *haddrp;	
 
   //Set the Locker Number
   struct locker rec;
  
   
   if((fd = open("Locker", O_RDWR | O_CREAT |O_TRUNC, 0660)) == -1){
		  printf("File open Error");
		  exit(0);   
   }
   else{
   printf("File Locker Open\n");
   }
   
   printf("Set the number of Locker: ");
   scanf("%d", &n);
   getchar(); 
   printf("All Locker set Empty\n");

   for(i = 1; i <= n; i++){
   	rec.id = i;
   	rec.time = 0;
   	strcpy(rec.passwd, Nullmsg);
   	strcpy(rec.text, Nullmsg);
   	strcpy(rec.lock, Empty);
   	lseek(fd, (rec.id - START_ID) * sizeof(rec), SEEK_SET);
   	write(fd, &rec, sizeof(rec));
   }
  
 
   signal(SIGCHLD, SIG_IGN);
   port = atoi(argv[1]);
   sfd = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);
   
   bzero((char *)&serveraddr, sizeof(serveraddr));
   serveraddr.sin_family = AF_INET;
   serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
   serveraddr.sin_port = htons((unsigned short)port);
   bind(sfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
   listen(sfd, 10);
   printf("Locker Server Online\n");
   printf("Locker Number 1 ~ %d\n", n);
   
   close(fd);
   
   while (1) {
    hp = gethostbyaddr((char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr),AF_INET);
    clientlen = sizeof(clientaddr);
    cfd = accept(sfd, (struct sockaddr *)&clientaddr, &clientlen); 
    haddrp = inet_ntoa(clientaddr.sin_addr);
    printf("Server:%s (%d)\n", haddrp, clientaddr.sin_port);
      
      //Chile Process
      if (fork() == 0) {
      fd = open("Locker", O_RDWR);
      while(1){

      printf("Child Process Execute\n");
      read(cfd, Inmsg1, 101);
      Select = atoi(Inmsg1);
      printf("Select : %d\n", Select);
      
      
      if(Select == 1){ //Show Locker
      	printf("Select 1 execute\n");
      		for(i = 1; i <= n; i++){
  			lseek(fd, (i - START_ID) * sizeof(rec), SEEK_SET);
  				if((read(fd, &rec, sizeof(rec)) > 0) && (rec.id != 0)){
  					sprintf(Lockermsg, "%d.Locker : %s  ", rec.id, rec.lock); 
					strcat(Outmsg2, Lockermsg);
					
					 				
  				      }
      		}
      		write(cfd, Outmsg2, 1001);
      		printf("%s\n", Outmsg2);
      		strcpy(Outmsg2, Nullmsg);
      }
      
      if(Select == 3){
      	printf("Select 3 execute\n");
      	
      	close(fd);
        close(cfd);
        exit(0); 
      }
      
      
      if(Select == 2){ //Use Locker
      	printf("Select 2 execute\n");
      	read(cfd, Inmsg3, 101);
      	printf("Choice Change Locker : %s\n", Inmsg3);
      	Choice =atoi(Inmsg3);
      	lseek(fd, (Choice - START_ID) * sizeof(rec), SEEK_SET);	
      
      		if(Choice > n || Choice < 1){
	   		sprintf(Outmsg4, "Locker not exist\n");
	   		write(cfd , Outmsg4, 101);
	   		continue;
      		}
      
      	
      	if(lockf(fd, F_TLOCK, sizeof(rec)) == -1){	 //Record Lock, If record is lock -> fail(-1);
      		read(fd, &rec, sizeof(rec));
    		sprintf(Outmsg4, "This Locker is used by someone OR TimeLimit exist\n");
    		write(cfd, Outmsg4, 101);
   		sprintf(TimeLimit, "%d.Locker-TimeLimit: %d\n", Choice, rec.time);
   		write(cfd, TimeLimit, 1001);
   		continue;
   		//lseek(fd, (Choice - START_ID) * sizeof(rec), SEEK_SET);
    		//lockf(fd,F_ULOCK, sizeof(rec)); //Record Unlock
      	}
      
      read(fd, &rec, sizeof(rec));	
      		
      		if(strcmp(rec.lock, "Empty") == 0){//If Locker is Empty 
      			sprintf(Outmsg4, "%d.Locker can use\n", Choice);
      			write(cfd, Outmsg4, 101);
      			
      			read(cfd, Password1, 101);
      			printf("Password1: %s\n", Password1);
      			
      			read(cfd, Password2, 101);
      			printf("Password2: %s\n", Password2);
      			
      			if(strcmp(Password1, Password2) == 0){ 
      				lseek(fd, -sizeof(rec), SEEK_CUR);
      				strcpy(rec.lock, "Use");
      				read(cfd, LockerText, 101);
      				strcpy(rec.text, LockerText);
      				strcpy(rec.passwd, Password2);
      				write(fd, &rec, sizeof(rec));
      				
      				lseek(fd, (Choice - START_ID) * sizeof(rec), SEEK_SET);
      				lockf(fd,F_ULOCK, sizeof(rec)); //Record Unlock	
      			}
      			
      			
      			else{
      				lseek(fd, (Choice - START_ID) * sizeof(rec), SEEK_SET);
      				lockf(fd,F_ULOCK, sizeof(rec)); //Record Unlock	
      			}
	
      		}
      		else{
      			sprintf(Outmsg4, "This Locker is Used\n");
      			write(cfd, Outmsg4, 101);
      			read(cfd, Password3, 101);
      			
      			if(strcmp(rec.passwd, Password3) == 0){
				lseek(fd, -sizeof(rec), SEEK_CUR);
				strcpy(Textout, rec.text);
				write(cfd, Textout, 101);
				strcpy(rec.lock, "Empty");
				write(fd, &rec, sizeof(rec));
				
				lseek(fd, (Choice - START_ID) * sizeof(rec), SEEK_SET);
      				lockf(fd,F_ULOCK, sizeof(rec)); 
		
				 
      		       }
  		       
  		       else{
  		        sprintf(Textout, "Password Incorrect-30 Second Lock and not Use\n");
  		        write(cfd, Textout, 101);
  		        
  		        while(t != 0){
  		        lseek(fd, (Choice - START_ID) * sizeof(rec), SEEK_SET);
  		        t = t - 1;
  		        rec.time = t;
  		        write(fd, &rec, sizeof(rec));
  		        sleep(1);
  		        }
  		    	
  		    	t = 30;
  		    	lseek(fd, (Choice - START_ID) * sizeof(rec), SEEK_SET);
      			lockf(fd,F_ULOCK, sizeof(rec)); 
  		       }
		}
      		
      } 
     
        }
	 } //fork
 } // while
}// main
