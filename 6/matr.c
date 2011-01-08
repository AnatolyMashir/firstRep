#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const char *printfErrorMessage = "some trouble with printf";
int main(int argc,char *argv)
{
	time_t t1 = time(NULL);
	int i,j,k;
    for(k=0; k<5; k++)
    {
	int **matr = (int **)malloc(10000*sizeof(int *));
	if(matr==NULL)
	{
		if(fprintf (stderr, "%s\n","no enough memory")==-1)
		{
			perror(printfErrorMessage);
		}
		return EXIT_FAILURE;
	}
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
				s+=matr[j][i];
		}
	}
	for(i=0; i<10000; i++)
	{
		free(matr[i]);
	}
	free(matr);
    }
	time_t t2 = time(NULL);
	if(fprintf (stdout, "\n%s %d\n","time work", t2-t1)==-1)
	{
			perror(printfErrorMessage);
			return EXIT_FAILURE;
	}
	return 0;
}




