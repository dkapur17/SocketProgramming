#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFF_SIZE 4096
#define PORT 6969
#define SA struct sockaddr

int getSize(char *fileName) {
    struct stat buff;
    stat(fileName, &buff);
    return buff.st_size;
}

void transmitFile(char *fileName, int sockfd) {
    char buff[BUFF_SIZE + 1];
    memset(buff, 0, BUFF_SIZE);
    printf("Client requested for %s\n", fileName);
    if (access(fileName, F_OK) == -1) {
        sprintf(buff, "~%s: File doesn't exist\n", fileName);
        write(sockfd, buff, BUFF_SIZE);
        return;
    }
    long int fileSize = getSize(fileName);
    sprintf(buff, "%ld", fileSize);
    write(sockfd, buff, BUFF_SIZE);
    int fd = open(fileName, O_RDONLY);

    long int bytesLeft = fileSize;
    while (bytesLeft >= BUFF_SIZE) {
        read(fd, buff, BUFF_SIZE);
        write(sockfd, buff, BUFF_SIZE);
        bytesLeft -= BUFF_SIZE;
    }
    if (bytesLeft) {
        read(fd, buff, bytesLeft);
        write(sockfd, buff, bytesLeft);
    }

    printf("%s: File transmission complete\n", fileName);
    close(fd);
}

void interact(int sockfd, char *terminate) {
    char buff[BUFF_SIZE];
    char *p = NULL;

    while (1) {
        memset(buff, 0, BUFF_SIZE);
        read(sockfd, buff, BUFF_SIZE);
        if (!strcmp("exit", buff)) {
            printf("Connection to client terminated\n");
            break;
        }
        if(!strcmp("kill", buff)){
            printf("Recieved kill command from the client. Exitting\n");
            *terminate = 1;
            break;
        }
        transmitFile(buff, sockfd);
    }
}

int main() {
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // Create Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Error! Unable to create socket!\n");
        exit(0);
    } else
        printf("Socket successfully created\n");

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Bind socket
    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0) {
        printf("Error! Unable to bind socket!\n");
        exit(0);
    } else
        printf("Socket successfully binded\n");

    // Start listening
    if ((listen(sockfd, 5)) != 0) {
        printf("Error! Unable to start listening on the socket!\n");
        exit(0);
    } else
        printf("Server is now listening on the socket\n");

    // Wait for connection request from client and accpet
    char terminate = 0;
   while(!terminate)
   {
        len = sizeof(cli);
        memset(&cli, 0, len);
        printf("Awaiting connection from client...\n");
        connfd = accept(sockfd, (SA *)&cli, &len);
        if (connfd < 0) {
            printf("Error! Unable to accept connection from the client!\n");
            exit(0);
        } else
            printf("Connection to client establisted successfully\n");

        interact(connfd, &terminate);
   }
    close(sockfd);
}