#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>


#define MAX_SIZE 4096
#define STDIN 0
#define TRUE 1999

void setupSocket(char *adrr, char *port);
void keepChatting();

int socket_fd;
struct sockaddr_in server_addr;
struct hostent *host;

int main(int argc, char **argv, char **envp){

  /* Validate args */
  if (argc < 3){
    fprintf(stderr, "[!] Wrong or missing arguments\n");
    fprintf(stderr, "[!] Try : ./chat-client <addr> <port>\n");
    exit(EXIT_FAILURE);
  }
  host = gethostbyname(argv[1]);
	if(host == NULL) {
    fprintf(stderr, "[!] getHost(): getHost() failed\n");
	}
  /* Setup the socket */
  setupSocket(argv[1], argv[2]);

  /* Just... Keep chatting :D */
  keepChatting();

  close(socket_fd);
  return 0;
}

void setupSocket(char *addr, char *port){
  if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    fprintf(stderr, "[!] setupSocket(): socket() failed\n");
    exit(EXIT_FAILURE);
  }

  server_addr.sin_family = AF_INET;
  bcopy((char *)host->h_addr, (char *)&server_addr.sin_addr.s_addr, host->h_length);
  server_addr.sin_port = htons(atoi(port));

  if (connect(socket_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) != 0){
    fprintf(stderr, "[!] setupSocket(): connect() failed\n");
    exit(EXIT_FAILURE);
  }
}

void keepChatting(){
  fd_set read_fds;
  char messageBuffer[MAX_SIZE];
  int messageSize;
  int verify;
  int verifywrite;
  while (TRUE){
    FD_CLR(socket_fd, &read_fds);
    FD_SET(socket_fd, &read_fds);
    FD_SET(STDIN, &read_fds);

    verify=select(socket_fd + 1, &read_fds, NULL, NULL, NULL);
    if(verify<0){
      exit(EXIT_FAILURE);
    }

    if (FD_ISSET(STDIN, &read_fds)){
      bzero(messageBuffer, sizeof(messageBuffer));
      messageSize = read(STDIN, messageBuffer, MAX_SIZE);
      if(messageSize==0){
        verifywrite = write(socket_fd, messageBuffer, messageSize);
        if(verifywrite<0){
          fprintf(stderr, "[!] keepChatting(): error write\n");
          exit(EXIT_FAILURE);
        }
        close(socket_fd);
        exit(0);
      }
      else{
        verifywrite = write(socket_fd, messageBuffer, messageSize);
        if(verifywrite<0){
          fprintf(stderr, "[!] keepChatting(): error write\n");
          exit(EXIT_FAILURE);
        }
      }
      
      bzero(messageBuffer, sizeof(messageBuffer));
      
      
      

    }
    else if (FD_ISSET(socket_fd, &read_fds)){
      bzero(messageBuffer, sizeof(messageBuffer));  
      messageSize = read(socket_fd, messageBuffer, sizeof(messageBuffer));
      if (messageSize <= 0){
        fprintf(stderr, "[!] keepChatting(): server orderly disconnected\n");
        close(socket_fd);
        exit(EXIT_SUCCESS);
      }
      printf("%s", messageBuffer);
      bzero(messageBuffer, MAX_SIZE);
    }
  }
}
