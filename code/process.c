#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char * argv[])
{
    initClk();
    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    int prevClk=getClk();
    remainingtime=(int)(*argv[0]);
    printf("started process\n");
    while (remainingtime > 0 )
    {
        // remainingtime = ??;
        if(prevClk<getClk())
        {
            printf("PID=%d ,remaining Time=%d\n",getpid(),remainingtime);
            remainingtime--;
            prevClk=getClk();
        }
    }
    printf(" PID=%d finished\n",getpid());
    kill(getppid(),SIGUSR1);
    printf("Parent PID=%d\n",getppid());
    return 0;
}
