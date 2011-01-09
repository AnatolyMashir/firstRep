#include <stdio.h>
extern int sum(int *, int *);

int main()
{
int a=20,b=30;
printf("%d\n", sum(&a,&b));
return 0;
}
