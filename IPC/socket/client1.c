#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    int sockfd;
    int len;
    struct sockaddr_un address;
    int result;
    char ch = 'A';
    //create socket 
    sockfd = socket(AF_UNIX,SOCK_STREAM,0);
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path,"server_socket");
    len = sizeof(address);

    //连接套接字
    result = connect(sockfd,(struct sockaddr*)&address,len);
    
    if (result==-1)
    {
         perror("oops:client");
	 exit(1);
    }
    write(sockfd,&ch,1);
    read(sockfd,&ch,1);
    printf("char from server = %c\n",ch);
    close(sockfd);
    exit(0);
}
    
