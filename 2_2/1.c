#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

volatile int flag = 1;

void handler()
{
	printf("I catch ctrl+c\n");
	signal (SIGINT, &handler);
}

void handler2()
{
	signal (SIGINT, SIG_DFL);
	flag = 0;
}

int main(argc, argv)
int argc;
char *argv[];
{
	signal (SIGINT, &handler);
	signal (20, &handler2);
	while(flag);
	return 0;
}
