#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

const char *printfErrorMessage = "some trouble with printf";
const char *getAffinityMessage = "some trouble with get affinity";
int main(int argc,char *argv)
{
	time_t t1 = time(NULL);
	int flag = 0;
	if(argc!=1)
	{
        flag = 1;
		if(fprintf (stdout, "%s\n","summation by row")==-1)
		{
			perror(printfErrorMessage);
			return EXIT_FAILURE;
		}
    }
	else
	{
		if(fprintf (stdout, "%s\n","summation by column")==-1)
		{
			perror(printfErrorMessage);
			return EXIT_FAILURE;
		}
	}
	int **matr = (int **)malloc(10000*sizeof(int *));
	if(matr==NULL)
	{
		if(fprintf (stderr, "%s\n","no enough memory")==-1)
		{
			perror(printfErrorMessage);
		}
		return EXIT_FAILURE;
	}
	int i,j;
	for(i=0; i<10000; i++)
	{
		matr[i] = (int *)malloc(10000*sizeof(int *));
		if(matr==NULL)
		{
			if(fprintf (stderr, "%s\n","no enough memory")==-1)
			{
				perror(printfErrorMessage);
			}
			for(j=0; j<i; j++)
			{
				free(matr[j]);
			}
			free(matr);
			return EXIT_FAILURE;
		}
	}
	for(i=0; i<10000; i++)
	{
		for(j=0; j<10000; j++)
		{
			matr[i][j] = (i+j)%10000;
		}
	}
	long s = 0;
	for(i=0; i<10000; i++)
	{
		for(j=0; j<10000; j++)
		{
			if(flag)
			{
				s+=matr[i][j];
			}
			else
			{
				s+=matr[j][i];
			}
		}
	}
	for(i=0; i<10000; i++)
	{
		free(matr[i]);
	}
	free(matr);
	time_t t2 = time(NULL);
	cpu_set_t cpu;
	if(sched_getaffinity(0,sizeof(cpu_set_t), &cpu))
	{
		if(fprintf (stderr, "%s\n","I can't get cpu set")==-1)
		{
			perror(getAffinityMessage);
			return EXIT_FAILURE;
		}
	}
	if(fprintf (stdout, "%s: ","cpu set")==-1)
	{
			perror(printfErrorMessage);
			return EXIT_FAILURE;
	}
	for(i=0; i<10; i++)
	{
		if(CPU_ISSET(i, &cpu))
		{
			if(fprintf (stdout, "%d ",i)==-1)
			{
				perror(printfErrorMessage);
				return EXIT_FAILURE;
			}
		}
	}
	if(fprintf (stdout, "\n%s %d\n","time work", t2-t1)==-1)
	{
			perror(printfErrorMessage);
			return EXIT_FAILURE;
	}
	return 0;
}




