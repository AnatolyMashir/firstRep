#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(argc, argv)
int argc;
char *argv[];
{
    int i;
    time_t t1 = time(NULL);
    for(i=0; i<10; i++)
    {
	int flag = 0;
	if(argc!=1)
	{
    	    flag = 1;
        }
	fprintf (stderr, "%s %d\n","flag", flag);
	int **matr = malloc(10000*sizeof(int *));
	int i,j;
	for(i=0; i<10000; i++)
	{
		matr[i] = malloc(10000*sizeof(int *));
	}
	for(i=0; i<10000; i++)
	{
		for(j=0; j<10000; j++)
		{
			matr[i][j] =  (i+j)%10000;
		}
	}
	long s = 0;
	for(i=0; i<10000; i++)
	{
		for(j=0; j<10000; j++)
		{
			if(flag)
			{
				//fprintf (stderr, "%s\n","s1");
				s+=matr[i][j];
			}
			else
			{
				//fprintf (stderr, "%s\n","s2");
				s+=matr[j][i];	
			}
		}
	}
	for(i=0; i<10000; i++)
	{
		free(matr[i]);
	}
	free(matr);
	fprintf (stderr, "%s %d\n","result", s);
	
    }
    time_t t2 = time(NULL);
    fprintf (stdout, "%s %d\n","time work", t2-t1);
    return 0;
}

