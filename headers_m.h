#include "main_header.h"
//===============================


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