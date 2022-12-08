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

typedef short bool;
#define true 1
#define false 0

#define SHKEY 300

///==============================
//don't mess with this variable//
int *shmaddr; //

//===============================

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

/*****************************************************************************/

/**************************** Functions Definitions **************************/

/* Function to Insert Process into Queue */
Processes_Node* Insert_Process(Process_List *Queue, Process *N)
{
    // Allocate memory for new node
    Processes_Node *link = (Processes_Node *)malloc(sizeof(Processes_Node));

    // insert the data to the new node and make the next point to null
    link->Process_Data.Process_ID = N->Process_ID;
    link->Process_Data.Arrival_time = N->Arrival_time;
    link->Process_Data.Remaining_time = N->Remaining_time;
    link->Process_Data.Running_time = N->Running_time;
    link->Process_Data.Waiting_time = N->Waiting_time;
    link->Process_Data.TA = N->TA;
    link->Process_Data.W_TA = N->W_TA;
    link->Process_Data.State = N->State;
    link->Process_Data.Priority = N->Priority;

    // If head is empty, create new list
    if (Queue->front == NULL)
    {
        Queue->front = link;
        Queue->rear = link;
        return link;
    }
    /* If not empty --> insert after Rear */
    link->Next = NULL;
    Queue->rear->Next = link;
    Queue->rear = link;

    return link;
}

/* Function to Insert Process into Queue using Running Time */
Processes_Node *Insert_Process_Running_Time(Process_List *Queue, Process *N)
{

}

/* Function to Insert Process Into Queue using Priority */
Processes_Node *Insert_Process_Priority(Process_List *Queue, Process *N)
{

}
/******************************************************************************/
