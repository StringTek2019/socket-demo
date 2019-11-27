//
// Created by stringtek on 2019/11/27.
//

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#define line(str) std::cout<<str
const int PORT=8888;
const int MESSAGE_LEN=1024;
const int FD_SET_LEN=1024;
int main(int argc,char* argv[])
{
    int socket_fd;
    int ret;
    int backlog=20;
    int on = 1;
    int pos=-1;
    int accept_fds[FD_SET_LEN];
    int max_fd=-1;
    int flags;
    char in_buff[MESSAGE_LEN];
    int accept_fd;
    timeval timeout;
    memset(accept_fds,-1,sizeof(accept_fds));
    for(int i=0;i<FD_SET_LEN;i++){
        if(accept_fds[i]!=-1){
            line(i);
            line("\n");
        }
    }
    fd_set readable_set;
    sockaddr_in server_addr,client_addr;
    socket_fd=socket(AF_INET,SOCK_STREAM,0);
    if(socket_fd==-1){

    }
    max_fd=socket_fd;
    ret=setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
    if(ret==-1){

    }
    flags=fcntl(socket_fd,F_GETFL);
    fcntl(socket_fd,F_SETFL,flags|O_NONBLOCK);
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=PORT;
    server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    memset(server_addr.sin_zero,0,8);
    ret=bind(socket_fd,(sockaddr*)&server_addr,sizeof(sockaddr));
    if(ret==-1){

    }
    ret=listen(socket_fd,backlog);
    if(ret==-1){
        
    }
    for(;;){
        timeout.tv_sec=0;
        timeout.tv_sec=5*1000*1000;
        FD_ZERO(&readable_set);
        FD_SET(socket_fd,&readable_set);
        for(int i=0;i<FD_SET_LEN;i++){
            if(accept_fds[i]!=-1){
                if(accept_fds[i]>max_fd)
                    max_fd=accept_fds[i];
                FD_SET(accept_fds[i],&readable_set);
            }
        }

        ret=select(max_fd+1,&readable_set,nullptr,nullptr,&timeout);
        if(ret==-1){

        }else if(ret==0){
            continue;
        }else if(ret>0){
            if(FD_ISSET(socket_fd,&readable_set)){
                for(int i=0;i<FD_SET_LEN;i++){
                    if(accept_fds[i]==-1){
                        pos=i;
                        break;
                    }
                }
                socklen_t len=sizeof(sockaddr);
                accept_fd=accept(socket_fd,(sockaddr*)&client_addr,&len);
                flags=fcntl(accept_fd,F_GETFL);
                fcntl(accept_fd,F_SETFL,flags|O_NONBLOCK);
                accept_fds[pos]=accept_fd;
            }
            for(int i=0;i<FD_SET_LEN;i++){
                if(accept_fds[i]!=-1&&FD_ISSET(accept_fds[i],&readable_set)){
                    memset(in_buff,0,MESSAGE_LEN);
                    ret=recv(accept_fds[i],in_buff,MESSAGE_LEN,0);
                    if(ret==0){
                        close(accept_fds[i]);
                    }
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