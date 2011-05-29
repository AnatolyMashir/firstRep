#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
//#include <curses.h>
#include <termio.h>
#include <unistd.h>

#define none 4
#define ask 3
#define quit 10
#define error 11

#include "iatomic.h"

const char *printfErrorMessage = "some trouble with printf";
const char *getAffinityMessage = "some trouble with get affinity";
int ***matrs;
atomic_t lock ATOMIC_INIT(none);
atomic_t current ATOMIC_INIT(0);
atomic_t next ATOMIC_INIT(1);
volatile int count;
char key;

int mygetch( ) 
{
	struct termios oldt,
	newt;
	int ch;
	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newt );
	ch = getchar();
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
	return ch;
}

void errorMain(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void errorUser(const char *msg)
{
    perror(msg);
    atomic_set(&lock, quit);
	pthread_exit(NULL);
}

void errorServer(const char *msg)
{
    perror(msg);
	atomic_set(&lock, error);
	signal (SIGALRM, SIG_DFL); // чтобы ваша душа была спокойна
	pthread_exit(NULL);
}

static void *userInt(void *args)
{
	int i, j;
	key = mygetch();
	while(key!='q')
	{
		atomic_set( &lock, ask);
		while(atomic_read(&lock)==ask);
		if(atomic_read(&lock)<3)
		{
			for(i=0;  i<count; i++)
			{
				for(j=0;  j<count; j++)
				{
					if(printf("%c", matrs[atomic_read(&lock)][i][j])==-1)
						errorUser(printfErrorMessage);
				}
				if(printf("\n")==-1)
					errorUser(printfErrorMessage);
			}
			if(printf("\n")==-1)
				errorUser(printfErrorMessage);
			atomic_set(&lock, none);
			key = mygetch();
		}
		else
		{
			key='q';
		}
	}
	atomic_set(&lock, quit);
}
 
char reCount(int i, int j, int current)
{
	int c=0;
	if(i-1>=0)
	{
		if(j-1>=0 && matrs[current][i-1][j-1]=='*')
			c++;
		if(matrs[current][i-1][j]=='*')
			c++;
		if(j+1<count && matrs[current][i-1][j+1]=='*')
			c++;
	}
	if(j-1>=0 && matrs[current][i][j-1]=='*')
		c++;
	if(j+1<count && matrs[current][i][j+1]=='*')
		c++;
	if(i+1<count)
	{
		if(j-1>=0 && matrs[current][i+1][j-1]=='*')
			c++;
		if(matrs[current][i+1][j]=='*')
			c++;
		if(j+1<count && matrs[current][i+1][j+1]=='*')
			c++;
	}
	//fprintf(stdout, "i=%d, j=%d, c=%d\n", i, j, c);
	if(c<3 || c>6)
	{
		return ' ';
	}
	return '*';
}

void reBild(int current, int next)
{
	int i, j;
	for(i=0;  i<count; i++)
	{
		for(j=0;  j<count; j++)
		{
			matrs[next][i][j]=reCount(i,j,current);
		}
	}
}

void handler(int signal, siginfo_t *info, void *something)
{
	fprintf(stdout, "%s\n","recount");
	int tmp = ATOMIC_INIT(atomic_read(&next));
	if(atomic_sub_and_test( tmp, &lock ))
	{
		atomic_set(&lock, (atomic_read(&next)+1)%3);
	}	
	reBild(atomic_read(&current), atomic_read(&next));
	alarm(1);
	atomic_set(&current, atomic_read(&next));
}

void *bildLife(void *args)
{	
	struct sigaction S;
	S.sa_sigaction = handler;
	sigfillset(&S.sa_mask);
	if(sigfillset(&S.sa_mask))
		errorServer("some trouble with mask");
	if(sigaction(SIGALRM, &S, NULL))
		errorServer("I can't set signal");
	alarm(1);
	while(atomic_read(&lock)!=quit)
	{
		if(atomic_read(&lock)==ask)				
		{			
			atomic_set(&lock,atomic_read(&current));
		}							
	}
	signal (SIGALRM, SIG_DFL);
}

void init(int k)
{
	int i, j;
	srand(time(NULL));
	for(i=0; i<count; i++)
	{
		for(j=0; j<count; j++)
		{
			matrs[k][i][j] = rand()%2?' ':'*';
		}
	}
}

void freeMatr(int i, int j)
{
	int k=j-1, l=i;
	if(j==0)
	{
		free(matrs[l]);
	}
	else if(j!=count)
	{
		for(; k>=0; k--)
		{
			free(matrs[l][k]);
		}
		free(matrs[l]);
	}
	for(l=i-1; l>=0; l--)
	{
		for(k=0; k<count; k++)
			free(matrs[l][k]);
		free(matrs[l]);
	}
	free(matrs);
}

int main(int argc,char *argv[])
{
	pthread_t thread1, thread2;
	if(argc!=2)
		errorMain("need count");
	if(!sscanf(argv[1], "%d", &count))
		errorMain("i, can not cast argument 1");
	int i,j,k;
	matrs = (int ***)malloc(3*sizeof(int **));
	if(matrs==NULL)
		errorMain("no enough memory");
	for(i=0; i<3; i++)
	{
		matrs[i] = (int **)malloc(count*sizeof(int *));
		if(matrs[i]==NULL)
		{
			freeMatr(i,0);
			errorMain("No enough memory");
		}	
		for(j=0; j<count; j++)
		{
			matrs[i][j]=(int *)malloc(count*sizeof(int));
			if(matrs[i][j]==NULL)
			{
				freeMatr(i,j);
				errorMain("No enough memory");
			}
		}
	}
	init(0);
    if (pthread_create(&thread1, NULL, userInt, NULL) || pthread_create(&thread2, NULL, bildLife,NULL))
		errorMain("i can't create thread");
    if (pthread_join(thread1, NULL) || pthread_join(thread2, NULL))
		errorMain("i can't join thread");
	freeMatr(2, count-1);
    return EXIT_SUCCESS;
}

