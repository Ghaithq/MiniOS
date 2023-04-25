// #include "headers.h"

// int PG_S_shmid;                   // shared memory id between process generator and scheduler
// struct processData *PG_S_shmaddr; // address of PG_S_shmid
// int prevID = -1;                  // used to check if memory changed
// struct PCB runningProcess;
// int PID;
// int PG_S_msgqid;
// FILE *outputFile;
// struct floatQueue WTAqueue;
// float WTASum=0;
// float waitingSum = 0;
// char algoType;
// int _prevCLK;

// void childHandler()
// {
//     printf("PID of child:%d\n", runningProcess.PID);
//     int status;
//     int TA=getClk()-runningProcess.arrivalTime;
//     float value = ((float)(TA))/runningProcess.executionTime;
//     float WTA = ((int)(value * 100 + .5) / 100.0);
//     WTASum+=WTA;
//     waitingSum+=runningProcess.waitingTime;
//     floatEnqueue(&WTAqueue,WTA);
//     runningProcess.id = -1;
//     // if(algoType=='3')
//     // {
//     //     _prevCLK=getClk();
//     // }
//     fprintf(outputFile,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",getClk(),runningProcess.id,runningProcess.arrivalTime,runningProcess.executionTime,0,runningProcess.waitingTime,TA,WTA);
//     wait(&status);
// }

// void exitHandler()
// {

//     int count=WTAqueue.count;
//     floatPrint(&WTAqueue);
//     float avgWTA=WTASum/count;
//     float avgWaiting=waitingSum/count;
//     float stdWTA=0;
//     for(int i=0;i<count;i++)
//     {
//         float x=*floatDequeue(&WTAqueue);
//         stdWTA+=((x-avgWTA)*(x-avgWTA));
//     }
//     stdWTA/=(count-1);
//     stdWTA=sqrt((double)stdWTA);
//     FILE* averageFile=fopen("scheduler.perf","w");
//     fprintf(averageFile,"CPU utilization = 100%% \n");
//     fprintf(averageFile,"Avg WTA = %f\n",avgWTA);
//     fprintf(averageFile,"Avg Waiting = %f\n",avgWaiting);
//     fprintf(averageFile,"Std WTA == %f\n",stdWTA);
    
    

//     fclose(averageFile);
//     fclose(outputFile);

//     exit(0);
// }


// struct msgBuffDummy
// {
//     char x[1];
//     int mtype;
// };

// void HPF()
// {
//     struct PriorityQueue processesPQ;
//     InitPriorityQueue(&processesPQ);
//     struct PCB newProcess;
//     runningProcess.id = -1;
//     double turnaroundSum = 0;
//     int sumProcesses = 0;
//     //(*PG_S_shmaddr).id = -1;
//     // prevID=-1;
//     struct msgBuffDummy dummy;
//     dummy.mtype=0;
//     dummy.x[1]='c';

//     struct processData tempPD; 
//     while (1)
//     {
//         // scheduling incoming new processes
//         int waitingTime = 0;
//         while ((*PG_S_shmaddr).id != prevID)
//         {
//             tempPD = *PG_S_shmaddr;
//             if (-1 == msgsnd(PG_S_msgqid, &dummy, sizeof(dummy), IPC_NOWAIT))
//                 printf("error happened in recv\n");
//             printf("arrived ID=%d, running time=%d\n", tempPD.id, tempPD.runningtime);
//             prevID = tempPD.id;
//             newProcess.priority = tempPD.priority;
//             newProcess.remaingTime = tempPD.runningtime;
//             newProcess.id = tempPD.id;
//             newProcess.state = 'W';
//             newProcess.arrivalTime = tempPD.arrivaltime;
//             newProcess.arrivalTime;
//             newProcess.executionTime=tempPD.runningtime;
//             priorityEnqueue(&processesPQ, newProcess, newProcess.priority);
//             usleep(1);
//         }

//         if (runningProcess.id == -1)
//         {
//             struct PCB *tempProcess = priorityDequeue(&processesPQ);

