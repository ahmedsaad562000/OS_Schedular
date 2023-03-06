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
#include <string.h>
#include "Semaphore.h"
#include <math.h>
#define atoa(x) #x




/**
 * ======================= Variables ========================
 */
typedef short bool;
#define true 1
#define false 0

#define SHKEY 300

///==============================
// don't mess with this variable//
int *shmaddr; //

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
        // Make sure that the clock exists
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
typedef enum
{
    STARTED = 0,
    FINISHED = 1,
    STOPPED = 2,
    RESUMED = 3
} Process_States;
/* Enum for Scheduling Algorithm */
typedef enum
{
    SJF = 0,
    HPF = 1,
    RR = 2,
    MLFL = 3
} SCHEDULING_MODE;
/* struct for Process information */
typedef struct
{
    int Process_ID;
    int PID;
    int Arrival_time;
    int Running_time;
    int TA;
    float W_TA;
    int Remaining_time;
    int Waiting_time;
    int Priority;
    int memsize;
    int memindex;
    int no_of_blocks;
    Process_States State;
} Process;

/* Node of Process */
struct Processes_Node
{
    int priority;
    Process Process_Data;
    struct Processes_Node *Next;
};

typedef struct
{
    /* Pointer to first & Last elements in the Queue */
    struct Processes_Node *front;
    struct Processes_Node *rear;
} Process_List;

typedef struct
{
    Process_List listOfQueues[10];
    Process_List toBeReturnedToItsLevel;
} MultiLevel;
/*****************************************************************************/

/**************************** Functions  **************************/

/**************************** Functions Declarations **************************/

/* Function to Insert Process into Queue */
struct Processes_Node *Insert_Process(Process_List *Queue, Process *N);
struct Processes_Node *Insert_Circular_Queue(Process_List *Queue, Process *N);

/* Function to Delete from circular queue in RR Algorithm */
void remove_From_Circular(Process_List *C_Queue, int id);



/*Ali's functions*/
Process *peekIntoPriorityQueue(Process_List *P_Queue);
void popFromQueue(Process_List *P_Queue);
void pushIntoPriorityQueue(Process_List *P_Queue, Process *newProcess);
int isPriorityQueueEmpty(Process_List *P_Queue);
void pushIntoMultiLevel(MultiLevel *m, Process *newProcess);
void pushIntoNextLevel(int currLevel, Process *processToBePushedIntoNextLevel, MultiLevel *m);
int AreAllLevelsEmpty(MultiLevel *m);
int isMultiLevelEmpty(MultiLevel *m);
int pushAllProcessBackToItsLevel(MultiLevel *m);
Process *getNextProcessFromMultiLevel(MultiLevel *m, int *currentLevel);
void AddWaitingMultiLevel(MultiLevel *m, Process *currentProcess);
void runMultiLevelProcess(Process **currentProcess, int currentLevel, int *Process_Semaphore, MultiLevel *m, int Time, FILE *processess_file, int *finishedProcessCount, int *total_waiting_time, float *total_WTA_time, bool *memory, FILE *processess_mem_file);
/*SJF*/
void Add_waiting_SJF(Process_List *P_Queue);
void COPY_then_DEQUEUE_HEAD(struct Processes_Node *curr_Proc, Process_List *P_Queue);
/*Print*/
void PRINT_CURR_PROCESS(Process *curr_Proc, int Time, FILE *processess_file);
void Close_file(FILE *file);
void Open_file_to_write(char *FileName, FILE *file);


int get_no_of_blocks(int mem_size);
int check_first_empty_location_if_odd(bool* mem);
int get_first_place(bool* mem  ,int blocks_to_reserve);
int mem_alloc(bool* mem , int blocks_to_reserve);
void mem_dealloc(bool* mem, int blocks_to_free , int index);
int RUN_CURR_PROCESS(struct Processes_Node *curr_Proc, int *Process_Semaphore, Process_List *P_Queue, int Time, FILE *processess_file, bool *memory, FILE *processess_mem_file);
void PRINT_MEMORY_DEALLOC(Process *curr_Proc, int Time, FILE *processess_file);

#endif