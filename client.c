#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
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

void printProgress(long int num, long int den) {
    float progress = 100.00 - ((((float)num)/den)*100);
    printf("\r");
    printf("Downloaded: %.2f%%", progress);
}

void recieveFile(int sockfd, long int fileSize, char* fileName) {
    char buff[BUFF_SIZE + 1];
    memset(buff, 0, BUFF_SIZE);
    int fd = open(fileName, O_CREAT | O_WRONLY, 0644);
    printf("\e[?25l");

    long int bytesLeft = fileSize;
    while (bytesLeft >= BUFF_SIZE) {
        read(sockfd, buff, BUFF_SIZE);
        write(fd, buff, BUFF_SIZE);
        printProgress(bytesLeft, fileSize);
        bytesLeft -= BUFF_SIZE;
    }
    if (bytesLeft) {
        read(sockfd, buff, bytesLeft);
        write(fd, buff, bytesLeft);
        bytesLeft = 0;
    }
    printProgress(bytesLeft, fileSize);
    printf("\e[?25h");
    printf("\n%s: Download Completed!\n", fileName);
    close(fd);
}

void interact(int sockfd) {
    char io[BUFF_SIZE];
    char buff[BUFF_SIZE];
    char* p = NULL;
    while (1) {
        memset(io, 0, BUFF_SIZE);
        printf("client> ");
        fgets(io, BUFF_SIZE, stdin);
        io[strlen(io) - 1] = '\0';
        if (!strlen(io))
            continue;
        p = strtok(io, " ");
        if (!strcmp(p, "exit")) {
            write(sockfd, p, strlen(p));
            printf("Client exitting\n");
            printf("\e[?25h");
            return;
        } 
        else if(!strcmp(p, "kill")){
            write(sockfd, p, strlen(p));
            printf("Client sent kill signal to server\n");
            printf("Client also exitting\n");
            printf("\e[?25h");
            return;
        }
        else if (!strcmp(p, "get")) {
            p = strtok(NULL, " ");
            if (p == NULL)
                printf("get: No files given\n");
            while (p) {
                memset(buff, 0, BUFF_SIZE);
                write(sockfd, p, strlen(p));
                read(sockfd, buff, BUFF_SIZE);

                if (buff[0] == '~') {
                    printf("%s", buff + 1);
                    p = strtok(NULL, " ");
                    continue;
                }
                long int fileSize = atol(buff);
                printf("Downloading %s: %ld bytes\n", p, fileSize);

                recieveFile(sockfd, fileSize, p);

                p = strtok(NULL, " ");
            }

        } else
            printf("%s: Command not found\n", p);
    }
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Error! Unable to create socket\n");
        exit(0);
    } else
        printf("Socket created successfully!\n");

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("Error! Unable to connect to the server\n");
        exit(0);
    } else
        printf("Connection to server established successfully\n");

    interact(sockfd);

    close(sockfd);
}