#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

 
int *a;
int *b;
long long M= 1000000000LL;
static void *thread_func(void *vptr_args)
{
    while((*b)!=M)
	{
		(*b)++;
//		fputs("  b\n", stderr);
	}
    return NULL;
}
 
int main(int argc,char *argv)
{
        printf("%d\n", M);
	time_t t1 = time(NULL);
	pthread_t thread;
	int *mass1 = malloc(64*sizeof(long long));
	int *mass2 = malloc(64*sizeof(long long));
	a = &mass1[0];
	b = &mass1[1];
	if(argc!=1)
    {
		int *b = &mass2[0];
		fprintf (stdout, "%s\n","far allocate");
    }
    if (pthread_create(&thread, NULL, thread_func, NULL) != 0)
    {
		fprintf (stderr, "%s","error1");
		free(mass1);
		free(mass2);
		return EXIT_FAILURE;
    }
	while((*a)!=M)
	{
		(*a)++;
//		fputs("a\n", stderr);
	}
    if (pthread_join(thread, NULL) != 0)
    {
		fprintf (stderr, "%s","error2");
		free(mass1);
		free(mass2);
		return EXIT_FAILURE;
    }
	free(mass1);
	free(mass2);
	time_t t2 = time(NULL);
	fprintf (stdout, "%s %d\n","time work", t2-t1);
    return EXIT_SUCCESS;
}

