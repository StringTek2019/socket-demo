//
// Created by stringtek on 2019/11/27.
//
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
//simplify output
#define line(str) std::cout<<str
const int MESSAGE_LEN=1024;
const int PORT=8888;
const int EVENT_LEN=20;
int main(int argc,char* argv[])
{
    int epoll_fd;
    int socket_fd;
    int client_fd;
    int ret;
    char in_buff[MESSAGE_LEN];
    unsigned int flags;
    int on = 1;
    epoll_event ev,events[EVENT_LEN];
    int event_num;
    // structure which to hold server address info
    // can be cast to sockaddr
    sockaddr_in server_addr,client_addr;
    //create tcp socket
    socket_fd=socket(AF_INET,SOCK_STREAM,0);
    if(socket_fd==-1){
        line("failed to create socket\n");
        exit(-1);
    }
    //set socket options REUSEADDR
    ret=setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
    if(ret==-1){
        line("failed to set socket option REUSEADDR");
        //just failed to set socket option, do not affect to use,so don't need to exit
    }
    //set socket to NONBLOCK
    flags=fcntl(socket_fd,F_GETFL);
    fcntl(socket_fd,F_SETFL,flags|O_NONBLOCK);
    //create epoll
    //os will ignore the arg of epoll_create but it cannot be set as 0
    epoll_fd=epoll_create(256);
    //add EPOLLIN event for epoll_fd,and set socket_fd to ev.data.fd
    // this for server socket means accept new client
    ev.events=EPOLLIN;
    ev.data.fd=socket_fd;
    epoll_ctl(epoll_fd,EPOLL_CTL_ADD,socket_fd,&ev);
    //set server address info
    server_addr.sin_family=AF_INET;//IPv4
    server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");//transfer string to in_addr_t
    server_addr.sin_port=PORT;
    memset(server_addr.sin_zero,0,8);
    //bind
    ret=bind(socket_fd,(sockaddr*)&server_addr,sizeof(sockaddr));
    if(ret==-1){
        line("failed to bind!\n");
        exit(-1);
    }
    for(;;){
        //event_num is the amount of to-do events
        //events is the to-do events array EVENT_LEN is len of it
        event_num=epoll_wait(epoll_fd,events,EVENT_LEN,500);
        for(int i=0;i<event_num;i++){
            //this means accept new client
            if(events[i].data.fd==socket_fd){
                socklen_t len=sizeof(sockaddr);
                client_fd=accept(socket_fd,(sockaddr*)&client_addr,&len);
                //set nonblock
                flags=fcntl(client_fd,F_GETFL);
                fcntl(client_fd,F_SETFL,flags|O_NONBLOCK);
                //set event for client we want to receive message from client so the event is EPOLLIN
                ev.events=EPOLLIN;
                ev.data.fd=client_fd;
                epoll_ctl(epoll_fd,EPOLL_CTL_ADD,client_fd,&ev);
            }else if(events[i].events&EPOLLIN){//client has data to read
                memset(in_buff,0,MESSAGE_LEN);
                ret=recv(events[i].data.fd,in_buff,MESSAGE_LEN,0);
                if(ret==-1){
                    line("error occurred!\n");
                }else if(ret==0){
                    close(events[i].data.fd);
                }else{
                    line("recv:");
                    line(in_buff);
                    line("\n");
                }
            }

        }
    }
    close(socket_fd);
    return 0;
}