//             if (tempProcess)
//             {
//                 printf("priority of running process: %d\n", tempProcess->priority);
//                 runningProcess = *tempProcess;
//                 char runTime = runningProcess.remaingTime;
//                 char *runtimeAddress = &runTime;
//                 int PID = fork();
//                 if (PID == 0)
//                 {
//                     //printf("lmaoooooooooo\n");
//                     int check1 = execl("./process", runtimeAddress, NULL);
//                     if (check1 == -1)
//                         printf("unsuccessful execv with error%d\n", errno);
//                     printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
//                 }
//                 runningProcess.PID = PID;
//                 runningProcess.state = 'R';
//                 runningProcess.arrivalTime;
//                 runningProcess.waitingTime = getClk() - runningProcess.arrivalTime;
//                 //waitingSum += runningProcess.waitingTime;
//                 fprintf(outputFile,"At time %d process %d started arr %d total %d remain %d wait %d\n",getClk(),runningProcess.id,runningProcess.arrivalTime,runningProcess.executionTime,runningProcess.remaingTime,runningProcess.waitingTime);
//             }
//         }
//     }
// }

// void SRTN()
// {
//     struct PriorityQueue Processes;
//     InitPriorityQueue(&Processes);
//     int prevClk = getClk();
//     runningProcess.id = -1;
//     prevID = -1;
//     struct PCB newProcess;
//     struct msgBuffDummy dummy;
//     dummy.mtype = 0;
//     dummy.x[1] = 'c';
//     struct processData tempPD;
//     while (1)
//     {

        
//         if(prevClk<getClk() && runningProcess.id!=-1)
//         {
//             runningProcess.remaingTime--;
//             prevClk=getClk();
//         }

//         while ((*PG_S_shmaddr).id != prevID)
//         { //////////a lock must be put on the shared memory because if more than one process arrived at the same time
//             tempPD = *PG_S_shmaddr;
//             if (-1 == msgsnd(PG_S_msgqid, &dummy, sizeof(dummy), IPC_NOWAIT))
//                 printf("error happened in recv\n");
//             printf("arrived ID=%d, running time=%d\n", (*PG_S_shmaddr).id, (*PG_S_shmaddr).runningtime);
//             struct PCB newProcess;
//             newProcess.arrivalTime = tempPD.arrivaltime;
//             newProcess.id = tempPD.id;
//             newProcess.priority = tempPD.priority;
//             newProcess.remaingTime = tempPD.runningtime;
//             newProcess.executionTime=tempPD.runningtime;
//             newProcess.state='W';
//             priorityEnqueue(&Processes,newProcess,newProcess.remaingTime);
//             prevID = newProcess.id;

//             usleep(1);
//         }

//         if (runningProcess.id == -1)
//         {
//             // printf("-----------------------------------------------------\n");
//             struct PCB *processPtr = priorityDequeue(&Processes);
//             if (processPtr)
//             {

//                 runningProcess = *processPtr;
//                 if (runningProcess.state == 'W')
//                 {
//                     runningProcess.waitingTime = getClk() - runningProcess.arrivalTime;
//                     char runTime = runningProcess.remaingTime;
//                     char *runtimeAddress = &runTime;
//                     int PID = fork();
//                     if (PID == 0)
//                     {
//                         int check1 = execl("./process", runtimeAddress, NULL);
//                         if (check1 == -1)
//                             printf("unsuccessful execv with error%d\n", errno);
//                     }
//                     runningProcess.PID = PID;
//                     runningProcess.state = 'R';
//                     printf(" a process started ID= %d , PID = %d , \n", runningProcess.id, runningProcess.PID);
//                     fprintf(outputFile,"At time %d process %d started arr %d total %d remain %d wait %d\n",getClk(),runningProcess.id,runningProcess.arrivalTime,runningProcess.executionTime,runningProcess.remaingTime,runningProcess.waitingTime);

//                 }
//                 else if (runningProcess.state == 'S')
//                 {
//                     //while(prevClk==getClk());
//                     runningProcess.state='R';
//                     kill(runningProcess.PID,SIGCONT);
//                     runningProcess.waitingTime+=getClk()-runningProcess.lastStopped;
//                     printf("resumed process id:%d , PID:%d\n",runningProcess.id,runningProcess.PID);
//                     fprintf(outputFile,"At time %d process %d resumed arr %d total %d remain %d wait %d\n",getClk(),runningProcess.id,runningProcess.arrivalTime,runningProcess.executionTime,runningProcess.remaingTime,runningProcess.waitingTime);

//                 }
//             }
//         }

//         if (runningProcess.id != -1 && Processes.Head && Processes.Head->process.remaingTime < runningProcess.remaingTime)
//         {
//             fprintf(outputFile,"At time %d process %d stopped arr %d total %d remain %d wait %d\n",getClk(),runningProcess.id,runningProcess.arrivalTime,runningProcess.executionTime,runningProcess.remaingTime,runningProcess.waitingTime);

