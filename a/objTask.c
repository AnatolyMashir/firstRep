#include <stdio.h>
#include <stdlib.h>

int matrix[2000][2000];
int main(int argc,char *argv)
{
	int i,j;
	for(i=0; i<2000; i++)
		for(j=0; j<2000; j++)
			matrix[i][j] = i+j;
	return 0;
}



