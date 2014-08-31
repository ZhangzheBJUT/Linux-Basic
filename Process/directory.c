#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>


int main(int argc,char**argv)
{
	int size = getdtablesize();
	printf("getdtablesize size:%d\n",size);

    int i; 
	for (i=0;i<getdtablesize();i++)
		close(i);         
	printf("exit......\n");
	return 0;
}

