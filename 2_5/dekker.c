#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define false 0
#define true 1

const char *printfErrorMessage = "some trouble with printf";
int flag[2] = {false,false};
volatile int turn = 0;
volatile int criticalResource = 0;

void *proc1(void *)
{
	int i;
	for(i=0; i<1000000000; i++)
	{
		flag[0] = true;
		while(flag[1] == true)
		{
			if(turn != 0)
			{
				flag[0] = false;
				while(turn != 0);
				flag[0] = true;
			}
		}
		
		criticalResource++;
		turn = 1;
		flag[0] = false;
	}
}

void *proc2(void *)
{
	int i;
	for(i=0; i<1000000000; i++)
	{
		flag[1] = true;
		while(flag[0] == true)
		{
			if(turn != 1)
			{
				flag[1] = false;
				while(turn != 1);
				flag[1] = true;
			}
		}
		
		criticalResource++;
		turn = 0;
		flag[1] = false;
	}
}

int main(int argc,char **argv)
{
	pthread_t thread1, thread2;
	cpu_set_t cpu1, cpu2;
	CPU_ZERO(&cpu1);
	CPU_SET(0, &cpu1);
	CPU_ZERO(&cpu2);
	CPU_SET(1, &cpu2);
	if (pthread_create(&thread1, NULL, proc1, NULL) || pthread_create(&thread2, NULL, proc2, NULL))
    {
		if(fprintf (stderr, "%s\n","i can't create thread")==-1)
		{
			perror(printfErrorMessage);
		}
		return EXIT_FAILURE;
    }
	if (pthread_setaffinity_np(thread1, sizeof(cpu_set_t), &cpu1) || pthread_setaffinity_np(thread2, sizeof(cpu_set_t), &cpu2))
	{
		if(fprintf (stderr, "%s\n","i can't execute 'set affinity' for thread")==-1)
		{
			perror(printfErrorMessage);
		}
		return EXIT_FAILURE;
	}
	if (pthread_join(thread1, NULL) || pthread_join(thread2, NULL))
    {
		if(fprintf (stderr, "%s\n","i can't join thread")==-1)
		{
			perror(printfErrorMessage);
		}
		return EXIT_FAILURE;
    }
	if(fprintf (stdout, "%s %d\n","criticalResource=", criticalResource)==-1)
	{
		perror(printfErrorMessage);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
