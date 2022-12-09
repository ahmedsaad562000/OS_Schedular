#include "headers.h"
int mode;
int q;
int main(int argc, char *argv[])
{
    initClk();
    printf("I'm the schedular from inside my file :)\n ");
    int x= getClk();
    int follow = x;
    int msg_queue_key = atoi(argv[1]);
    mode = atoi(argv[2]);
    q = -1;
    
    int rec_value;
    struct Processes_Node process_to_be_recieved;
    if(mode == RR)
    {
        q = atoi(argv[2]);
    }
    //MSG_QUEUE between Process generator and schedular. 
    int PG_TO_SCH_MSG_QUE_ID = msgget(msg_queue_key, 0666 | IPC_CREAT);
    printf("Schedular values: mode = %d , msg_key = %d, q = %d , msg_queue_id = %d\n", mode,msg_queue_key,q,PG_TO_SCH_MSG_QUE_ID);
    if (PG_TO_SCH_MSG_QUE_ID == -1)
    {
        exit(-1);
    }
    while(1)
    {
    x = getClk();
     if(follow != x) //new second
        {
        follow = x;
        printf("Schedular: Current Time is %d\n", x);
        rec_value = msgrcv(PG_TO_SCH_MSG_QUE_ID, &process_to_be_recieved, sizeof(process_to_be_recieved.Process_Data), 0, IPC_NOWAIT);
        if(rec_value != -1)
        {
         printf("-----------Schedular_process_recieved with id: %d in time: %d\n",process_to_be_recieved.Process_Data.Process_ID , x);   
        }
        }
    }
    //TODO: implement the scheduler.
    //TODO: upon termination release the clock resources.

    destroyClk(true);
}
