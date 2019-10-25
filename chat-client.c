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

void setupSocket(char *adrr, char *port);
void keepChatting();

int socket_fd, connection_fd;
struct sockaddr_in server_addr, client_addr;

int main(int argc, char **argv, char **envp){

  /* Validate args */
  if (argc < 3){
    fprintf(stderr, "[!] Wrong or missing arguments\n");
    fprintf(stderr, "[!] Try : ./chat-client <addr> <port>\n");
    exit(EXIT_FAILURE);
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
  server_addr.sin_addr.s_addr = inet_addr(addr);
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

  while (1999){
    FD_CLR(socket_fd, &read_fds);
    FD_SET(socket_fd, &read_fds);
    FD_SET(STDIN, &read_fds);

    select(socket_fd + 1, &read_fds, NULL, NULL, NULL);

    if (FD_ISSET(STDIN, &read_fds)){
      bzero(messageBuffer, sizeof(messageBuffer));
      messageSize = read(0, messageBuffer, sizeof(messageBuffer));
      write(socket_fd, messageBuffer, sizeof(messageBuffer));
    }
    else if (FD_ISSET(socket_fd, &read_fds)){
      bzero(messageBuffer, sizeof(messageBuffer));
      messageSize = read(socket_fd, messageBuffer, sizeof(messageBuffer));
      if (messageSize <= 0){
        fprintf(stderr, "[!] keepChatting(): server orderly disconnected\n");
        exit(EXIT_SUCCESS);
      }
      printf("From server: %s\n", messageBuffer);
    }
  }

  exit(EXIT_SUCCESS);
}
