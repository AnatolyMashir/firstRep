#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

const char *printfErrorMessage = "some trouble with printf";

volatile int flag = 1;

void sigHandler(int number, siginfo_t *info, void *something)
{
	printf("Hello, signal %d!\n",info->si_signo);
	if(number==20)
		flag=0;
}

int main()
{
	struct sigaction S;
	int i;
	
	S.sa_sigaction = sigHandler;
	S.sa_flags = SA_SIGINFO;
	if(sigfillset(&S.sa_mask))
	{
		if(fprintf(stderr, "%s\n","some trouble with mask")==-1)
		{
			perror(printfErrorMessage);
			return EXIT_FAILURE;
		}
	}

	for(i=1; i<32; ++i)
		if(i!=9 && i!=19 && sigaction(i, &S, NULL))
		{
			if(fprintf(stderr, "%s\n","can't set signal")==-1)
			{
				perror(printfErrorMessage);
				return EXIT_FAILURE;
			}
		}
	
	while(flag);
	return EXIT_SUCCESS;
}

