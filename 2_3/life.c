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
pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER; 
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

void errorMain(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void errorUser(const char *msg)
{
    perror(msg);
    lock=quit;
	pthread_exit(&bad);
}

void errorServer(const char *msg)
{
    perror(msg);
	lock=error;	
	signal (SIGALRM, SIG_DFL); // чтобы ваша душа была спокойна
	pthread_exit(&bad);
}

static void *userInt(void *args)
{
	int i, j;
	key = mygetch();
	while(key!='q')
	{
		if(pthread_mutex_lock(&mutex)) // почему лок тут и вообще зачем см. ниже
			errorUser("i can't lock mutex");
		lock=ask;
		if(pthread_mutex_unlock(&mutex))
			errorUser("i can't unlock mutex");
		while(lock==ask);
		if(lock<3)
		{
			for(i=0;  i<count; i++)
			{
				for(j=0;  j<count; j++)
				{
					if(printf("%c", matrs[lock][i][j])==-1)
						errorUser(printfErrorMessage);
				}
				if(printf("\n")==-1)
					errorUser(printfErrorMessage);
			}
			if(printf("\n")==-1)
				errorUser(printfErrorMessage);
			lock=none;
			key = mygetch();
		}
		else
		{
			key='q';
		}
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
	/*
	for(i=0;  i<count; i++)
	{
		for(j=0;  j<count; j++)
		{
			printf("%c", matrs[current][i][j]);
			
		}
		printf("\n");
	}
	printf("\n");*/
	for(i=0;  i<count; i++)
	{
		for(j=0;  j<count; j++)
		{
			matrs[next][i][j]=reCount(i,j,current);
		}
	}
}

void handler()
{
	fprintf(stdout, "%s\n","recount");
	if(pthread_mutex_lock(&mutex_lock))
		errorServer("i can't lock mutex");
	if(next==lock)
	{
		next=(next+1)%3;
	}	
	if(pthread_mutex_unlock(&mutex_lock))
		errorServer("i can't unlock mutex");
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
		if(pthread_mutex_lock(&mutex))
			errorServer("i can't lock mutex");
		/*о том почему сдесь стоит мьютекс, вообще он тут не нужен т.к. потоки не конкурентные
		когда 1 пишет 2ой всегда читает, причем юзер ставится в активное ожидание - да я в 
		курсе что вы этого не любите :-) а лок юзается для принудительного обновления кэша
		(если вдруг, а скорее всего так оно и есть, переменная ушла в кэш процессора)	
		в данной задаче этот лок ни на что не влияет, кроме возможного времени отклика клиента
		вообще этот лок появился как результат опыта написания многопользовательской жизни*/
		if(lock==ask)				
		{						
			if(pthread_mutex_lock(&mutex_lock))
				errorServer("i can't lock mutex");
			lock=current;			
			if(pthread_mutex_unlock(&mutex_lock))
				errorServer("i can't unlock mutex");
		}							
		if(pthread_mutex_unlock(&mutex))  
			errorServer("i can't unlock mutex");
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
	if(pthread_mutex_destroy(&mutex) || pthread_mutex_destroy(&mutex_lock))
		errorMain("i can't destroy mutex");
    return EXIT_SUCCESS;
}