//             printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
//             runningProcess.state='S';
//             runningProcess.lastStopped=getClk();
//             kill(runningProcess.PID,SIGSTOP);
//             priorityEnqueue(&Processes,runningProcess,runningProcess.remaingTime);
//             printf("paused process id:%d , PID:%d\n",runningProcess.id,runningProcess.PID);
//             runningProcess.id=-1;

//         }
        

//     }
// }

// // void RR()
// // {
// //     struct Queue RR_Queue = {NULL, NULL, 0};
// //     int RR_TimeSlice = 4;
// //     struct processData currPD;
// //     struct msgBuffDummy _buffer = {'c', 0};
// //     struct PCB _pcb;
// //     int stat_loc;
// //     int _pid;
// //     runningProcess.id = -1;
// //     int clk;
// //     while (1)
// //     {
// //         // Adding a process to the round robin queue

// //         if ((*PG_S_shmaddr).id != prevID)
// //         {
// //             currPD = *PG_S_shmaddr;
// //             if(msgsnd(PG_S_msgqid, &_buffer, sizeof(_buffer), !IPC_NOWAIT)==-1)
// //              printf("error happened in recv\n");
// //             _pcb.priority = currPD.priority;
// //             _pcb.id = currPD.id;
// //             _pcb.remaingTime = currPD.runningtime;
// //             _pcb.state = 'W';
// //             _pcb.arrivalTime = currPD.arrivaltime;
// //             printf("Arrival time: %d\n", currPD.arrivaltime);
// //             enqueue(&RR_Queue, _pcb);
// //             prevID = currPD.id;
// //         }
// //         if (RR_Queue.count != 0)
// //         {
// //             if (runningProcess.id == -1)
// //             {

// //                 runningProcess = *dequeue(&RR_Queue);
// //                 if (runningProcess.state == 'W')
// //                 {

// //                     runningProcess.executionTime = getClk();
// //                     runningProcess.waitingTime = getClk() - runningProcess.arrivalTime;
// //                     _prevCLK = getClk();
// //                     int PID = fork();
// //                     if (PID == 0)
// //                     {
// //                         int check1 = execl("./process", (char *)&runningProcess.remaingTime, NULL);
// //                         if (check1 == -1)
// //                             printf("Unsuccessful execl with error%d\n", errno);
// //                     }
// //                     runningProcess.PID = PID;
// //                     runningProcess.state = 'R';
// //                     printf(" a process started ID= %d , PID = %d , \n", runningProcess.id, runningProcess.PID);
// //                     fprintf(outputFile,"At time %d process %d started arr %d total %d remain %d wait %d\n",getClk(),runningProcess.id,runningProcess.arrivalTime,runningProcess.executionTime,runningProcess.remaingTime,runningProcess.waitingTime);

// //                 }
// //                 else if (runningProcess.state == 'S')
// //                 {
// //                     runningProcess.state = 'R';
// //                     kill(runningProcess.PID, SIGCONT);
// //                     printf("resumed process id:%d , PID:%d\n", runningProcess.id, runningProcess.PID);
// //                     fprintf(outputFile,"At time %d process %d resumed arr %d total %d remain %d wait %d\n",getClk(),runningProcess.id,runningProcess.arrivalTime,runningProcess.executionTime,runningProcess.remaingTime,runningProcess.waitingTime);

// //                 }
// //             }
// //             else
// //             {
// //                 clk = getClk();
// //                 if (clk - _prevCLK == RR_TimeSlice)
// //                 {
// //                     usleep(1);
// //                     runningProcess.state = 'S';
// //                     runningProcess.remaingTime-=RR_TimeSlice;
// //                     if(runningProcess.id!=-1)
// //                     {
// //                         fprintf(outputFile,"At time %d process %d stopped arr %d total %d remain %d wait %d\n",getClk(),runningProcess.id,runningProcess.arrivalTime,runningProcess.executionTime,runningProcess.remaingTime,runningProcess.waitingTime);
// //                         kill(runningProcess.PID, SIGSTOP);
// //                         enqueue(&RR_Queue, runningProcess);
// //                         runningProcess.id = -1;
// //                     }
// //                     printf("\nSwitching process..........\n");
// //                     _prevCLK = clk;
// //                 }
// //             }
// //         }
// //     }
// // }
// void RR()
// {
//     struct Queue RR_Queue = {NULL, NULL, 0};
//     int RR_TimeSlice = 4;
//     struct processData currPD;
//     struct msgBuffDummy _buffer = {'c', 0};
//     struct PCB _pcb;
//     int stat_loc;
//     int _pid;
//     int _prevCLK;
//     int clk;
//     int prevCLK=getClk();
//     runningProcess.id = -1;
//     while (1)
//     {
//         // Adding a process to the round robin queue

