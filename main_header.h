#ifndef MAIN_HEADER_H
#define MAIN_HEADER_H
#include <stdio.h> //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int getClk()
{
    return *shmaddr;
}

/*
 * All processes call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
*/
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *)shmat(shmid, (void *)0, 0);
}

/*
 * All processes call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
*/

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}
/************************ Enums & Structs Definition**************************/
/* Enum for Process states */
typedef enum{
    STARTED ,
    FINISHED ,
    STOPPED , 
    RESUMED
}Process_States;

/* struct for Process information */
typedef struct{
    int Process_ID;
    int Arrival_time;
    int Running_time;
    int TA;
    int W_TA;
    int Remaining_time;
    int Waiting_time;
    int Priority;
    Process_States State;
}Process;

/* Node of Process */
typedef struct{
    Process Process_Data;
    Process_Node* Next;
}Processes_Node;

typedef struct{
    /* Pointer to first & Last elements in the Queue */
    Processes_Node *front = NULL;
    Processes_Node *rear = NULL;
}Process_List;
#endif