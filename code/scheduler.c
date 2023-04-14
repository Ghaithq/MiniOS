#include "headers.h"

int PG_S_shmid;                   // shared memory id between process generator and scheduler
struct processData *PG_S_shmaddr; // address of PG_S_shmid
int prevID = -1;                  // used to check if memory changed
struct PCB runningProcess;
int PID;
int PG_S_msgqid;
FILE *outputFile;
struct floatQueue WTAqueue;
float WTASum=0;
float waitingSum = 0;

void childHandler()
{
    printf("PID of child:%d\n", runningProcess.PID);
    int status;
    int TA=getClk()-runningProcess.arrivalTime;
    float value = ((float)(TA))/runningProcess.executionTime;
    float WTA = ((int)(value * 100 + .5) / 100.0);
    WTASum+=WTA;
    waitingSum+=runningProcess.waitingTime;
    floatEnqueue(&WTAqueue,WTA);
    fprintf(outputFile,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",getClk(),runningProcess.id,runningProcess.arrivalTime,runningProcess.executionTime,0,runningProcess.waitingTime,TA,WTA);
    runningProcess.id = -1;
    wait(&status);
}

void exitHandler()
{

    int count=WTAqueue.count;
    floatPrint(&WTAqueue);
    float avgWTA=WTASum/count;
    float avgWaiting=waitingSum/count;
    float stdWTA=0;
    for(int i=0;i<count;i++)
    {
        float x=*floatDequeue(&WTAqueue);
        stdWTA+=((x-avgWTA)*(x-avgWTA));
    }
    stdWTA/=(count-1);
    stdWTA=sqrt((double)stdWTA);
    FILE* averageFile=fopen("scheduler.perf","w");
    fprintf(averageFile,"CPU utilization = 100%% \n");
    fprintf(averageFile,"Avg WTA = %f\n",avgWTA);
    fprintf(averageFile,"Avg Waiting = %f\n",avgWaiting);
    fprintf(averageFile,"Std WTA == %f\n",stdWTA);
    
    

    fclose(averageFile);
    fclose(outputFile);
    destroyClk(true);
    raise(SIGKILL);
}


struct msgBuffDummy
{
    char x[1];
    int mtype;
};

void HPF()
{
    struct PriorityQueue processesPQ;
    InitPriorityQueue(&processesPQ);
    struct PCB newProcess;
    runningProcess.id = -1;
    double turnaroundSum = 0;
    int sumProcesses = 0;
    //(*PG_S_shmaddr).id = -1;
    // prevID=-1;
    struct msgBuffDummy dummy;
    dummy.mtype=0;
    dummy.x[1]='c';

    struct processData tempPD; 
    while (1)
    {
        // scheduling incoming new processes
        int waitingTime = 0;
        while ((*PG_S_shmaddr).id != prevID)
        {
            tempPD = *PG_S_shmaddr;
            if (-1 == msgsnd(PG_S_msgqid, &dummy, sizeof(dummy), IPC_NOWAIT))
                printf("error happened in recv\n");
            printf("arrived ID=%d, running time=%d\n", tempPD.id, tempPD.runningtime);
            prevID = tempPD.id;
            newProcess.priority = tempPD.priority;
            newProcess.remaingTime = tempPD.runningtime;
            newProcess.id = tempPD.id;
            newProcess.state = 'W';
            newProcess.arrivalTime = tempPD.arrivaltime;
            newProcess.arrivalTime;
            newProcess.executionTime=tempPD.runningtime;
            priorityEnqueue(&processesPQ, newProcess, newProcess.priority);
            usleep(1);
        }

        if (runningProcess.id == -1)
        {
            struct PCB *tempProcess = priorityDequeue(&processesPQ);

            if (tempProcess)
            {
                printf("priority of running process: %d\n", tempProcess->priority);
                runningProcess = *tempProcess;
                char runTime = runningProcess.remaingTime;
                char *runtimeAddress = &runTime;
                int PID = fork();
                if (PID == 0)
                {
                    printf("lmaoooooooooo\n");
                    int check1 = execl("./process", runtimeAddress, NULL);
                    if (check1 == -1)
                        printf("unsuccessful execv with error%d\n", errno);
                    printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
                }
                runningProcess.PID = PID;
                runningProcess.state = 'R';
                runningProcess.arrivalTime;
                runningProcess.waitingTime = getClk() - runningProcess.arrivalTime;
                //waitingSum += runningProcess.waitingTime;
                fprintf(outputFile,"At time %d process %d started arr %d total %d remain %d wait %d\n",getClk(),runningProcess.id,runningProcess.arrivalTime,runningProcess.executionTime,runningProcess.remaingTime,runningProcess.waitingTime);
            }
        }
    }
}

void SRTN()
{
    struct PriorityQueue Processes;
    InitPriorityQueue(&Processes);
    int prevClk = getClk();
    runningProcess.id = -1;
    prevID = -1;
    struct PCB newProcess;
    struct msgBuffDummy dummy;
    dummy.mtype = 0;
    dummy.x[1] = 'c';
    struct processData tempPD;
    while (1)
    {

        
        if(prevClk<getClk() && runningProcess.id!=-1)
        {
            runningProcess.remaingTime--;
            prevClk=getClk();
        }

        while ((*PG_S_shmaddr).id != prevID)
        { //////////a lock must be put on the shared memory because if more than one process arrived at the same time
            tempPD = *PG_S_shmaddr;
            if (-1 == msgsnd(PG_S_msgqid, &dummy, sizeof(dummy), IPC_NOWAIT))
                printf("error happened in recv\n");
            printf("arrived ID=%d, running time=%d\n", (*PG_S_shmaddr).id, (*PG_S_shmaddr).runningtime);
            struct PCB newProcess;
            newProcess.arrivalTime = tempPD.arrivaltime;
            newProcess.id = tempPD.id;
            newProcess.priority = tempPD.priority;
            newProcess.remaingTime = tempPD.runningtime;
            newProcess.executionTime=tempPD.runningtime;
            newProcess.state='W';
            priorityEnqueue(&Processes,newProcess,newProcess.remaingTime);
            prevID = newProcess.id;

            usleep(1);
        }

        if (runningProcess.id == -1)
        {
            // printf("-----------------------------------------------------\n");
            struct PCB *processPtr = priorityDequeue(&Processes);
            if (processPtr)
            {

                runningProcess = *processPtr;
                if (runningProcess.state == 'W')
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
                    fprintf(outputFile,"At time %d process %d started arr %d total %d remain %d wait %d\n",getClk(),runningProcess.id,runningProcess.arrivalTime,runningProcess.executionTime,runningProcess.remaingTime,runningProcess.waitingTime);

                }
                else if (runningProcess.state == 'S')
                {
                    //while(prevClk==getClk());
                    runningProcess.state='R';
                    kill(runningProcess.PID,SIGCONT);
                    runningProcess.waitingTime+=getClk()-runningProcess.lastStopped;
                    printf("resumed process id:%d , PID:%d\n",runningProcess.id,runningProcess.PID);
                    fprintf(outputFile,"At time %d process %d resumed arr %d total %d remain %d wait %d\n",getClk(),runningProcess.id,runningProcess.arrivalTime,runningProcess.executionTime,runningProcess.remaingTime,runningProcess.waitingTime);

                }
            }
        }

        if (runningProcess.id != -1 && Processes.Head && Processes.Head->process.remaingTime < runningProcess.remaingTime)
        {
            fprintf(outputFile,"At time %d process %d stopped arr %d total %d remain %d wait %d\n",getClk(),runningProcess.id,runningProcess.arrivalTime,runningProcess.executionTime,runningProcess.remaingTime,runningProcess.waitingTime);

            printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
            runningProcess.state='S';
            runningProcess.lastStopped=getClk();
            kill(runningProcess.PID,SIGSTOP);
            priorityEnqueue(&Processes,runningProcess,runningProcess.remaingTime);
            printf("paused process id:%d , PID:%d\n",runningProcess.id,runningProcess.PID);
            runningProcess.id=-1;

        }
        

    }
}

