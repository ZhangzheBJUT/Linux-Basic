#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char**argv)
{
	int fd; //文件描述符
	char* mapped, *p;
	int flength      = 1024;
	void *start_addr = 0;

	if (argc < 2)
	{
		printf("argc less than 2\n");
		exit(-1);
	}

    fd = open(argv[1],O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);
	flength = lseek(fd,1,SEEK_END);
	write(fd,"\0",1); //在文件末尾添加一个空字符
	lseek(fd,0,SEEK_SET);
	mapped = mmap(start_addr,flength,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    printf("%s\n",mapped);

	while((p=strstr(mapped,"test")))
	{
		memcpy(p,"map",3);
		p+=3;
	}
	munmap((void*)mapped,flength);
	close(fd);

	return 0;
}

