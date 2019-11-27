//
// Created by stringtek on 2019/11/27.
//
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#define line(str) std::cout<<str
const int PORT=8888;
const int MESSAGE_LEN=1024;
int main()
{
    int socket_fd;
    int ret;
    sockaddr_in client_addr,server_addr;
    char out_buff[MESSAGE_LEN];
    //create tcp socket
    socket_fd=socket(AF_INET,SOCK_STREAM,0);
    if(socket_fd==-1){
        line("failed to create socket!\n");
        exit(-1);
    }
    //set info of server_addr
    server_addr.sin_family=AF_INET;//IPv4
    server_addr.sin_port=PORT;
    server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    memset(server_addr.sin_zero,0,8);
    ret=connect(socket_fd,(sockaddr*)&server_addr,sizeof(sockaddr));
    if(ret==-1){
        line("failed to connect!\n");
        exit(-1);
    }
    for(;;){
        //get input from stdin(usually is keyboard)
        fgets(out_buff,MESSAGE_LEN,stdin);
        //alter the last character from \n to \0
        out_buff[strlen(out_buff)-1]='\0';
        ret=send(socket_fd,out_buff,MESSAGE_LEN,0);
        if(ret==-1){
            line("failed to send\n");
        }
    }

}
