/*C programs to demonstrate TCP sockets programming.
  C code of client process that connects to server through socket.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>	
#include <netdb.h>      

#define SERVICE_PORT 8888	/*hard-coded port number.*/

int conn(char *host, int port);	/*Connects to host, port and returns socket.*/
void disconn(int fd);	/*To close a socket connection.*/

int debug = 1;

int main(int argc, char **argv) {
   extern char *optarg;
   extern int optind;
   int c, err = 0, fd, port = SERVICE_PORT;	/*SERVICE_PORT is the default port, File descriptor for socket.*/
   char *prompt = 0, *host = "localhost";	/*Default host.*/
   static char usage[] = "usage: %s [-h serverhost] [-p port]\n";
   while((c = getopt(argc, argv, "dh:p:")) != -1) {
      switch(c) {
         case 'h': host = optarg;
                   break;
         case 'p': port = atoi(optarg);
                   if(port < 1024 || port > 65535) {
                      fprintf(stderr, "Invalid port number: %s\n", optarg);
                      err = 1;
                   }
                   break;
         case '?': err = 1;
                   break;
      }
   }
   if(err || (optind < argc)) {	/*In case of error or extra arguments.*/
      fprintf(stderr, usage, argv[0]);
      exit(1);
   }
   printf("Connecting to %s, port %d\n", host, port);
   if((fd = conn(host, port)) < 0)	/*Connection.*/
      exit(1);	/*In case something goes wrong.*/	
   disconn(fd);	/*To disconnect.*/
   return 0;
}

int conn(char *host, int port) {
   struct hostent *hp;	/*For host information.*/
   unsigned int alen;	/*For address length when port number is received.*/
   struct sockaddr_in myaddr;	/*Client address.*/
   struct sockaddr_in servaddr;	/*Server address.*/
   int fd;  /*fd is the file descriptor for the connected socket.*/
   if(debug) 
      printf("Conn (host=\"%s\", port=\"%d\")\n", host, port);
   if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("Cannot create socket");
      return -1;
   }
   memset((char *) &myaddr, 0, sizeof(myaddr));
   myaddr.sin_family = AF_INET;
   myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
   myaddr.sin_port = htons(0);
   if(bind(fd, (struct sockaddr *) &myaddr, sizeof(myaddr)) < 0) {
      perror("Bind failed");
      close(fd);
      return -1;
   }	
   alen = sizeof(myaddr);
   if(getsockname(fd, (struct sockaddr *) &myaddr, &alen) < 0) {
      perror("Getsockname failed");
      close(fd);
      return -1;
   }	
   if(debug)
      printf("Local port number = %d\n", ntohs(myaddr.sin_port));
   memset((char*) &servaddr, 0, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_port = htons(port);
   hp = gethostbyname(host);
   if(!hp) {
      fprintf(stderr, "Could not obtain address of %s\n", host);
      close(fd);
      return -1;
   }
   memcpy((void *) &servaddr.sin_addr, hp->h_addr_list[0], hp->h_length);  /*To put the host's address into the server address structure.*/
   if(connect(fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {  /*To connect to server.*/
      perror("Connect failed");
      close(fd);
      return -1;
   }
   if(debug)
      printf("Connected socket = %d\n", fd);
   return fd;
}

void disconn(int fd) {   /*To disconnect from the server.*/
   if(debug) 
      printf("Disconn (%d)\n", fd);
   shutdown(fd, 2); /*2 means future sends & receives are disallowed.*/
}
