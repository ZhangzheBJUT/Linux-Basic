#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>


#include <sys/msg.h>

#define MAX_TEXT 512

struct my_msg_st {
	long int my_msg_type;
	char some_text[BUFSIZ];
};


int main()
{
	int running = 1;
	int msgid;
	struct my_msg_st some_data;
    char buffer[BUFSIZ];

	msgid = msgget((key_t)1234,0666 | IPC_CREAT);
	if (msgid == -1)
	{
		fprintf(stderr,"msgget failed with error: %d\n",errno);
		exit(EXIT_FAILURE);
	}


	while(running)
	{
		printf("Enter som text:");
		fgets(buffer,BUFSIZ,stdin);
		some_data.my_msg_type = 1;
		strcpy(some_data.some_text,buffer);

		if(msgsnd(msgid,(void*)&some_data,MAX_TEXT,0) == -1)
		{
			fprintf(stderr,"msgsnd failed.\n"); 
			exit(EXIT_FAILURE);
		}


		if(strncmp(some_data.some_text,"end",3) == 0)
		{
			running = 0;
		}

    }
	
   exit(EXIT_FAILURE);
}
