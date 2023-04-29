#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
int prevClk;
int clk;
bool stop;
int last;
void ContHandler()
{
    prevClk=getClk();
}
void StopHandler()
{
    stop=true;
    last=getClk();
    fflush(stdout);
}
int main(int agrc, char * argv[])
{
    signal(SIGUSR2,StopHandler);
    signal(SIGCONT,ContHandler);
    initClk();

    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
     prevClk=getClk();
    remainingtime=(int)(*argv[0]);
    printf("started process\n");
    while (remainingtime > 0 )
    {
        clk=getClk();
        
        if(clk>prevClk){
            remainingtime--;
            printf("PID=%d ,remaining Time=%d, current clock=%d, prevClk=%d\n",getpid(),remainingtime,clk,prevClk);
             prevClk=getClk();
        }
        if(stop && remainingtime>0 && last==prevClk)
        {
            prevClk=getClk();
            stop=false;
            kill(getpid(),SIGSTOP);
        }

    }
    printf(" PID=%d finished at time=%d\n",getpid(),getClk());
    kill(getppid(),SIGUSR1);
    printf("Parent PID=%d\n",getppid());
    return 0;
}

