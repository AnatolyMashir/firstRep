#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "iatomic.h"

#define false 0
#define true 1

const char *printfErrorMessage = "some trouble with printf";
atomic_t flag1 ATOMIC_INIT(false);
atomic_t flag2 ATOMIC_INIT(false);
atomic_t turn ATOMIC_INIT(0);
atomic_t criticalResource ATOMIC_INIT(0);

void *proc1(void *)
{
	int i;
	for(i=0; i<10000000; i++)
	{
		atomic_set( &flag1, true);
		while(atomic_read(&flag2) == true)
		{
			if(atomic_read(&turn) != 0)
			{
				atomic_set( &flag1, false);
				while(atomic_read(&turn) != 0);
				atomic_set( &flag1, true);
			}
		}
		
		atomic_inc(&criticalResource);
		atomic_set( &turn, 1);
		atomic_set( &flag1, false);
	}
}

void *proc2(void *)
{
	int i;
	for(i=0; i<10000000; i++)
	{
		atomic_set( &flag2, true);
		while(atomic_read(&flag1) == true)
		{
			if(atomic_read(&turn) != 1)
			{
				atomic_set( &flag2, false);
				while(atomic_read(&turn) != 1);
				atomic_set( &flag2, true);
			}
		}
		atomic_inc(&criticalResource);
		atomic_set( &turn, 0);
		atomic_set( &flag2, false);
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
		perror("i can't create thread");
		return EXIT_FAILURE;
    }
	if (pthread_setaffinity_np(thread1, sizeof(cpu_set_t), &cpu1) || pthread_setaffinity_np(thread2, sizeof(cpu_set_t), &cpu2))
	{
		perror("i can't execute 'set affinity' for thread");
		return EXIT_FAILURE;
	}
	if (pthread_join(thread1, NULL) || pthread_join(thread2, NULL))
    {
		perror("i can't join threads");
		return EXIT_FAILURE;
    }
	if(fprintf (stdout, "%s %d\n","criticalResource=", criticalResource)==-1)
	{
		perror(printfErrorMessage);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