//         if ((*PG_S_shmaddr).id != prevID)
//         {
//             currPD = *PG_S_shmaddr;
//             if(msgsnd(PG_S_msgqid, &_buffer, sizeof(_buffer), !IPC_NOWAIT)==-1)
//              printf("error happened in recv\n");
//             _pcb.priority = currPD.priority;
//             _pcb.id = currPD.id;
//             _pcb.remaingTime = currPD.runningtime;
//             _pcb.executionTime=currPD.runningtime;
//             _pcb.state = 'W';
//             _pcb.arrivalTime = currPD.arrivaltime;
//             printf("Arrival time: %d\n", currPD.arrivaltime);
//             enqueue(&RR_Queue, _pcb);
//             prevID = currPD.id;
            
//         }
//         clk=getClk();
//         if (RR_Queue.count != 0)
//         {
//             if (runningProcess.id == -1)
//             {
//                 runningProcess = *dequeue(&RR_Queue);
                
//                 if (runningProcess.state == 'W')
//                 {

//                     runningProcess.waitingTime = clk - runningProcess.arrivalTime;
//                     int PID = fork();
//                     if (PID == 0)
//                     {
                        
//                         int check1 = execl("./process", (char *)&runningProcess.remaingTime, NULL);
//                         if (check1 == -1)
//                             printf("Unsuccessful execl with error%d\n", errno);
//                     }
//                      _prevCLK = clk;
//                     runningProcess.PID = PID;
//                     runningProcess.state = 'R';
//                     printf(" a process started ID= %d , PID = %d , \n", runningProcess.id, runningProcess.PID);
//                     fprintf(outputFile,"At time %d process %d started arr %d total %d remain %d wait %d\n",clk,runningProcess.id,runningProcess.arrivalTime,runningProcess.executionTime,runningProcess.remaingTime,runningProcess.waitingTime);

//                 }
//                 else if (runningProcess.state == 'S')
//                 {
//                     runningProcess.state = 'R';
//                     kill(runningProcess.PID, SIGCONT);
//                      _prevCLK = clk;
//                     printf("resumed process id:%d , PID:%d\n", runningProcess.id, runningProcess.PID);
//                     fprintf(outputFile,"At time %d process %d resumed arr %d total %d remain %d wait %d\n",clk,runningProcess.id,runningProcess.arrivalTime,runningProcess.executionTime,runningProcess.remaingTime,runningProcess.waitingTime);

//                 }
//             }
//             else
//             {

                
              
//                 if (getClk() - _prevCLK == RR_TimeSlice)
//                 {
//                     runningProcess.state = 'S';
//                     runningProcess.remaingTime-=RR_TimeSlice;
//                     if(runningProcess.remaingTime!=0)
//                     {
//                     fprintf(outputFile,"At time %d process %d stopped arr %d total %d remain %d wait %d\n",getClk(),runningProcess.id,runningProcess.arrivalTime,runningProcess.executionTime,runningProcess.remaingTime,runningProcess.waitingTime);
//                     enqueue(&RR_Queue, runningProcess);
//                     runningProcess.id = -1;
//                     kill(runningProcess.PID, SIGSTOP);
//                     }
//                     printf("\nSwitching process..........\n");
//                     _prevCLK = getClk();
//                 }
//             }
//         }
//     }
// }


// int main(int argc, char *argv[])
// {
//     // initClk();
//     floatQueueInit(&WTAqueue);
//     signal(SIGUSR1, childHandler);
//     signal(SIGINT,exitHandler);
//     printf("%c\n", *argv[0]);

//     //------------------Creating a shm with Scheduler------------------//
//     PG_S_shmid = shmget(SHKEY_PG_S, sizeof(struct processData), IPC_CREAT | 0666);
//     if ((long)PG_S_shmid == -1)
//     {
//         perror("Error in creating shm between process generator and sched!\n");
//         exit(-1);
//     }
//     PG_S_shmaddr = shmat(PG_S_shmid, (void *)0, 0);
//     if ((long)PG_S_shmaddr == -1)
//     {
//         perror("Error in attaching the shm  between process generator and sched!\n");
//         exit(-1);
//     }
//     initClk();
//     prevID = -1;

