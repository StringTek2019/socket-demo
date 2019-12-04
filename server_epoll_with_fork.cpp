//
// Created by stringtek on 2019/12/3.
//

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <wait.h>

#define line(str) std::cout<<str
const int MAX_EV_LEN=1024;
const int PORT=8888;
const int MESSAGE_LEN=1024;
const int PROCESS=4;
int main(int argc,char* argv[])
{
    int socket_fd;
    int client_fd;
    int ret;
    int on = 1;
    int flags;
    int backlog=10;
    int epoll_fd;
    int ev_amount;
    pid_t pid=-1;
    char in_buff[MESSAGE_LEN];
    epoll_event ev,event[MAX_EV_LEN];
    sockaddr_in server_addr,client_addr;
    socket_fd=socket(AF_INET,SOCK_STREAM,0);
    if(socket_fd==-1){
        line("failed to create socket!\n");
        exit(-1);
    }
    ret=setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
    if(ret==-1){
        line("failed to set socket option REUSEADDR!\n");
    }
    flags=fcntl(socket_fd,F_GETFL);
    fcntl(socket_fd,F_SETFL,O_NONBLOCK|flags);
    server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=PORT;
    memset(server_addr.sin_zero,0,sizeof(server_addr.sin_zero));
    ret=bind(socket_fd,(sockaddr*)&server_addr,sizeof(sockaddr));
    if(ret==-1){
        line("failed to bind!\n");
        exit(-1);
    }
    ret=listen(socket_fd,backlog);
    if(ret==-1){
        line("failed to listen!\n");
        exit(-1);
    }
    epoll_fd=epoll_create(256);
    ev.events=EPOLLIN;
    ev.data.fd=socket_fd;
    epoll_ctl(epoll_fd,EPOLL_CTL_ADD,socket_fd,&ev);
    for(int i=0;i<PROCESS;i++){
        if(pid!=0){
            pid=fork();
        }
    }
    if(pid==0){
        for(;;){
            ev_amount=epoll_wait(epoll_fd,event,MAX_EV_LEN,500);
            for(int i=0;i<ev_amount;i++){
                if(event[i].data.fd==socket_fd){
                    line("catch new client...\n");
                    socklen_t len=sizeof(sockaddr);
                    client_fd=accept(socket_fd,(sockaddr*)&client_addr,&len);
                    flags=fcntl(client_fd,F_GETFL);
                    fcntl(client_fd,F_SETFL,flags|O_NONBLOCK);
                    ev.events=EPOLLIN|EPOLLET;
                    ev.data.fd=client_fd;
                    epoll_ctl(epoll_fd,EPOLL_CTL_ADD,client_fd,&ev);
                }else if(event[i].events&EPOLLIN){
                    memset(in_buff,0,sizeof(in_buff));
                    ret=recv(event[i].data.fd,in_buff,MESSAGE_LEN,0);
                    if(ret==0){
                        close(event[i].data.fd);
                    }else if(ret==-1){
                        line(errno);
                    }else{
                        line("recv:");
                        line(in_buff);
                        line("\n");
                    }
                }
            }
        }
    }else{
        do{
            pid=waitpid(-1,nullptr,0);
        }while(pid!=-1);
    }
    line("quit server...");
    close(socket_fd);

    return 0;
}