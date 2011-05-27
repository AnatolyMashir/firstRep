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

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
const char *printfErrorMessage = "some trouble with printf";
const char *getAffinityMessage = "some trouble with get affinity";
int ***matrs;
volatile char lock=none;
volatile int count;
int current=0, next=1;
char key;
char bad = 'e';

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
static void *userInt(void *args)
{
	//initscr();
	int i, j;
	key = mygetch();
	//fprintf(stdout, "%s %c\n","user read ", key);
	while(key!='q')
	{
		if(pthread_mutex_lock(&mutex)) // почему лок тут и вообще зачем см. ниже
		{
			if(fprintf (stderr, "%s\n","i can't lock mutex")==-1)
			{
				perror(printfErrorMessage);
			}
			lock=quit;
			pthread_exit(&bad);
		}
		lock=ask;
		if(pthread_mutex_unlock(&mutex))
		{
			if(fprintf (stderr, "%s\n","i can't unlock mutex")==-1)
			{
				perror(printfErrorMessage);
			}
			lock=quit;
			pthread_exit(&bad);
		}
		while(lock==ask);
		//fprintf(stdout, "%s\n","i this");
		for(i=0;  i<count; i++)
		{
			for(j=0;  j<count; j++)
			{
				if(printf("%c", matrs[lock][i][j])==-1)
				{
					perror(printfErrorMessage);
					lock=quit;
				}
			}
			if(printf("\n")==-1)
			{
				perror(printfErrorMessage);
				lock=quit;
			}
		}
		if(printf("\n")==-1)
		{
			perror(printfErrorMessage);
			lock=quit;
		}	
		lock=none;
		key = mygetch();
	}
	lock=quit;
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
	if(c<3 || c>6)
	{
		return ' ';
	}
	return '*';
}

int reBild(int current, int next)
{
	int i, j;
	for(i=0;  i<count; i++)
	{
		for(j=0;  j<count; j++)
		{
			matrs[next][i][j]=reCount(i,j,current);
		}
	}
	return 0;
}

void handler()
{
	fprintf(stdout, "%s\n","recount");
	if(next==lock)
	{
		next=(next+1)%3;
	}
	reBild(current, next);
	alarm(1);
	signal (SIGALRM, &handler);
	current=next;
}

void *bildLife(void *args)
{	
	handler();
	while(lock!=quit)
	{
		if(pthread_mutex_lock(&mutex))		// о том почему сдесь стоит мьютекс, вообще он тут не нужен т.к. потоки не конкурентные
		{									//когда 1 пишет 2ой всегда читает, причем юзер ставитс€ в активное ожидание - да € в 
			if(fprintf (stderr, "%s\n","i can't lock mutex")==-1)
			{								// курсе что вы этого не любите :-) а лок юзаетс€ дл€ принудительного обновлени€ кэша
				perror(printfErrorMessage);	//(если вдруг, а скорее всего так оно и есть, переменна€ ушла в кэш процессора)	
			}								//в данной задаче этот лок ни на что не вли€ет, кроме возможного времени отклика клиента
			key='q';						//вообще этот лок по€вилс€ как результат опыта написани€ многопользовательской жизни
			lock=ask;
			pthread_exit(&bad);
		}							
		if(lock==ask)				
		{							
			lock=current;			
		}							
		if(pthread_mutex_unlock(&mutex))  
		{
			if(fprintf (stderr, "%s\n","i can't unlock mutex")==-1)
			{
				perror(printfErrorMessage);
			}
			key='q';
			lock=ask;
			pthread_exit(&bad);
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

int main(int argc,char *argv[])
{
	//time_t t1 = time(NULL);
	pthread_t thread1, thread2;
	cpu_set_t cpu1, cpu2;
	if(argc!=2)
	{
		if(fprintf (stderr, "%s\n","need count")==-1)
		{
			perror(printfErrorMessage);
		}
		return EXIT_FAILURE;
   	}
	if(!sscanf(argv[1], "%d", &count))
	{
		if(fprintf(stderr, "%s %s\n","i, can not cast", argv[1])==-1)
		{
			perror(printfErrorMessage);
		}
		return EXIT_FAILURE;
	}
	int i,j,k;
	matrs = (int ***)malloc(3*sizeof(int **));
	if(matrs==NULL)
	{
		if(fprintf (stderr, "%s\n","no enough memory")==-1)
		{
			perror(printfErrorMessage);
		}
		return EXIT_FAILURE;
	}
	for(i=0; i<3; i++)
	{
		matrs[i] = (int **)malloc(count*sizeof(int *));
		if(matrs[i]==NULL)
		{
			if(fprintf (stderr, "%s\n","no enough memory")==-1)
			{
				perror(printfErrorMessage);
			}
			return EXIT_FAILURE;
		}	
		for(j=0; j<count; j++)
		{
			matrs[i][j]=(int *)malloc(count*sizeof(int));
			if(matrs[i][j]==NULL)
			{
				if(fprintf (stderr, "%s\n","no enough memory")==-1)
				{
					perror(printfErrorMessage);
				}
				return EXIT_FAILURE;
			}
		}
	}
	init(0);
    if (pthread_create(&thread1, NULL, userInt, NULL) || pthread_create(&thread2, NULL, bildLife,NULL))
    {
		if(fprintf (stderr, "%s\n","i can't create thread")==-1)
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
	for(i=0; i<3; i++)
	{
		for(j=0; j<count; j++)
		{
			free(matrs[i][j]);
		}
		free(matrs[i]);
	}
	free(matrs);
	if(pthread_mutex_destroy(&mutex))
	{
		if(fprintf (stderr, "%s\n","i can't destroy mytex")==-1)
		{
			perror(printfErrorMessage);
		}
		return EXIT_FAILURE;
	}
    return EXIT_SUCCESS;
}

