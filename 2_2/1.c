#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

const char *printfErrorMessage = "some trouble with printf";

int pipefd[2];

void sigHandler(int number, siginfo_t *info, void *something)
{
	write(pipefd[1], &number, sizeof(number));
	if(number==20)
		 close(pipefd[1]);
}

int main()
{
	struct sigaction S;
	int i, number;
	
	if (pipe(pipefd) == -1) 
	{
		perror("pipe fail");
        return EXIT_FAILURE;
    }

	S.sa_sigaction = sigHandler;
	S.sa_flags = SA_SIGINFO;
	if(sigfillset(&S.sa_mask))
	{
		if(fprintf(stderr, "%s\n","some trouble with mask")==-1)
		{
			perror(printfErrorMessage);
		}
		return EXIT_FAILURE;
	}

	for(i=1; i<32; ++i)
		if(i!=9 && i!=19 && sigaction(i, &S, NULL))
		{
			if(fprintf(stderr, "%s\n","can't set signal")==-1)
			{
				perror(printfErrorMessage);
			}
			return EXIT_FAILURE;
		}

	do
	{
		if(read(pipefd[0], &number, sizeof(number)) > 0)
			printf("Hello, signal %d! pid=%d, gid=%d\n", number, getpid(), getgid());
	}
	while(number != 20);
	return 0;
}