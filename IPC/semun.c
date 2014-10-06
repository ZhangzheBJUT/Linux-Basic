#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
//#include "test.h"
#include <sys/sem.h>
//#include "semun.h"

union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

static int set_semvalue(void);
static void del_semvalue(void);
static int semaphore_p(void);
static int semaphore_v(void);

static int sem_id;

int main(int argc,char **argv)
{
	int i ;
	int pause_time;
	char op_char = 'O';
	srand((unsigned int)getpid());

            //create semaphore
	sem_id = semget((key_t)1234,1,0666|IPC_CREAT);


	if (argc >1)
	{
		//set sempvalue
		if (!set_semvalue())
		{
			fprintf(stderr,"Failed to initialize semaphore");
			exit(EXIT_FAILURE);
		}
		op_char = 'X';
		sleep(2);
	}

	for (i = 0; i < 10; ++i)
	{
		if (!semaphore_p())
		{
			exit(EXIT_FAILURE);
		}
		printf("%c",op_char);
		fflush(stdout);
		pause_time = rand() % 3;
		sleep(pause_time);
		printf("%c",op_char);
		fflush(stdout);
		if (!semaphore_v())
		{
			exit(EXIT_FAILURE);
		}
		pause_time = rand() % 3;
		sleep(pause_time);

	}

	printf("\n%d - finished \n",getpid());
	if (argc>1)
	{
		sleep(10);
		del_semvalue();
	}
	exit(EXIT_SUCCESS);
}


static int set_semvalue(void)
{
	union semun sem_union;
	sem_union.val = 1;
	if (semctl(sem_id,0,SETVAL,sem_union) == -1)
	{
		return 0;
	}
	return 1;
} 

static void del_semvalue()
{
	union semun sem_union;
	sem_union.val = 1;
	if (semctl(sem_id,0,IPC_RMID,sem_union) == -1)
		fprintf(stderr, "Failedto delete semaphore.\n");
}


static int semaphore_p()
{
	struct sembuf sem_b;

	sem_b.sem_num = 0;
	sem_b.sem_op     = -1;
	sem_b.sem_flg  = SEM_UNDO;

	if (semop(sem_id,&sem_b,1)== -1)
	{
		fprintf(stderr,"sempore_p failed.");
		return 0;
	}
	return 1;
}

static int semaphore_v()
{
	struct sembuf sem_b;

	sem_b.sem_num = 0;
	sem_b.sem_op     = 1;
	sem_b.sem_flg  = SEM_UNDO;

	if (semop(sem_id,&sem_b,1)== -1)
	{
		fprintf(stderr,"sempore_p failed.");
		return 0;
	}
	return 1;
}
