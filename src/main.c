#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h> // To create the socket
#include <netinet/in.h> // For sockaddr_in struct
#include <string.h>
#include <unistd.h>

#define PORT_N 8080
#define BUF_SIZE 1024

int main(void){
  int server_fd;
  struct sockaddr_in server_address;
  socklen_t server_addrlen = sizeof(server_address);
  char buf[BUF_SIZE];
  char* resp = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello, World";
  
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
  printf("Socket creation succcess");
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(PORT_N);
  
  int binded = bind(server_fd, (struct sockaddr*) &server_address, server_addrlen);
  if( binded < 0){
    perror("Bind failed");
    exit(EXIT_FAILURE);
  }
  printf("Binding success");

  if(listen(server_fd, 3) < 0){
    perror("Listening failed");
    exit(EXIT_FAILURE);
  }
  printf("Server listening on port: %d", PORT_N);

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

    //Read request
    read(*client_fd, buf, BUF_SIZE);
    printf("Request: %s\n", buf);

    send(*client_fd, resp, strlen(resp), 0);
    close(*client_fd);
    free(client_fd);
  }
  close(server_fd);
  return 0;
}

