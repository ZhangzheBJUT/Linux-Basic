#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(int argc,char* argv[])
{
	int data_processed;
	int file_descriptor;
	char buffer[BUFSIZ+1];

	memset(buffer,'\0',sizeof(buffer));
	sscanf(argv[1],"%d",&file_descriptor);

	data_processed = read(file_descriptor,buffer,BUFSIZ);
	printf("%d Read %d bytes :%s \n",getpid(),data_processed,buffer);
	exit(EXIT_SUCCESS);
}
