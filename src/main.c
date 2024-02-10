#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h> // To create the socket
#include <netinet/in.h> // For sockaddr_in struct
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PORT_N 8080
#define BUF_SIZE 1048576 //Equal to 1MB

void client_handler(int* arg);
int get_file_size(int fd);


int main(void){
  int server_fd;
  struct sockaddr_in server_address;
  socklen_t server_addrlen = sizeof(server_address);
  char buf[BUF_SIZE];

  
  /* Create a new socket, AF_INET is a const declared in sys/socket.h
     and means we are using IPv4 address family. SOCK_STREAM is also
     declared in sys/socket.h and means we create a byte-stream socket.
     Last argument is protocol, if 0, protocol will be chosen on request.
     socket() returns non negative value if successful.
  */
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(server_fd < 0){ 
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(PORT_N);
  
  int binded = bind(server_fd, (struct sockaddr*) &server_address, server_addrlen);
  if( binded < 0){
    perror("Bind failed");
    exit(EXIT_FAILURE);
  }


  if(listen(server_fd, 3) < 0){
    perror("Listening failed");
    exit(EXIT_FAILURE);
  }
  printf("Server listening on port: %d\n", PORT_N);

  //Get client info and send a response
  while(1){
    int* client_fd = malloc(sizeof(int));
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);
    *client_fd = accept(server_fd, (struct sockaddr*) &client_address, (socklen_t*)&client_len);
    
    if(*client_fd < 0){
      perror("Error on accept");
      exit(EXIT_FAILURE);
    }
    client_handler(client_fd);
    close(*client_fd);
    free(client_fd);
  }
  close(server_fd);
  return 0;
}

void client_handler(int* arg){
  int client_fd = *arg;
  char* buf = (char*)malloc(BUF_SIZE);
  char* resp = (char*)malloc(BUF_SIZE);
  int index_fd = open("index.html", O_RDONLY);
  ssize_t bytes_read;
  int f_size = get_file_size(index_fd);
  sprintf(resp, "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: %d\n\n", f_size);
  send(client_fd, resp, strlen(resp), 0);

  while(read(index_fd, buf, BUF_SIZE) > 0){
    send(client_fd, buf, strlen(buf), 0);
  }
  close(index_fd);
  free(buf);
  free(resp);
}

int get_file_size(int fd){
  struct stat file_stat;
  fstat(fd, &file_stat);
  int f_size = (int)file_stat.st_size;
  return f_size;
}
