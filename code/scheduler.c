
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
float WTASum = 0;
float waitingSum = 0;
int memManagementAlgo;
bool SIGUSR1INT;

// struct floatQueue memQueueArr[9];
struct FloatPriorityQueue memQueueArr[9];
bool mem[1024];
struct Queue waitingList;
int totalAllocatedMemory;
int allocatedProcessID;

int firstFitAllocate(int size)
{
    int c = 0;
    for (int i = 0; i < 1024; i++)
    {
        c = 0;

        while (mem[i] == 0 && i < 1024 && c != size)
        {
            c++;
            i++;
        }
        if (c == size)
        {
            for (int j = i - size; j < i; j++)
            {
                mem[j] = 1;
            }
            printf("allocated memory from %d to %d\n", i - size, i);
            fprintf(outputFileMem, "At time %d allocated %d bytes for process %d from %d to %d\n", getClk(), size, allocatedProcessID, i - size, i);
            totalAllocatedMemory += size;
            return (i - size);
        }
    }
    return -1;
}
bool firstFitCanAllocate(int size)
{
    int c = 0;
    for (int i = 0; i < 1024; i++)
    {
        c = 0;

        while (mem[i] == 0 && i < 1024 && c != size)
        {
            c++;
            i++;
        }
        if (c == size)
            return true;
    }
    return false;
}
void firstFitDeallocate(int start, int size)
{
    for (int i = start; i < start + size; i++)
        mem[i] = 0;
    printf("deallocated memory from %d to %d\n", start, start + size);
    fprintf(outputFileMem, "At time %d freed %d bytes from process %d from %d to %d\n", getClk(), size, runningProcess.id, start, start + size);
    totalAllocatedMemory -= size;
}
int buddyAllocate(int size)
{
    int pos = ceil(log2(size));
    int i = pos;
    while (memQueueArr[i].count == 0 && i < 9)
    {
        i++;
    }
    if (i >= 9)
        return -1;
    while (i > pos)
    {
        int mem1 = (int)*floatPriorityDequeue(&memQueueArr[i]);
        printf("dequeued from Queue%d start %d\n", i, mem1);
        int mem2 = mem1 + pow(2, i - 1);
        i--;
        printf("enqueued to Queue%d start %d\n", i, mem1);
        floatPriorityEnqueue(&memQueueArr[i], mem1, mem1);
        printf("enqueued to Queue%d start %d\n", i, mem2);
        floatPriorityEnqueue(&memQueueArr[i], mem2, mem2);
    }
    int start = (int)*(floatPriorityDequeue(&memQueueArr[pos]));
    printf("finished buddy Allocationxxxxxxxxx\n");
    fprintf(outputFileMem, "At time %d allocated %d bytes for process %d from %d to %d\n", getClk(), size, allocatedProcessID, start, start + (int)pow(2, pos));
    totalAllocatedMemory += pow(2, pos);
    printf("finished buddy Allocationxxxxxxxxx\n");
    return start;
}
void buddyDeallocate(int start, int size)
{
    int pos = ceil(log2(size));
    int size1 = size;
    size = pow(2, pos);
    totalAllocatedMemory -= size;
    fprintf(outputFileMem, "At time %d freed %d bytes from process %d from %d to %d\n", getClk(), size1, runningProcess.id, start, start + size);
    // floatEnqueue(&memQueueArr[pos],start);
    int dir = ((start / size) % 2 == 0) ? 1 : -1;
    int target = start + dir * size;
    int temp;
    bool flag = true;
    if (pos == 8)
    {
        floatPriorityEnqueue(&memQueueArr[pos], start, start);
        return;
    }
    while (flag && pos < 8)
    {
        struct FloatPriorityQueue tempQueueMem;
        InitFloatPriorityQueue(&tempQueueMem);
        dir = ((start / size) % 2 == 0) ? 1 : -1;
        target = start + dir * size;
        flag = false;
        int memQueueArrPosCount = memQueueArr[pos].count;
        //  printf("11111111111111111111111111111111111%d\n",memQueueArrPosCount);
        for (int i = 0; i < memQueueArrPosCount; i++)
        {

            temp = *floatPriorityDequeue(&memQueueArr[pos]);
            if (temp == target)
            {
                printf("Merged size:%d start:%d target:%d\n", size, start, target);
                flag = true;
                pos++;
                if (dir == -1)
                    start = target;
                size *= 2;
                break;
            }
            else
                floatPriorityEnqueue(&tempQueueMem, temp, temp);
        }
        while (tempQueueMem.count > 0)
        {
            temp = *floatPriorityDequeue(&tempQueueMem);
            if (flag)
                floatPriorityEnqueue(&memQueueArr[pos - 1], temp, temp);
            else
                floatPriorityEnqueue(&memQueueArr[pos], temp, temp);
        }

        if (!flag || size == 256)
        {
            printf("cant merge enqueued to Queue%d start %d\n", pos, start);
            floatPriorityEnqueue(&memQueueArr[pos], start, start);
        }
    }
}
int allocate(int size)
{
    if (memManagementAlgo == 1)
        return firstFitAllocate(size);
    else
        return buddyAllocate(size);
}
void deallocate(int start, int size)
{
    if (memManagementAlgo == 1)
        firstFitDeallocate(start, size);
    else
        buddyDeallocate(start, size);
}
bool canAllocate(int size)
{
    int pos = ceil(log2(size));
    if (memManagementAlgo == 1)
        return firstFitCanAllocate(size);
    else
        return (1024 - totalAllocatedMemory >= pow(2, pos));
}

