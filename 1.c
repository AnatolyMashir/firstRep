#include <stdio.h>
#include <errno.h>

int main(){
    const char *mess = "my first exeption in c";
    errno = 0;
    printf("Hello, world!\r\n");
    if(errno !=0)
    {
	perror(mess);
    }
    return 1;
}