//     key_t key_id = ftok("keyfile", MSGKEY_PG_S);
//     PG_S_msgqid = msgget(key_id, 0666 | IPC_CREAT);



//     outputFile=fopen("scheduler.log","w");
//     if(outputFile==NULL)
//         printf("NULL\n");
//     //fprintf(outputFile,"kdksjvbdkjfbvkdfjbvkdfba.jdfnv,dskfjvbs,djhfvb\n");

//    // HPF();
//    algoType=*argv[0];

//     if (*argv[0] == '1')
//         HPF();
//     else if (*argv[0] == '2')
//         SRTN();
//     else  if (*argv[0] == '3')
//         RR();

//     fclose(outputFile);
//     //destroyClk(true);
// }

#include "headers.h"

int PG_S_shmid;                   // shared memory id between process generator and scheduler
struct processData *PG_S_shmaddr; // address of PG_S_shmid
int prevID = -1;                  // used to check if memory changed
struct PCB runningProcess;
int PID;
int PG_S_msgqid;
FILE *outputFile;
FILE *outputFileMem;
struct floatQueue WTAqueue;
float WTASum=0;
float waitingSum = 0;
int memManagementAlgo;
bool SIGUSR1INT;

struct floatQueue memQueueArr[9];
bool mem[1024];
struct Queue waitingList;
int  totalAllocetedMemory;
int allocatedProcessID;


int firstFitAllocate(int size)
{
    int c=0;
    for(int i=0;i<1024;i++)
    {
        c=0;

        while(mem[i]==0 && i<1024 && c!=size)
        {
            c++;
            i++;
        }
        if(c==size)
        {
            for(int j=i-size;j<i;j++)
            {
                mem[j]=1;
            }
            printf("allocated memory from %d to %d\n",i-size,i);
            fprintf(outputFileMem,"At time %d allocated %d bytes for process %d from %d to %d\n",getClk(),size,allocatedProcessID,i-size,i);
            totalAllocetedMemory+=size;
            return (i-size);
        }
    }
    return -1;
}
bool firstFitCanAllocate(int size)
{
    int c=0;
    for(int i=0;i<1024;i++)
    {
        c=0;

        while(mem[i]==0 && i<1024 && c!=size)
        {
            c++;
            i++;
        }
        if(c==size)
            return true;
    }
    return false;
}
void firstFitDeallocate(int start,int size)
{
    for(int i=start;i<=start+size;i++)
        mem[i]=0;
    printf("deallocated memory from %d to %d\n",start,start+size);
    fprintf(outputFileMem,"At time %d freed %d bytes from process %d from %d to %d\n",getClk(),size,runningProcess.id,start,start+size);
    totalAllocetedMemory-=size;
}
int buddyAllocate(int size)
{
    int pos=ceil(log2(size));
    int i=pos;
    while(memQueueArr[i].count==0 && i<9)
    {
        i++;
    }
    if(i>=9)
        return -1;
    while(i>pos)
    {
        int mem1=(int)*floatDequeue(&memQueueArr[i]);
        printf("dequeued from Queue%d start %d\n",i,mem1);
        int mem2=mem1+pow(2,i-1);
        i--;
        printf("enqueued to Queue%d start %d\n",i,mem1);  
        floatEnqueue(&memQueueArr[i],mem1);
        printf("enqueued to Queue%d start %d\n",i,mem2);  
        floatEnqueue(&memQueueArr[i],mem2);
    }
    int start=(int)*(floatDequeue(&memQueueArr[pos]));
    fprintf(outputFileMem,"At time %d allocated %d bytes for process %d from %d to %d\n",getClk(),size,allocatedProcessID,start,start+(int)pow(2,pos));
    totalAllocetedMemory+=pow(2,pos);
    return start;
}
void buddyDeallocate(int start, int size)
{
    int pos=ceil(log2(size));
    int size1=size;
    size=pow(2,pos);
    totalAllocetedMemory-=size;
    fprintf(outputFileMem,"At time %d freed %d bytes from process %d from %d to %d\n",getClk(),size1,runningProcess.id,start,start+size);

    //floatEnqueue(&memQueueArr[pos],start);
    int dir=((start/size)%2==0)?1:-1;
    int target=start+dir*size;
    int temp;
    bool flag=true;
    if(pos==8)
    {
        floatEnqueue(&memQueueArr[pos],start);
        return;
    }
    while(flag && pos<8)
    {
        dir=((start/size)%2==0)?1:-1;
        target=start+dir*size;
        flag=false;
        for(int i=0;i<memQueueArr[pos].count;i++)
        {

            temp=*floatDequeue(&memQueueArr[pos]);
            if(temp==target)
            {
                printf("Merged size:%d start:%d target:%d\n",size,start,target);
                flag=true;
                pos++;
                if(dir==-1)
                    start=target;
                size*=2;
                break;
            }
            else
                floatEnqueue(&memQueueArr[pos],temp);
        }
        if(!flag || size==256)
        {
            printf("cant merge enqueued to Queue%d start %d\n",pos,start);  
            floatEnqueue(&memQueueArr[pos],start);
        }
    }


}
int allocate(int size)
{
    if(memManagementAlgo==1)
        return firstFitAllocate(size);
    else
        return buddyAllocate(size);
}
void deallocate(int start,int size)
{
    if(memManagementAlgo==1)
        firstFitDeallocate(start,size);
    else
        buddyDeallocate(start,size);
}
bool canAllocate(int size)
{
    int pos=ceil(log2(size));
    if(memManagementAlgo==1)
        return firstFitCanAllocate(size);
    else
        return (1024-totalAllocetedMemory>=pow(2,pos));
}

