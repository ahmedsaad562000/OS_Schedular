#include "main_header.h"

Process *newProcess(int Process_ID, int Arrival_time, int Running_time, int TA, int W_TA, int Remaining_time, int Waiting_time, int Priority, Process_States State)
{
    Process *temp = (Process *)malloc(sizeof(Process));
    temp->Process_ID = Process_ID;
    temp->Arrival_time = Arrival_time;
    temp->Running_time = Running_time;
    temp->TA = TA;
    temp->W_TA = W_TA;
    temp->Remaining_time = Remaining_time;
    temp->Waiting_time = Waiting_time;
    temp->Priority = Priority;
    temp->State = State;

    return temp;
}

struct Processes_Node *newPriorityQueueNode(Process *N)
{
    int priority = N->Priority;
    struct Processes_Node *temp = (struct Processes_Node *)malloc(sizeof(struct Processes_Node));
    temp->priority = priority;
    temp->Process_Data = *N;
    temp->Next = NULL;

    return temp;
}

Process *peekIntoQueue(Process_List *P_Queue)
{
    /*if list is already empty*/
    if (P_Queue->front == NULL)
    {
        return NULL;
    }
    return &P_Queue->front->Process_Data;
}

void popFromQueue(Process_List *P_Queue)
{
    /*if list is already empty*/
    if (P_Queue->front == NULL)
    {
        return;
    }
    struct Processes_Node *temp = P_Queue->front;
    P_Queue->front = P_Queue->front->Next;
    free(temp);
}

void pushIntoPriorityQueue(Process_List *P_Queue, Process *newProcess)
{
    // Create new Node
    struct Processes_Node *temp = newPriorityQueueNode(newProcess);
    /*if list is already empty*/
    if (P_Queue->front == NULL)
    {
        P_Queue->front = temp;
        temp->Next = NULL;
        return;
    }

    struct Processes_Node *start = P_Queue->front;
    struct Processes_Node *prev_start = P_Queue->front;
    if (P_Queue->front->priority > newProcess->Priority)
    {
        P_Queue->front->Process_Data.State = STOPPED;
        temp->Next = P_Queue->front;
        P_Queue->front = temp;
    }
    else
    {
        start = P_Queue->front->Next;
        while (start != NULL && start->priority < newProcess->Priority)
        {
            start = start->Next;
            prev_start = prev_start->Next;
        }
        temp->Next = prev_start->Next;
        prev_start->Next = temp;
    }
}

int isPriorityQueueEmpty(Process_List *P_Queue)
{
    return P_Queue->front == NULL;
}

void pushIntoQueue(Process_List *P_Queue, Process *newProcess)
{
    struct Processes_Node *temp = newPriorityQueueNode(newProcess);
    /*if list is already empty*/
    if (P_Queue->front == NULL)
    {
        P_Queue->front = temp;
        temp->Next = NULL;
        return;
    }
}

void pushIntoMultiLevel(MultiLevel *m, Process *newProcess)
{
    int priority = newProcess->Priority;
    Process_List *priorityList = &m->listOfQueues[priority];
    Insert_Process(priorityList, newProcess);
}
void pushIntoNextLevel(int currLevel, Process *processToBePushedIntoNextLevel, MultiLevel *m)
{
    int priority = currLevel + 1;
    if (priority != 10)
    {
        Process_List *priorityList = &m->listOfQueues[priority];
        Insert_Process(priorityList, processToBePushedIntoNextLevel);
    }
    else
        Insert_Process(&m->toBeReturnedToItsLevel, processToBePushedIntoNextLevel);

    popFromQueue(&m->listOfQueues[currLevel]);
}
int AreAllLevelsEmpty(MultiLevel *m)
{
    for (int i = 0; i < 10; i++)
    {
        if (m->listOfQueues[i].front != NULL)
        {
            return 0;
        }
    }
    return 1;
}
int isMultiLevelEmpty(MultiLevel *m)
{
    if (m->toBeReturnedToItsLevel.front == NULL && AreAllLevelsEmpty(m))
        return 1;
    else
        return 0;
}
int pushAllProcessBackToItsLevel(MultiLevel *m)
{
    while (!isPriorityQueueEmpty(&m->toBeReturnedToItsLevel))
    {
        Process *processData = peekIntoQueue(&m->toBeReturnedToItsLevel);
        pushIntoMultiLevel(m, processData);
        popFromQueue(&m->toBeReturnedToItsLevel);
    }
}
Process *getNextProcessFromMultiLevel(MultiLevel *m, int *currentLevel)
{
    for (int i = 0; i < 10; i++)
    {
        if (!isPriorityQueueEmpty(&m->listOfQueues[i]))
        {
            *currentLevel = i;
            return peekIntoQueue(&m->listOfQueues[i]);
        }
    }
    return NULL;
}

void AddWaitingMultiLevel(MultiLevel *m, Process *currentProcess)
{
    for (int i = 0; i < 10; i++)
    {
        Add_waiting_SJF(&m->listOfQueues[i]);
    }
    Add_waiting_SJF(&m->toBeReturnedToItsLevel);
    --currentProcess->Waiting_time;
}

void runMultiLevelProcess(Process **currentProcess, int currentLevel, int *Process_Semaphore, MultiLevel *m, int Time, FILE *processess_file, int *finishedProcessCount)
{
    /*run_curr_process_logic*/
    up(Process_Semaphore[(*currentProcess)->Process_ID - 1]);
    AddWaitingMultiLevel(m, (*currentProcess)); /*Function to increase waiting time for not runing processes in ready queue*/
    --(*currentProcess)->Remaining_time;        /*decrease remaining time for the runing process*/
    /*check if this clock was process last_clock*/
    if ((*currentProcess)->Remaining_time == 0)
    {
        up(Process_Semaphore[(*currentProcess)->Process_ID - 1]);
        (*currentProcess)->State = FINISHED;
        /*Print finish process information*/
        PRINT_CURR_PROCESS((*currentProcess), Time + 1, processess_file);
        popFromQueue(&m->listOfQueues[currentLevel]);
        *currentProcess = NULL;
        ++*finishedProcessCount;
    }
}

void setProcessState(Process *p)
{
    if (p->Remaining_time == p->Running_time)
        p->State = STARTED;
    else
        p->State = RESUMED;
}