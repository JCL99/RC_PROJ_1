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

int socket_fd, connection_fd;
struct sockaddr_in server_addr;

typedef struct client{
  struct sockaddr_in client_addr;
  int ip_addr;
}

void setupSocket(char *port);

int main(int argc, char **argv, char **envp){

  /* Validate args */
  if (argc < 2){
    fprintf(stderr, "[!] Wrong or missing arguments\n");
    fprintf(stderr, "[!] Try : ./chat-server <port>\n");
    exit(EXIT_FAILURE);
  }

  /* Setup the socket */
  setupSocket(argv[1]);

  while (TRUE){
    connection_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &len);
    if (connfd < 0) {
        printf("server acccept failed...\n");
        exit(0);
    }
  }

  return 0;
}

void setupSocket(char *port){
  if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    fprintf(stderr, "[!] setupSocket(): socket() failed\n");
    exit(EXIT_FAILURE);
  }

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