void childHandler()
{
    SIGUSR1INT=true;
    printf("PID of child:%d\n", runningProcess.PID);
    int status;
    int clk=getClk();
    /////////////////////deallocating//////////////////////////////////////////////////////
    deallocate(runningProcess.memStart,runningProcess.memSize);
    ///////////////////////////////////////////////////////////////////////////////////////

    int TA=clk-runningProcess.arrivalTime;
    float value = ((float)(TA))/runningProcess.executionTime;
    float WTA = ((int)(value * 100 + .5) / 100.0);
    WTASum+=WTA;
    waitingSum+=runningProcess.waitingTime;
    floatEnqueue(&WTAqueue,WTA);
    fprintf(outputFile,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",clk,runningProcess.id,runningProcess.arrivalTime,runningProcess.executionTime,0,runningProcess.waitingTime,TA,WTA);
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
    fclose(outputFileMem);
    exit(0);
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
            ///////////////setting memory size //////////////////////////////////////////
            newProcess.memSize=tempPD.memSize;
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
                //////////////////////allocating memory//////////////////////////////////////////////////////
                runningProcess.memStart=allocate(runningProcess.memSize);
                ////////////////////////////////////////////////////////////////////////////////////////////
                char *runtimeAddress = &runTime;
                int PID = fork();
                if (PID == 0)
                {
                    //printf("lmaoooooooooo\n");
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
    struct PCB *tempPCBMem;

    while (1)
    {
        SIGUSR1INT=false;
        while(waitingList.count>0 && canAllocate(waitingList.Head->process.memSize))
        {            
            tempPCBMem=dequeue(&waitingList);
            allocatedProcessID=tempPCBMem->id;
            tempPCBMem->memStart=allocate(tempPCBMem->memSize);
            priorityEnqueue(&Processes,*tempPCBMem,tempPCBMem->remaingTime);
            free(tempPCBMem);
            //printf("waiting list doesnt equal 0\n");
        }
        // if(prevClk<getClk() && runningProcess.id!=-1)
        // {
        //     runningProcess.remaingTime--;
        //     prevClk=getClk();
        // }

        while ((*PG_S_shmaddr).id != prevID)
        { //////////a lock must be put on the shared memory because if more than one process arrived at the same time
            tempPD = *PG_S_shmaddr;
            if (-1 == msgsnd(PG_S_msgqid, &dummy, sizeof(dummy), !IPC_NOWAIT))
                printf("error happened in recv\n");
            printf("arrived ID=%d, running time=%d\n", (*PG_S_shmaddr).id, (*PG_S_shmaddr).runningtime);
            struct PCB newProcess;
            newProcess.arrivalTime = tempPD.arrivaltime;
            newProcess.id = tempPD.id;
            newProcess.priority = tempPD.priority;
            newProcess.remaingTime = tempPD.runningtime;
            newProcess.executionTime=tempPD.runningtime;
            newProcess.memSize=tempPD.memSize;
            newProcess.state='W';
            newProcess.memStart=-1;
            priorityEnqueue(&Processes,newProcess,newProcess.remaingTime);
            prevID = newProcess.id;
            
            usleep(1);
        }

        if (runningProcess.id == -1 && !SIGUSR1INT)
        {
            // printf("-----------------------------------------------------\n");
            struct PCB *processPtr;
            /////////////////////////////////
            
            processPtr = priorityDequeue(&Processes);
                //printf("waiting list equals zero\n");

            //Deallocated =false;
            /////////////////////////////////
            if (processPtr )
            {

                runningProcess = *processPtr;
                free(processPtr);
                if (runningProcess.state == 'W')
                {
                    ////////////////////////////////////////////////////////////////////
                    //if(memStart==-1)
                    if(runningProcess.memStart==-1)
                    {
                        allocatedProcessID=runningProcess.id;
                        runningProcess.memStart=allocate(runningProcess.memSize);
                    }
                    if(runningProcess.memStart==-1)
                    {
                      //  printf("no enough memory\n");
                        enqueue(&waitingList,runningProcess);
                        runningProcess.id=-1;
                    }
                    /////////////////////////////////////////////////////////////////////
                    else{
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
                        prevClk=getClk();
                        runningProcess.PID = PID;
                        runningProcess.state = 'R';
                        printf(" a process started ID= %d , PID = %d , \n", runningProcess.id, runningProcess.PID);
                        fprintf(outputFile,"At time %d process %d started arr %d total %d remain %d wait %d\n",getClk(),runningProcess.id,runningProcess.arrivalTime,runningProcess.executionTime,runningProcess.remaingTime,runningProcess.waitingTime);
                    }
                }
                else if (runningProcess.state == 'S' && !SIGUSR1INT)
                {
                    //while(prevClk==getClk());
                    runningProcess.state='R';
                    kill(runningProcess.PID,SIGCONT);
                    prevClk=getClk();
                    runningProcess.waitingTime+=getClk()-runningProcess.lastStopped;
                    printf("resumed process id:%d , PID:%d\n",runningProcess.id,runningProcess.PID);
                    fprintf(outputFile,"At time %d process %d resumed arr %d total %d remain %d wait %d\n",getClk(),runningProcess.id,runningProcess.arrivalTime,runningProcess.executionTime,runningProcess.remaingTime,runningProcess.waitingTime);

                }
                // else if(SIGUSR1INT)
                // {
                //     priorityEnqueue(&Processes,runningProcess,runningProcess.remaingTime);
                //     runningProcess.id=-1;
                //     printf("interupted==========================\n");
                // }
            }
        }

        if(prevClk<getClk() && runningProcess.id!=-1)
        {
            runningProcess.remaingTime--;
            prevClk=getClk();
        }
        if (runningProcess.id != -1 && Processes.Head && Processes.Head->process.remaingTime < runningProcess.remaingTime)
        {
            if(canAllocate(Processes.Head->process.memSize) || Processes.Head->process.memStart!=-1){
                fprintf(outputFile,"At time %d process %d stopped arr %d total %d remain %d wait %d\n",getClk(),runningProcess.id,runningProcess.arrivalTime,runningProcess.executionTime,runningProcess.remaingTime,runningProcess.waitingTime);
                
                printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
                runningProcess.state='S';
                runningProcess.lastStopped=getClk();
                //usleep(1);
                kill(runningProcess.PID,SIGSTOP);
                prevClk=getClk();
                priorityEnqueue(&Processes,runningProcess,runningProcess.remaingTime);
                printf("paused process id:%d , PID:%d\n",runningProcess.id,runningProcess.PID);
                runningProcess.id=-1;
            }
            else{
                tempPCBMem=priorityDequeue(&Processes);
                enqueue(&waitingList,*tempPCBMem);
                free(tempPCBMem);
            }

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
    int clk;
    int prevCLK=getClk();
    runningProcess.id = -1;
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
            _pcb.executionTime=currPD.runningtime;
            _pcb.state = 'W';
            _pcb.arrivalTime = currPD.arrivaltime;
            printf("Arrival time: %d\n", currPD.arrivaltime);
            enqueue(&RR_Queue, _pcb);
            prevID = currPD.id;
            
        }
        clk=getClk();
        if (RR_Queue.count != 0)
        {
            if (runningProcess.id == -1)
            {
                runningProcess = *dequeue(&RR_Queue);
                
                if (runningProcess.state == 'W')
                {

                    runningProcess.waitingTime = clk - runningProcess.arrivalTime;
                    int PID = fork();
                    if (PID == 0)
                    {
                        
                        int check1 = execl("./process", (char *)&runningProcess.remaingTime, NULL);
                        if (check1 == -1)
                            printf("Unsuccessful execl with error%d\n", errno);
                    }
                     _prevCLK = clk;
                    runningProcess.PID = PID;
                    runningProcess.state = 'R';
                    printf(" a process started ID= %d , PID = %d , \n", runningProcess.id, runningProcess.PID);
                    fprintf(outputFile,"At time %d process %d started arr %d total %d remain %d wait %d\n",clk,runningProcess.id,runningProcess.arrivalTime,runningProcess.executionTime,runningProcess.remaingTime,runningProcess.waitingTime);

                }
                else if (runningProcess.state == 'S')
                {
                    runningProcess.state = 'R';
                    kill(runningProcess.PID, SIGCONT);
                     _prevCLK = clk;
                    printf("resumed process id:%d , PID:%d\n", runningProcess.id, runningProcess.PID);
                    fprintf(outputFile,"At time %d process %d resumed arr %d total %d remain %d wait %d\n",clk,runningProcess.id,runningProcess.arrivalTime,runningProcess.executionTime,runningProcess.remaingTime,runningProcess.waitingTime);

                }
            }
            else
            {

                
              
                if (getClk() - _prevCLK == RR_TimeSlice)
                {
                    runningProcess.state = 'S';
                    runningProcess.remaingTime-=RR_TimeSlice;
                    usleep(1);
                    if(runningProcess.remaingTime!=0)
                    {
                    fprintf(outputFile,"At time %d process %d stopped arr %d total %d remain %d wait %d\n",getClk(),runningProcess.id,runningProcess.arrivalTime,runningProcess.executionTime,runningProcess.remaingTime,runningProcess.waitingTime);
                    enqueue(&RR_Queue, runningProcess);
                    runningProcess.id = -1;
                    kill(runningProcess.PID, SIGSTOP);
                    }
                    printf("\nSwitching process..........\n");
                    _prevCLK = getClk();
                }
            }
        }
    }
}



int main(int argc, char *argv[])
{
    // initClk();
    ProcessQueueInit(&waitingList);
    floatQueueInit(&WTAqueue);
    for(int i=0;i<9;i++)
        floatQueueInit(&memQueueArr[i]);
    floatEnqueue(&memQueueArr[8],0);
    floatEnqueue(&memQueueArr[8],256);
    floatEnqueue(&memQueueArr[8],512);
    floatEnqueue(&memQueueArr[8],768);
    memManagementAlgo=(int)(*argv[1])-'0';
    printf("----------------------------------------------------------%d\n",memManagementAlgo);
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
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



    outputFile=fopen("scheduler.log","w");
    outputFileMem=fopen("memory.log","w");
    
    if(outputFile==NULL)
        printf("NULL\n");
    //fprintf(outputFile,"kdksjvbdkjfbvkdfjbvkdfba.jdfnv,dskfjvbs,djhfvb\n");

   // HPF();

    if (*argv[0] == '1')
        HPF();
    else if (*argv[0] == '2')
        SRTN();
    else  if (*argv[0] == '3')
        RR();

    fclose(outputFile);
    //destroyClk(true);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    // int arr[100];
    // arr[0]=buddyAllocate(7);
    // for(int i=0;i<9;i++)
    // {
    //     printf("MemQueue%d\n",i);
    //     floatPrint(&memQueueArr[i]);

    // }
    // arr[1]=buddyAllocate(7);
    // for(int i=0;i<9;i++)
    // {
    //     printf("MemQueue%d\n",i);
    //     floatPrint(&memQueueArr[i]);

    // }
    // arr[2]=buddyAllocate(15);
    // arr[3]=buddyAllocate(129);
    // arr[4]=buddyAllocate(129);
    // for(int i=0;i<9;i++)
    // {
    //     printf("MemQueue%d\n",i);
    //     floatPrint(&memQueueArr[i]);

    // }

    
    // buddyDeallocate(arr[0],7);
    // buddyDeallocate(arr[1],7);
    
    // for(int i=0;i<9;i++)
    // {
    //     printf("MemQueue%d\n",i);
    //     floatPrint(&memQueueArr[i]);

    // }
    // buddyDeallocate(arr[2],15);
    // buddyDeallocate(arr[3],129);
    // for(int i=0;i<9;i++)
    // {
    //     printf("MemQueue%d\n",i);
    //     floatPrint(&memQueueArr[i]);

    // }
    
    
    
    // buddyDeallocate(arr[4],129);

    // for(int i=0;i<9;i++)
    // {
    //     printf("MemQueue%d\n",i);
    //     floatPrint(&memQueueArr[i]);

    // }
}