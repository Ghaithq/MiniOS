#include "headers.h"

// /* Modify this file as needed*/
// int remainingtime;
// int prevClk;
// int S_P_shmid;
// int* S_P_shmaddr;
// int prevRemainingTime;
// bool decremented;
// void sigcontHandler()
// {
//     prevClk=getClk();
// }

// void sigstopHandler()
// {
//     if(!decremented && remainingtime==prevRemainingTime)
//     {
//         printf("cccccccccccccccccccccccccccccc\n");
//         remainingtime--;
//     }
//     raise(SIGSTOP);

// }


// void    block_signal(int signal)
// {
// //    Set of signals to block
//     sigset_t    sigset;

// //    Initialize set to 0
//     sigemptyset(&sigset);
// //    Add the signal to the set
//     sigaddset(&sigset, signal);
// //    Add the signals in the set to the process' blocked signals
//     sigprocmask(SIG_BLOCK, &sigset, NULL);
//     if (signal == SIGQUIT)
//         printf("\e[36mSIGQUIT (ctrl-\) blocked.\e[0m\n");
// }
// // Unblocks the given signal
// void    unblock_signal(int signal)
// {
// //    Set of signals to unblock
//     sigset_t    sigset;

// //    Initialize the set to 0
//     sigemptyset(&sigset);
// //    Add the signal to the set
//     sigaddset(&sigset, signal);
// //    Remove set signals from the process' blocked signals
//     sigprocmask(SIG_UNBLOCK, &sigset, NULL);
//     if (signal == SIGQUIT)
//         printf("\e[36mSIGQUIT (ctrl-\) unblocked.\e[0m\n");
// }


// int main(int agrc, char * argv[])
// {
//     signal(SIGCONT,sigcontHandler);
//     signal(SIGUSR2,sigstopHandler);
//     initClk();
//     //////////////////////////
//     // S_P_shmid = shmget(SHKEY_S_P, sizeof(int), IPC_CREAT | 0666);
//     // if ((long)S_P_shmid == -1)
//     // {
//     //     perror("Error in creating shm between process and sched!\n");
//     //     exit(-1);
//     // }
//     // S_P_shmaddr = shmat(S_P_shmid, (void *)0, 0);
//     // if ((long)S_P_shmaddr == -1)
//     // {
//     //     perror("Error in attaching the shm  between process and sched!\n");
//     //     exit(-1);
//     // }
//     //////////////////////////
//     //TODO it needs to get the remaining time from somewhere
//     //remainingtime = ??;
//     prevClk=getClk();
//     remainingtime=(int)(*argv[0]);
//     prevRemainingTime=remainingtime;
//     printf("started process\n");

//     while (remainingtime > 0 )
//     {
//         prevClk=getClk();
//         while(prevClk==getClk());
//         //*S_P_shmaddr=0;
//         decremented=false;
//         if(prevClk==getClk()-1){
//             printf("PID=%d ,remaining Time=%d, current clock=%d, prevClk=%d\n",getpid(),remainingtime,getClk(),prevClk);
//             remainingtime--;
//             decremented=true;
//         }
//         prevRemainingTime=remainingtime;
//         //*S_P_shmaddr=1;
//         prevClk=getClk();
//         printf("-------------------------------------------------------------\n");
//         //usleep(1);

//     }
//     printf(" PID=%d finished at time=%d\n",getpid(),getClk());
//     kill(getppid(),SIGUSR1);
//     printf("Parent PID=%d\n",getppid());
//     return 0;
// }



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
    printf(" PID=%d finished\n",getpid());
    kill(getppid(),SIGUSR1);
    printf("Parent PID=%d\n",getppid());
    return 0;
}








