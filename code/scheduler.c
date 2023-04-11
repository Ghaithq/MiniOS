#include "headers.h"

int PG_S_shmid;//shared memory id between process generator and scheduler
struct processData* PG_S_shmaddr;//address of PG_S_shmid
int prevID=-1;//used to check if memory changed
struct PCB runningProcess;
int PID;
void childHandler()
{
    runningProcess.id=-1;
    printf("PID of child:%d\n",runningProcess.PID);
    int status;
    wait(&status);
    
}


void HPF()
{


    //the problem is starting scheduler whilst the queue is empty
    struct PriorityQueue processesPQ; 
    InitPriorityQueue(&processesPQ);
    struct PCB newProcess;
    runningProcess.id=-1;
    double waitingSum=0;
    double turnaroundSum=0;
    int sumProcesses=0;
    (*PG_S_shmaddr).id=-1;
    while(1){
        //scheduling incoming new processes

        int waitingTime=0;

        //data for running process
        //struct PCB arr[3];
        
        //printf("prevID=%d, *PG_S_shmaddr).id=%d\n",prevID,(*PG_S_shmaddr).id);
        //initClk();
        if((*PG_S_shmaddr).id!=prevID)
        {
            printf("arrived ID=%d, running time=%d\n",(*PG_S_shmaddr).id,(*PG_S_shmaddr).runningtime);
            prevID=(*PG_S_shmaddr).id;
            newProcess.priority=(*PG_S_shmaddr).priority;
            newProcess.remaingTime=(*PG_S_shmaddr).runningtime;
            newProcess.id=(*PG_S_shmaddr).id;
            newProcess.state='W';
            newProcess.arrivalTime=(*PG_S_shmaddr).arrivaltime;
            newProcess.arrivalTime;
            priorityEnqueue(&processesPQ,newProcess,newProcess.priority);
            //priorityPrint(&processesPQ);
            
        }
        //printf("runningProcessID=%d\n",runningProcess.id);
        if(runningProcess.id==-1)
        {
            printf("i am count of PQ:%d\n",processesPQ.count);
            struct PCB* tempProcess=priorityDequeue(&processesPQ);

            if(tempProcess)
            {
                printf("temp is not null\n");
                runningProcess=*tempProcess;
                char runTime=runningProcess.remaingTime;
                char* runtimeAddress=&runTime;
                int PID=fork();
                printf("%d\n",runningProcess.remaingTime);
                if(PID==0)
                {
                    char* cmd[2]={runtimeAddress,NULL};
                    printf("created fork \n");
                    if((int)*runtimeAddress!=runningProcess.remaingTime)
                        printf("qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq\n");
                    int check1=execv("./process",&runtimeAddress);
                    if(check1==-1)
                        printf("unsuccessful execv with error%d\n",errno);
                    printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
                }
                printf("I am PID:%d\n",PID);
                runningProcess.PID=PID;
                runningProcess.state='R';
                runningProcess.arrivalTime;                
                runningProcess.waitingTime=getClk()-runningProcess.arrivalTime;
                waitingSum+=runningProcess.waitingTime;
            }
        }
    }
}

int main(int argc, char * argv[])
{
    //initClk();
    signal(SIGCHLD,childHandler);
    printf("%c\n",*argv[0]);

    //------------------Creating a shm with Scheduler------------------//
    PG_S_shmid=shmget(SHKEY_PG_S,sizeof(struct processData),IPC_CREAT | 0666);
    if ((long)PG_S_shmid == -1)
    {
        perror("Error in creating shm between process generator and sched!\n");
        exit(-1);
    }
    PG_S_shmaddr = shmat(PG_S_shmid, (void *)0, 0);
    (*PG_S_shmaddr).id=-1;
    if ((long)PG_S_shmaddr == -1)
    {
        perror("Error in attaching the shm  between process generator and sched!\n");
        exit(-1);
    }
    initClk();
    prevID=-1;
    // while(1)
    // {
    //     if(prevID!=(*PG_S_shmaddr).id)
    //     {
    //         printf("arrived ID=%d, running time=%d\n",(*PG_S_shmaddr).id,(*PG_S_shmaddr).runningtime);
    //         prevID=(*PG_S_shmaddr).id;
    //     }
    // }
    //if(*argv[1]=='1')
        HPF();
    
    //upon termination release the clock resources.
    //while(1);

    destroyClk(true);
}

