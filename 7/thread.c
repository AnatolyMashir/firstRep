#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

const char *printfErrorMessage = "some trouble with printf";
const char *getAffinityMessage = "some trouble with get affinity";
volatile long long int *a;
volatile long long int *b;
volatile int count = 1;
long long M = 1000000000LL;
volatile long long mArray[10];
static void *thread_func(void *args)
{
	int *c;
	c = (int*)args;
   while((*c)!=M)
	{
		(*c)++;
	}
    return NULL;
}
 
int main(int argc,char *argv)
{
	time_t t1 = time(NULL);
	pthread_t thread1, thread2;
	cpu_set_t cpu1, cpu2;
	CPU_ZERO(&cpu1);
	CPU_SET(0, &cpu1);
	CPU_ZERO(&cpu2);
	CPU_SET(1, &cpu2);
	a = &mArray[0];
	b = &mArray[1];
	if(argc!=1)
	{
        b = &mArray[10];
		if(fprintf (stdout, "%s\n","far allocate")==-1)
		{
			perror(printfErrorMessage);
			return EXIT_FAILURE;
		}
    }
	else
	{
		if(fprintf (stdout, "%s\n","near allocate")==-1)
		{
			perror(printfErrorMessage);
			return EXIT_FAILURE;
		}
	}
    if (pthread_create(&thread1, NULL, thread_func, (void*)(a)) || pthread_create(&thread2, NULL, thread_func,(void*)(b)))
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
	if(fprintf (stdout, "%s\n%s\n","thread1 work on cpu 0", "thread2 work on cpu 1")==-1)
	{
		perror(printfErrorMessage);
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
	time_t t2 = time(NULL);
	if(fprintf (stdout, "%s %d\n","time work", t2-t1)==-1)
	{
		perror(printfErrorMessage);
		return EXIT_FAILURE;
	}
    return EXIT_SUCCESS;
}
