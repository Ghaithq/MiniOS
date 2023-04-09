#include "headers.h"


int main(int argc, char * argv[])
{
    //initClk();
    printf("%c\n",*argv[0]);

    //------------------Creating a shm with Scheduler------------------//
    int PG_S_shmid=shmget(SHKEY_PG_S,sizeof(struct processData),IPC_CREAT | 0644);
    if ((long)PG_S_shmid == -1)
    {
        perror("Error in creating shm between process generator and sched!\n");
        exit(-1);
    }
    struct processData* PG_S_shmaddr = shmat(PG_S_shmid, (void *)0, 0);
    if ((long)PG_S_shmaddr == -1)
    {
        perror("Error in attaching the shm  between process generator and sched!\n");
        exit(-1);
    }
    int prevID=-1;//used to check if memory changed

    
    //upon termination release the clock resources.



    while(1){


        // recvVal==msgrcv(PGmsgqid,&msgPG,sizeof(struct msgbuffPG),0,IPC_NOWAIT);
        // if(recvVal==-1)
        // {
        //     if(errno!=ENOMSG)
        //         printf("recvID=%d\n",msgPG.data.id);
        // }
        // else
        // {
        //     printf("recvID=%d\n",msgPG.data.id);
        // }



        if((*PG_S_shmaddr).id!=prevID)
        {
            printf("recvID=%d\n",(*PG_S_shmaddr).id);
            prevID=(*PG_S_shmaddr).id;
        }

    }
    //destroyClk(true);
}

