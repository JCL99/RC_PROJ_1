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
struct sockaddr_in server_addr, client_addr;

int main(int argc, char **argv, char **envp){

  /* Validate args */
  if (argc < 3){
    fprintf(stderr, "[!] Wrong or missing arguments\n");
    fprintf(stderr, "[!] Try : ./chat-client <addr> <port>\n");
    exit(EXIT_FAILURE);
  }

  return 0;
}
