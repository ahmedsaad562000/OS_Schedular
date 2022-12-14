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

typedef struct PriorityNode
{
<<<<<<< HEAD
=======
    int data;
>>>>>>> 94f1b2d2df6d8df54af2bd8840209df95a80b652
    int priority;
    Process *Process_Data;
    struct PriorityNode *next;
} PriorityNode;

PriorityNode *newPriorityQueueNode(Process *N)
{
    int priority = N->Priority;
    PriorityNode *temp = (PriorityNode *)malloc(sizeof(PriorityNode));
    temp->priority = priority;
    temp->Process_Data = N;
    temp->next = NULL;

    return temp;
}

Process *peekIntoPriorityQueue(PriorityNode **head)
{
    return (*head)->Process_Data;
}

void popFromPriorityQueue(PriorityNode **head)
{
    PriorityNode *temp = *head;
    (*head) = (*head)->next;
    free(temp);
}

void pushIntoPriorityQueue(PriorityNode **head, Process *newProcess)
{
    PriorityNode *start = (*head);
    int p = newProcess->Priority;
    // Create new Node
    PriorityNode *temp = newPriorityQueueNode(newProcess);

    if ((*head)->priority > p)
    {
        temp->next = *head;
        (*head) = temp;
    }
    else
    {
        while (start->next != NULL &&
               start->next->priority < p)
        {
            start = start->next;
        }
        temp->next = start->next;
        start->next = temp;
    }
}

int isPriorityQueueEmpty(PriorityNode **head)
{
    return (*head) == NULL;
}
