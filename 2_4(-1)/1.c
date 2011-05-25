#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/poll.h>

#define TIMEOUT 1

const char *printfErrorMessage = "some trouble with printf";
int *endStream, *numbers, ret, count=0, countLive;
char buf[128];
struct pollfd *fds;

void closeAndExit()
{
	free(endStream);
	free(fds);
	exit(EXIT_FAILURE);
}
	
void tryRead()
{
	int i;
	ret = poll(fds, count, TIMEOUT);
	if(ret == -1)
	{
		if(fprintf(stderr, "%s\n","some trouble with poll")==-1)
		{
			perror(printfErrorMessage);
		}
		closeAndExit();
	}
	if(ret != 0)
	{
		for(i=0; i<count; i++)
		{
			if(!endStream[i])
			{
				if((fds[i].revents & POLLNVAL) || (fds[i].revents & POLLHUP) || (fds[i].revents & POLLERR))
				{
					endStream[i]=1;
					countLive--;
				}
				else if(fds[i].revents & POLLIN)
				{
					int result = read(fds[i].fd, buf, 20);
					if(result==-1)
					{
						if(fprintf(stdout, "%s %d\n","I can not read file", i)==-1)
						{
							perror(printfErrorMessage);
							closeAndExit();
						}
						endStream[i]=1;
						countLive--;
					}
					else if(result == 0)
					{
						endStream[i]=1;
						countLive--;
						//fprintf(stdout, "i close %d countLive %d\n", i, countLive);
						
					}
					else
					{
						write(1, buf, result);
					}	
				}/*
				else
				{
					if(fprintf(stdout, "%s %d\n","I can not read file", i)==-1)
					{
						perror(printfErrorMessage);
						closeAndExit();
					}
					endStream[i]=1;
					count--;
				}*/
			}
		}
	}
}

int main(int argc,char *argv[])
{
	int i;
	int file;
	endStream = malloc((argc-1)*sizeof(int));
	fds = (struct pollfd *)malloc((argc-1)*sizeof(struct pollfd*));
	if(endStream==NULL || fds==NULL)
	{
		puts("Error: not enough memory");
		return 2;
	}

	for(i=0; i<argc; i++)
	{
		endStream[i]=0;
	}
	
	for(i=1; i<argc; i++)
	{
		if(sscanf(argv[i], "%d", &file))
		{
			fds[count].fd = file;
			fds[count++].events = POLLIN | POLLNVAL | POLLERR | POLLHUP;
		}
	}
	countLive = count;
	while(countLive)
	{
		//fprintf(stdout, "%s\n","I can not read file");
		tryRead();
	}
}