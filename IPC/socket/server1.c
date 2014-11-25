#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>


int main()
{
    int server_sockfd,client_sockfd;
    int server_len,client_len;

    struct sockaddr_un server_address;
    struct sockaddr_un client_address;
    
    unlink("server_socket");
    //创建套接字
    server_sockfd = socket(AF_UNIX,SOCK_STREAM,0);
    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path,"server_socket");
    server_len = sizeof(server_address);

    //命名套接字
    bind(server_sockfd,(struct sockaddr*)&server_address,server_len);
    
    //创建套接字队列
    listen(server_sockfd,5);

    //接受连接
    while(1)
    {
       char ch;
       printf("server waiting\n");
       client_len = sizeof(client_address);
       client_sockfd=accept(server_sockfd,(struct sockaddr*)&client_address,&client_len);
       read(client_sockfd,&ch,1);
       printf("get data from server:%c\n",ch);
       ch++;
     
       write(client_sockfd,&ch,1);
       //关闭套接字
       close(client_sockfd);
     }
}

