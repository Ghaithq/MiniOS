#include "headers.h"

int PG_S_shmid;                   // shared memory id between process generator and scheduler
struct processData *PG_S_shmaddr; // address of PG_S_shmid
int prevID = -1;                  // used to check if memory changed
struct PCB runningProcess;
int PID;
void childHandler()
{
    runningProcess.id = -1;
    printf("PID of child:%d\n", runningProcess.PID);
    int status;
    wait(&status);
}

void HPF()
{
    struct PriorityQueue processesPQ;
    InitPriorityQueue(&processesPQ);
    struct PCB newProcess;
    runningProcess.id = -1;
    double waitingSum = 0;
    double turnaroundSum = 0;
    int sumProcesses = 0;
    //(*PG_S_shmaddr).id = -1;
    //prevID=-1;
    while (1)
    {
        // scheduling incoming new processes

        int waitingTime = 0;

        while ((*PG_S_shmaddr).id != prevID)
        {
            printf("arrived ID=%d, running time=%d\n", (*PG_S_shmaddr).id, (*PG_S_shmaddr).runningtime);
            prevID = (*PG_S_shmaddr).id;
            newProcess.priority = (*PG_S_shmaddr).priority;
            newProcess.remaingTime = (*PG_S_shmaddr).runningtime;
            newProcess.id = (*PG_S_shmaddr).id;
            newProcess.state = 'W';
            newProcess.arrivalTime = (*PG_S_shmaddr).arrivaltime;
            newProcess.arrivalTime;
            priorityEnqueue(&processesPQ, newProcess, newProcess.priority);
        }

        if (runningProcess.id == -1)
        {
            struct PCB *tempProcess = priorityDequeue(&processesPQ);

            if (tempProcess)
            {
                printf("priority of running process: %d\n",tempProcess->priority);
                runningProcess = *tempProcess;
                char runTime = runningProcess.remaingTime;
                char *runtimeAddress = &runTime;
                int PID = fork();
                if (PID == 0)
                {
                    int check1 = execl("./process", runtimeAddress, NULL);
                    if (check1 == -1)
                        printf("unsuccessful execv with error%d\n", errno);
                    printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
                }
                runningProcess.PID = PID;
                runningProcess.state = 'R';
                runningProcess.arrivalTime;
                runningProcess.waitingTime = getClk() - runningProcess.arrivalTime;
                waitingSum += runningProcess.waitingTime;
            }
        }
    }
}

void SRTN()
{
    struct PriorityQueue Processes;
    InitPriorityQueue(&Processes);
    int prevClk=getClk();
    runningProcess.id = -1;
    prevID = -1;
    struct PCB newProcess;
    while (1)
    {

        //  if(prevClk<getClk() && runningProcess.id!=-1)
        // {
        //     runningProcess.remaingTime--;
        //     prevClk=getClk();
        // }
        
        while ((*PG_S_shmaddr).id != prevID)
        {//////////a lock must be put on the shared memory because if more than one process arrived at the same time
            printf("arrived ID=%d, running time=%d\n", (*PG_S_shmaddr).id, (*PG_S_shmaddr).runningtime);
            struct PCB newProcess;
            newProcess.arrivalTime = (*PG_S_shmaddr).arrivaltime;
            newProcess.id = (*PG_S_shmaddr).id;
            newProcess.priority = (*PG_S_shmaddr).priority;
            newProcess.remaingTime = (*PG_S_shmaddr).runningtime;

            newProcess.state='W';
            priorityEnqueue(&Processes,newProcess,newProcess.remaingTime);
            prevID = newProcess.id;


            
        }


        if(runningProcess.id==-1)
        {
            //printf("-----------------------------------------------------\n");
            struct PCB* processPtr = priorityDequeue(&Processes);
            if(processPtr)
            {

                runningProcess = *processPtr;
                if(runningProcess.state=='W')
                {
                    runningProcess.waitingTime = getClk() - runningProcess.arrivalTime;
                    char runTime = runningProcess.remaingTime;
                    char *runtimeAddress = &runTime;
                    int PID = fork();
                    if (PID == 0)
                    {
                            int check1 = execl("./process", runtimeAddress, NULL);
                            if (check1 == -1)
                                printf("unsuccessful execv with error%d\n", errno);
                        
                    }
                    runningProcess.PID = PID;
                    runningProcess.state = 'R';
                    printf(" a process started ID= %d , PID = %d , \n", runningProcess.id, runningProcess.PID);
                }
                else if(runningProcess.state=='S')
                {
                    runningProcess.state='R';
                    kill(runningProcess.PID,SIGCONT);
                    printf("resumed process id:%d , PID:%d\n",runningProcess.id,runningProcess.PID);
                }
            }
        }

        if(runningProcess.id!=-1&&Processes.Head&&Processes.Head->process.remaingTime<runningProcess.remaingTime)
        {
            printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
            runningProcess.state='S';
            kill(runningProcess.PID,SIGSTOP);
            priorityEnqueue(&Processes,runningProcess,runningProcess.remaingTime);
            printf("paused process id:%d , PID:%d\n",runningProcess.id,runningProcess.PID);
            runningProcess.id=-1;

        }

    }
    
}


int main(int argc, char *argv[])
{
    // initClk();
    signal(SIGUSR1, childHandler);
    printf("%c\n", *argv[0]);

    //------------------Creating a shm with Scheduler------------------//
    PG_S_shmid = shmget(SHKEY_PG_S, sizeof(struct processData), IPC_CREAT | 0666);
    if ((long)PG_S_shmid == -1)
    {
        perror("Error in creating shm between process generator and sched!\n");
        exit(-1);
    }
    PG_S_shmaddr = shmat(PG_S_shmid, (void *)0, 0);
    if ((long)PG_S_shmaddr == -1)
    {
        perror("Error in attaching the shm  between process generator and sched!\n");
        exit(-1);
    }
    initClk();
    prevID = -1;

    //HPF();
    if (*argv[0] == '1')
        HPF();
    else if(*argv[0] == '2')
        SRTN();
    destroyClk(true);
}
