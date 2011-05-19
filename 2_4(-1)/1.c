#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/poll.h>

const char *printfErrorMessage = "some trouble with printf";
int *endStream, ret, count=0;
char buf[128];
struct pollfd fds[100];

void closeAndExit()
{
	free(endStream);
	exit(EXIT_FAILURE);
}
	
void tryRead()
{
	int i;
	ret = poll(fds, count, -1);
	if(ret == -1)
	{
		if(fprintf(stderr, "%s\n","some trouble with poll")==-1)
		{
			perror(printfErrorMessage);
		}
		closeAndExit();
	}
	for(i=0; i<count; i++)
	{
		if(!endStream[i])
		{
			if(fds[i].revents & POLLNVAL)
			{
				endStream[i]=1;
				count--;
			}
			else if(fds[i].revents & POLLOUT)
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
					count--;
				}
				else
				{
					write(1, buf, result);
				}	
			}
			else
			{
				if(fprintf(stdout, "%s %d\n","I can not read file", i)==-1)
				{
					perror(printfErrorMessage);
					closeAndExit();
				}
				endStream[i]=1;
				count--;
			}
		}
	}
}

int main(int argc,char *argv[])
{
	int i;
	int file;
	endStream = malloc((argc-1)*sizeof(int));
	for(i=0; i<argc; i++)
	{
		endStream[i]=0;
	}
	
	for(i=1; i<argc; i++)
	{
		if(sscanf(argv[i], "%d", &file))
		{
			fds[count].fd = file;
			fds[count++].events = POLLOUT;
		}
	}
	while(count)
	{
		//fprintf(stdout, "%s\n","I can not read file");
		tryRead();
	}
}