#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void ouch(int sig)
{
	printf("OUCH!-I got signal %d\n",sig);
	//恢复信号SIGINT的默认处理行为
	(void) signal(SIGINT,SIG_DFL);
}


int main()
{
	//自定义信号SIGINT的处理行为
	(void)signal(SIGINT,ouch);

	while(1)
	{
		printf("Hello world!\n");
		sleep(1);
	}
}
