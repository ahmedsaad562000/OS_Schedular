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

Process *peekIntoPriorityQueue(Priority_Process_List *P_Queue)
{
    /*if list is already empty*/
    if (P_Queue->head == NULL)
    {
        return NULL;
    }
    return &P_Queue->head->Process_Data;
}

void popFromPriorityQueue(Priority_Process_List *P_Queue)
{
    /*if list is already empty*/
    if (P_Queue->head == NULL)
    {
        return;
    }
    struct Processes_Node *temp = P_Queue->head;
    P_Queue->head = P_Queue->head->Next;
    free(temp);
}

void pushIntoPriorityQueue(Priority_Process_List *P_Queue, Process *newProcess)
{
    // Create new Node
    struct Processes_Node *temp = (struct Processes_Node *)malloc(sizeof(struct Processes_Node));
    temp->priority = newProcess->Priority;
    temp->Process_Data.Arrival_time = newProcess->Arrival_time;
    temp->Process_Data.PID = newProcess->PID;
    temp->Process_Data.Process_ID = newProcess->Process_ID;
    temp->Process_Data.Remaining_time = newProcess->Remaining_time;
    temp->Process_Data.Running_time = newProcess->Running_time;
    temp->Process_Data.State = newProcess->State;
    temp->Process_Data.TA = newProcess->TA;
    temp->Process_Data.W_TA = newProcess->W_TA;
    temp->Process_Data.Waiting_time = newProcess->Waiting_time;

    /*if list is already empty*/
    if (P_Queue->head == NULL)
    {
        P_Queue->head = temp;
        temp->Next = NULL;
        return;
    }

    struct Processes_Node *start = P_Queue->head;
    struct Processes_Node *prev_start = P_Queue->head;
    if (P_Queue->head->priority > newProcess->Priority)
    {
        P_Queue->head->Process_Data.State = STOPPED; 
        temp->Next = P_Queue->head;
        P_Queue->head = temp;
    }
    else
    {
        start = P_Queue->head->Next;
        while (start != NULL && start->priority < newProcess->Priority)
        {
            start = start->Next;
            prev_start = prev_start->Next;
        }
        temp->Next = prev_start->Next;
        prev_start->Next = temp;
    }
}

bool isPriorityQueueEmpty(Priority_Process_List *P_Queue)
{
    return P_Queue->head == NULL;
}
