#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main()
{
	int data_processed;
	int file_pipes[2];
	const char some_data[] = "1233";
	char buffer[BUFSIZ+1];

	memset(buffer,'\0',sizeof(buffer));

	if (pipe(file_pipes)==0)
	{
		data_processed = write(file_pipes[1],some_data,strlen(some_data));
		printf("Wrote %d bytes \n",data_processed);

		data_processed = read(file_pipes[0],buffer,BUFSIZ);
		printf("Read %d bytes \n",data_processed);
		exit(EXIT_SUCCESS);
	}
	exit(EXIT_FAILURE);
}
/*
这个程序用数组file_pipes[]中的两个文描述符创建一个管道，然后它用文件描述符file_pipes[1]向管道中写数据
，再从file_pipes[0]读回数据。注意，管道有一些内置的缓存区，它在write和read调用之间保持数据。

管道的真正优势体现在，当你想在两个进程之间传递数据的时候。当程序用fork调用创建新进程时，原先打开
的文件描述符仍将保持打开状态。如果在原先的进程中创建一个管道，然后再调用fork创建新进程，我们即可以通过
管道在两个进程之间传递数据。
 */
