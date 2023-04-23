#include "headers.h"
int PG_S_shmid;
int PG_S_msgqid;


void clearResources()
{
    //TODO Clears all resources in case of interruption
    if(-1 == (shmctl(PG_S_shmid, IPC_RMID, NULL)))
    {   
        perror("shmctl");
    }         
    if(-1 == (msgctl(PG_S_msgqid, IPC_RMID, NULL)))
    {   
        printf("error in deleting msgq\n");
    } 

    killpg(getpgrp(),SIGINT);
    exit(0);
}


struct msgBuffDummy
{
    char x[1];
    int mtype;
};




int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);



    
    // TODO Initialization
    // 1. Read the input files.
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    // 3. Initiate and create the scheduler and clock processes.
    // 4. Use this function after creating the clock process to initialize clock
    //------------------for the chosen scheduling algorithm---------------//
    //signal(SIGINT,clearResources);
    printf("choose a memory management algorithm:\n");
    printf("1-First Fit\n");
    printf("2-Buddy\n");
    char memManagementAlgo;
    scanf("%c",&memManagementAlgo);
    
    char algorithm;
    
    printf("choose a scheduling algorithm:\n");
    printf("1-Highest Priority First\n");
    printf("2-Shortest Remaining Time Next\n");
    printf("3-Round Robin\n");
    scanf("%c",&algorithm);
    scanf("%c",&algorithm);
    printf("memManagementAlgo: %c, algorithm=%c\n",memManagementAlgo,algorithm);
    //sleep(5);
    char *algorithmAddress=&algorithm;

    //------------------Creating clock and scheduling processes----------------//
    if(fork()==0)
        execl("./clk",argv[0],NULL);
    initClk();
    if(fork()==0)
        execl("./scheduler",algorithmAddress,&memManagementAlgo,NULL);
    

    //------------------Creating a shm with Scheduler------------------//
    PG_S_shmid = shmget(SHKEY_PG_S, sizeof(struct processData), IPC_CREAT | 0666);
    if ((long)PG_S_shmid == -1)
    {
        perror("Error in creating shm between process generator and sched!\n");
        exit(-1);
    }
    struct processData* PG_S_shmaddr = shmat(PG_S_shmid, (void *)0, 0);
    (*PG_S_shmaddr).id=-1;
    if ((long)PG_S_shmaddr == -1)
    {
        perror("Error in attaching the shm  between process generator and sched!\n");
        exit(-1);
    }
    //------------------processing data from file------------------//
    // To get time use this
    int x = getClk();


    //------------------creating a msgq between process gen and sched------------------//
    key_t key_id=ftok("keyfile",MSGKEY_PG_S);
    PG_S_msgqid=msgget(key_id,0666 | IPC_CREAT);



    

    FILE* pFile=fopen("processes.txt","r");
    printf("current time is %d\n", x);
    struct processData process;
    if(pFile==NULL)
        printf("couldn't open file");
    
    struct Queue processesQ;
    ProcessQueueInit(&processesQ);



    struct msgBuffDummy dummy;
    dummy.mtype=0;
    dummy.x[1]='c';

    int p;
    while(1)
    {

        fscanf(pFile,"%d",&p);
        process.id=p;
        fscanf(pFile,"%d",&p);
        process.arrivaltime=p;
        fscanf(pFile,"%d",&p);
        process.runningtime=p;
        fscanf(pFile,"%d",&p);
        process.priority=p;
        fscanf(pFile,"%d",&p);
        process.memSize=p;
        if(feof(pFile))
            break;
        while(process.arrivaltime!=getClk());
        (*PG_S_shmaddr)=process;
        msgrcv(PG_S_msgqid,&dummy,sizeof(dummy),0,!IPC_NOWAIT);
        //usleep(1000);

        printf("*PG_S_shmaddr.id=%d\n",(*PG_S_shmaddr).id);
        printf("id=%d , arrival time=%d ,clock=%d\n",process.id,process.arrivaltime,getClk());
    }
    while(1);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    //destroyClk(true);
}
