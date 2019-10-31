#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/types.h>

#define MAX_SIZE 4096
#define STDIN 0
#define TRUE 1999
#define MAXCLIENTS 1000

int socket_fd,tamanho, connection_fd, client_socket[MAXCLIENTS];
int max_fd, fd, fd_changed;
struct sockaddr_in server_addr, client_addr;
int opt=1;
int iter;

void setupSocket(char *port);

int main(int argc, char **argv, char **envp){
  unsigned int len;
  fd_set readfds;
  int i;
  char messageBuffer[4096]; 
  char aux[4096];
  int messageSize = 0;
  
  /* Validate args */
  if (argc < 2){
    fprintf(stderr, "[!] Wrong or missing arguments\n");
    fprintf(stderr, "[!] Try : ./chat-server <port>\n");
    exit(EXIT_FAILURE);
  }
  
  for (i = 0; i < MAXCLIENTS - 1; i++) {   
      client_socket[i] = 0;   
  }   

  /* Setup the socket */
  
  setupSocket(argv[1]);
  tamanho=sizeof(client_addr);
  while (TRUE){
    FD_ZERO(&readfds);
    FD_SET(socket_fd, &readfds);
    int i;
    for (i = 0; i<MAXCLIENTS-1;i++) {   
      //socket descriptor  
      fd=client_socket[i];     
      //Add the file descriptor to the set, to be used in the select function  
      if(fd>0)   
          FD_SET(fd,&readfds);    
      //Highest file decriptor from the sockets  
      if(fd>max_fd)   
          max_fd=fd;   
    }
    fd_changed = select( max_fd + 1 , &readfds , NULL , NULL , NULL);
    if ((fd_changed < 0)) { 
      fprintf(stderr, "[!] select(): failed\n");  
      exit(EXIT_FAILURE);
    }
    if (FD_ISSET(socket_fd, &readfds)) {
      len = sizeof(client_addr);
      connection_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &len);
      if (connection_fd < 0) {
        fprintf(stderr, "[!] main(): accept() failed\n");
        exit(EXIT_FAILURE);
      }
      else{
        /*Adding the socket file descriptor to the list of clients*/
        for (i = 0 ; i < MAXCLIENTS; i++){
          if((client_socket[i] == 0)){
            client_socket[i] = connection_fd; 
            break;
          }
        }
      
        for (i = 0; i < MAXCLIENTS; i++) {
          if ((client_socket[i]) != 0) {
            dprintf(client_socket[i], "%s:%d joined.\n", inet_ntoa(client_addr.sin_addr), (int)ntohs(client_addr.sin_port));
            /* dprintf(0, "%s:%d joined!\n", inet_ntoa(client_addr.sin_addr), (int)ntohs(client_addr.sin_port)); */
          }
        }
      }
    }
    for(i = 0 ; i < MAXCLIENTS; i++) {
      if (FD_ISSET(client_socket[i] , &readfds)){
        bzero(messageBuffer, sizeof(messageBuffer));
        messageSize = read(client_socket[i], messageBuffer, sizeof(messageBuffer));
        getpeername(client_socket[i] , (struct sockaddr*)&client_addr , (socklen_t*)&tamanho);
        if(messageSize > 0) {
          //send the msg to everyone but who sent it

          for(int k=0;k<messageSize;k++){
            strncat(aux,&messageBuffer[k],1);
            if(messageBuffer[k]=='\n'){
              for (int j = 0; j < MAXCLIENTS; j++) {
                if (client_socket[j] != 0 && client_socket[j]!=client_socket[i]) {
                  dprintf(client_socket[j] , "%s:%d %s", inet_ntoa(client_addr.sin_addr), (int)ntohs(client_addr.sin_port), aux);
              /* dprintf(0, " %s:%d: %s", inet_ntoa(client_addr.sin_addr), (int)ntohs(client_addr.sin_port), messageBuffer); */
                }
              }
              bzero(aux,MAX_SIZE);

            }
          }
          
        }
        if(messageSize == 0 ){
          for (int j = 0; j < MAXCLIENTS; j++){
              if (client_socket[j] != 0 && client_socket[j]!=client_socket[i]){
                dprintf(client_socket[j], "%s:%d left.\n", inet_ntoa(client_addr.sin_addr), (int)ntohs(client_addr.sin_port));
              }
              
          }
          close(client_socket[i]);
          client_socket[i] = 0;
        }
      }
    }
    bzero(messageBuffer, sizeof(messageBuffer));
  
  }
  return 0;
}

void setupSocket(char *port){
  if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    fprintf(stderr, "[!] setupSocket(): socket() failed\n");
    exit(EXIT_FAILURE);
  }
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof(opt)) < 0) {
        fprintf(stderr, "[!] setupSocket(): setsockopt() failed\n");
        exit(EXIT_FAILURE);
    }
  max_fd=socket_fd;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(atoi(port));

  if ((bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))) != 0) {
      fprintf(stderr, "[!] setupSocket(): bind() failed\n");
      exit(EXIT_FAILURE);
  }

  if ((listen(socket_fd, 1000)) != 0) {
    fprintf(stderr, "[!] setupSocket(): listen() failed\n");
    exit(EXIT_FAILURE);
  }
}