void RR()
{
    struct Queue RR_Queue = {NULL, NULL, 0};
    int RR_TimeSlice = 4;
    struct processData currPD;
    struct msgBuffDummy _buffer = {'c', 0};
    struct PCB _pcb;
    int stat_loc;
    int _pid;
    int _prevCLK;
    runningProcess.id = -1;
    int clk;
    while (1)
    {
        // Adding a process to the round robin queue

        if ((*PG_S_shmaddr).id != prevID)
        {
            currPD = *PG_S_shmaddr;
            if(msgsnd(PG_S_msgqid, &_buffer, sizeof(_buffer), !IPC_NOWAIT)==-1)
             printf("error happened in recv\n");
            _pcb.priority = currPD.priority;
            _pcb.id = currPD.id;
            _pcb.remaingTime = currPD.runningtime;
            _pcb.state = 'W';
            _pcb.arrivalTime = currPD.arrivaltime;
            printf("Arrival time: %d\n", currPD.arrivaltime);
            enqueue(&RR_Queue, _pcb);
            prevID = currPD.id;
        }
        if (RR_Queue.count != 0)
        {
            if (runningProcess.id == -1)
            {

                runningProcess = *dequeue(&RR_Queue);
                if (runningProcess.state == 'W')
                {

                    runningProcess.executionTime = getClk();
                    runningProcess.waitingTime = getClk() - runningProcess.arrivalTime;
                    _prevCLK = getClk();
                    int PID = fork();
                    if (PID == 0)
                    {
                        int check1 = execl("./process", (char *)&runningProcess.remaingTime, NULL);
                        if (check1 == -1)
                            printf("Unsuccessful execl with error%d\n", errno);
                    }
                    runningProcess.PID = PID;
                    runningProcess.state = 'R';
                    printf(" a process started ID= %d , PID = %d , \n", runningProcess.id, runningProcess.PID);
                }
                else if (runningProcess.state == 'S')
                {
                    runningProcess.state = 'R';
                    kill(runningProcess.PID, SIGCONT);
                    printf("resumed process id:%d , PID:%d\n", runningProcess.id, runningProcess.PID);
                }
            }
            else
            {
                clk = getClk();
                if (clk - _prevCLK == RR_TimeSlice)
                {
                    runningProcess.state = 'S';
                    kill(runningProcess.PID, SIGSTOP);
                    enqueue(&RR_Queue, runningProcess);
                    runningProcess.id = -1;
                    printf("\nSwitching process..........\n");
                    _prevCLK = clk;
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    // initClk();
    floatQueueInit(&WTAqueue);
    signal(SIGUSR1, childHandler);
    signal(SIGINT,exitHandler);
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

    key_t key_id = ftok("keyfile", MSGKEY_PG_S);
    PG_S_msgqid = msgget(key_id, 0666 | IPC_CREAT);

    key_t key_id=ftok("keyfile",MSGKEY_PG_S);
    PG_S_msgqid=msgget(key_id,0666 | IPC_CREAT);

    outputFile=fopen("scheduler.log","w");
    if(outputFile==NULL)
        printf("NULL\n");
    //fprintf(outputFile,"kdksjvbdkjfbvkdfjbvkdfba.jdfnv,dskfjvbs,djhfvb\n");

   // HPF();

    if (*argv[0] == '1')
        HPF();
    else if (*argv[0] == '2')
        SRTN();
    fclose(outputFile);
    destroyClk(true);
}