void childHandler()
{
    SIGUSR1INT = true;
    printf("PID of child:%d\n", runningProcess.PID);
    int status;
    int clk = getClk();
    /////////////////////deallocating//////////////////////////////////////////////////////
    deallocate(runningProcess.memStart, runningProcess.memSize);
    ///////////////////////////////////////////////////////////////////////////////////////

    int TA = clk - runningProcess.arrivalTime;
    float value = ((float)(TA)) / runningProcess.executionTime;
    float WTA = ((int)(value * 100 + .5) / 100.0);
    WTASum += WTA;
    waitingSum += runningProcess.waitingTime;
    floatEnqueue(&WTAqueue, WTA);
    fprintf(outputFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", clk, runningProcess.id, runningProcess.arrivalTime, runningProcess.executionTime, 0, runningProcess.waitingTime, TA, WTA);
    runningProcess.id = -1;
    wait(&status);
}

void exitHandler()
{

    int count = WTAqueue.count;
    floatPrint(&WTAqueue);
    float avgWTA = WTASum / count;
    float avgWaiting = waitingSum / count;
    float stdWTA = 0;
    for (int i = 0; i < count; i++)
    {
        float x = *floatDequeue(&WTAqueue);
        stdWTA += ((x - avgWTA) * (x - avgWTA));
    }
    stdWTA /= (count - 1);
    stdWTA = sqrt((double)stdWTA);
    FILE *averageFile = fopen("scheduler.perf", "w");
    fprintf(averageFile, "CPU utilization = 100%% \n");
    fprintf(averageFile, "Avg WTA = %f\n", avgWTA);
    fprintf(averageFile, "Avg Waiting = %f\n", avgWaiting);
    fprintf(averageFile, "Std WTA == %f\n", stdWTA);

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
    int prevCLK = getClk();
    struct PCB *tempPCBMem;
    runningProcess.id = -1;
    SIGUSR1INT = false;
    while (1)
    {
        // Adding a process to the round robin queue

        clk = getClk();

        int waitingListCount = waitingList.count;
        if (SIGUSR1INT)
        {
            for (int i = 0; i < waitingListCount; i++)
            {
                tempPCBMem = dequeue(&waitingList);
                if (tempPCBMem && canAllocate(tempPCBMem->memSize))
                {
                    printf("tempPCBMemID=%d ,memSize=%d, i=%d\n", tempPCBMem->id, tempPCBMem->memSize, i);
                    printf("allocated********************\n");
                    allocatedProcessID = tempPCBMem->id;
                    tempPCBMem->memStart = allocate(tempPCBMem->memSize);
                    enqueue(&RR_Queue, *tempPCBMem);
                }
                else
                    enqueue(&waitingList, *tempPCBMem);
                printf("freed tempPCBMemID=%d\n", tempPCBMem->id);
                free(tempPCBMem);
            }
            printf("waitingListCount=%d\n", waitingList.count);
            SIGUSR1INT = false;
        }
        if ((*PG_S_shmaddr).id != prevID)
        {
            currPD = *PG_S_shmaddr;
            if (msgsnd(PG_S_msgqid, &_buffer, sizeof(_buffer), !IPC_NOWAIT) == -1)
                printf("error happened in recv\n");
            _pcb.priority = currPD.priority;
            _pcb.id = currPD.id;
            _pcb.remaingTime = currPD.runningtime;
            _pcb.executionTime = currPD.runningtime;
            _pcb.state = 'W';
            _pcb.arrivalTime = currPD.arrivaltime;
            _pcb.memSize = currPD.memSize;
            _pcb.memStart = -1;
            printf("Arrival time: %d\n", currPD.arrivaltime);
            prevID = currPD.id;
            if (waitingList.count > 0 && !canAllocate(_pcb.memSize))
                enqueue(&waitingList, _pcb);
            else
                enqueue(&RR_Queue, _pcb);
        }
        if (RR_Queue.count != 0)
        {
            if (runningProcess.id == -1)
            {
                runningProcess = *dequeue(&RR_Queue);

                if (runningProcess.state == 'W')
                {
                    if (runningProcess.memStart == -1)
                    {
                        allocatedProcessID = runningProcess.id;
                        if (runningProcess.memStart = allocate(runningProcess.memSize) == -1)
                        {
                            enqueue(&waitingList, runningProcess);
                            runningProcess.id = -1;
                        }
                        else
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
                            fprintf(outputFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", clk, runningProcess.id, runningProcess.arrivalTime, runningProcess.executionTime, runningProcess.remaingTime, runningProcess.waitingTime);
                        }
                    }
                }
                else if (runningProcess.state == 'S')
                {
                    runningProcess.state = 'R';
                    kill(runningProcess.PID, SIGCONT);
                    _prevCLK = clk;
                    printf("resumed process id:%d , PID:%d\n", runningProcess.id, runningProcess.PID);
                    fprintf(outputFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", clk, runningProcess.id, runningProcess.arrivalTime, runningProcess.executionTime, runningProcess.remaingTime, runningProcess.waitingTime);
                }
            }
            else
            {
                if (clk - _prevCLK == RR_TimeSlice)
                {
                    if(canAllocate(RR_Queue.Head->process.memSize) || RR_Queue.Head->process.memStart != -1)
                    {
                    runningProcess.remaingTime -= RR_TimeSlice;
                    runningProcess.state = 'S';
                    if (runningProcess.remaingTime > 0 && runningProcess.id != -1)
                    {
                        fprintf(outputFile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", clk, runningProcess.id, runningProcess.arrivalTime, runningProcess.executionTime, runningProcess.remaingTime, runningProcess.waitingTime);
                        enqueue(&RR_Queue, runningProcess);
                        runningProcess.id = -1;
                        runningProcess.state = ' ';
                        kill(runningProcess.PID, SIGUSR2);
                    }
                    printf("\nSwitching process..........\n");
                    _prevCLK = getClk();
                    }
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
    for (int i = 0; i < 9; i++)
        InitFloatPriorityQueue(&memQueueArr[i]);
    floatPriorityEnqueue(&memQueueArr[8], 0, 0);
    floatPriorityEnqueue(&memQueueArr[8], 256, 256);
    floatPriorityEnqueue(&memQueueArr[8], 512, 512);
    floatPriorityEnqueue(&memQueueArr[8], 768, 768);
    memManagementAlgo = (int)(*argv[1]) - '0';
    // printf("----------------------------------------------------------%d\n",memManagementAlgo);
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    signal(SIGUSR1, childHandler);
    signal(SIGINT, exitHandler);
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

    outputFile = fopen("scheduler.log", "w");
    outputFileMem = fopen("memory.log", "w");

    if (outputFile == NULL)
        printf("NULL\n");
    // fprintf(outputFile,"kdksjvbdkjfbvkdfjbvkdfba.jdfnv,dskfjvbs,djhfvb\n");

    // HPF();

    if (*argv[0] == '3')
        RR();

    fclose(outputFile);
    // destroyClk(true);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // int arr[100];
    // arr[0]=buddyAllocate(7);
    // for(int i=0;i<9;i++)
    // {
    //     printf("MemQueue%d\n",i);
    //     floatPriorityPrint(&memQueueArr[i]);

    // }
    // arr[1]=buddyAllocate(7);
    // for(int i=0;i<9;i++)
    // {
    //     printf("MemQueue%d\n",i);
    //     //floatPriorityPrint(&memQueueArr[i]);

    // }
    // arr[2]=buddyAllocate(15);
    // arr[3]=buddyAllocate(129);
    // arr[4]=buddyAllocate(129);
    // for(int i=0;i<9;i++)
    // {
    //     printf("MemQueue%d\n",i);
    //     floatPriorityPrint(&memQueueArr[i]);

    // }

    // buddyDeallocate(arr[0],7);
    // buddyDeallocate(arr[1],7);

    // for(int i=0;i<9;i++)
    // {
    //     printf("MemQueue%d\n",i);
    //     floatPriorityPrint(&memQueueArr[i]);

    // }
    // buddyDeallocate(arr[2],15);
    // buddyDeallocate(arr[3],129);
    // for(int i=0;i<9;i++)
    // {
    //     printf("MemQueue%d\n",i);
    //     floatPriorityPrint(&memQueueArr[i]);

    // }

    // buddyDeallocate(arr[4],129);

    // for(int i=0;i<9;i++)
    // {
    //     printf("MemQueue%d\n",i);
    //     floatPriorityPrint(&memQueueArr[i]);

    // }
}