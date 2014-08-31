#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>


int main(int argc,char**argv)
{
	pid_t pid;
	int i,fd;

	char*buf = "This is a daemon program.\n";
	pid = fork();
	if (pid < 0)
	{
		printf("fork error!");
		exit(-1);
	}
	else if (pid > 0)
	{
		exit(0); //父进程退出
	}

	setsid();    //子进程中创建新会话
	chdir("/");  //设置工作目录为根目录
	umask(0);    //设置权限掩码

	for (i=0;i<getdtablesize();i++)  //getdtablesize返回子进程文件描述符表项数
		close(i);                    //关闭这些不将用到的文件描述符

	while(1)
	{
		fd = open("/tmp/daemon.log",O_CREAT|O_WRONLY|O_APPEND,0600);
		if(fd < 0)
		{
			printf("Open file error!\n");
			exit(1);
		}

		write(fd,buf,strlen(buf)+1);
		close(fd);
		sleep(10);
		printf("Never output!\n");
	}
	return 0;
}

