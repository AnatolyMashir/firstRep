#include <stdio.h>
#include <errno.h>

int main(){
    const char *mess = "some trouble with printf";
    if(printf("Hello, world!\r\n")==-1)
    {
	perror(mess);
	return 0;
    }
    return 1;
}
