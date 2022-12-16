#include "main_header.h"

/*****************************************************************************/

/**************************** Functions Definitions **************************/

/* Function to Read from file and insert it into linked list*/
void Read_file(Process_List *LinkedList, char *FileName , int mode)
{
    Process *proc = (Process *)malloc(sizeof(Process));
    FILE *pFile;
    pFile = fopen("processes.txt", "r");
    char line[256];
    int ff[4];
    while (fgets(line, sizeof(line), pFile))
    {
        if (line[0] == '#')
        {
            continue;
        }

        char *token = strtok(line, "\t");
        int c = 0;
        while (token != NULL)
        {
            ff[c] = atoi(token);
            token = strtok(NULL, "\t");
            c++;
        }

        /*set process parameters*/
        proc->Process_ID = ff[0];
        proc->Arrival_time = ff[1];
        proc->Running_time = ff[2];
        proc->Remaining_time = proc->Running_time;
        proc->Priority = (mode == SJF)?ff[2]:ff[3];
        proc->Waiting_time = 0;
        proc->TA = 0;
        proc->W_TA = 0;
        proc->State = STOPPED;

        /*Insert process into linked list*/
        Insert_Process(LinkedList, proc);
    }
    /*Close file*/
    fclose(pFile);
}

/* Function to Delete from circular queue in RR Algorithm */
void remove_From_Circular(Process_List *C_Queue, int id)
{
    struct Processes_Node *delete_node = NULL;
    struct Processes_Node *current = NULL;
    struct Processes_Node *prev = NULL;

    // check if the linked list is empty
    if (C_Queue->front == NULL)
    {
        return;
    }

    // If this is the last node to be deleted
    if (C_Queue->front->Process_Data.Process_ID == id && C_Queue->front == C_Queue->rear)
    {
        C_Queue->front = NULL;
        C_Queue->rear = NULL;
        free(C_Queue->front);
        return;
    }

    // check the first node in the linked list
    if (C_Queue->front->Process_Data.Process_ID == id)
    {
        delete_node = C_Queue->front;
        // move head to next node
        C_Queue->front = C_Queue->front->Next;
        C_Queue->rear->Next = C_Queue->front;
        free(delete_node);
        return;
    }

    // check the rest of nodes in the linked list
    current = C_Queue->front->Next;
    prev = C_Queue->front;
    while (current != C_Queue->front)
    {
        if (current->Process_Data.Process_ID == id)
        {
            // element found
            delete_node = current;
            if(delete_node ==C_Queue->rear)
            {
                C_Queue->rear=prev;
            }
            prev->Next = current->Next;
            free(delete_node);
            return;
        }
        prev = current;
        current = current->Next;
    }
}

/* Function to Check on circular Queue is empty used for RR Algorithm */
int IsEmpty_Queue(Process_List *C_Queue)
{
    return C_Queue->front == NULL;
}

/* Function to increase waiting time for not runing processes used for RR Algorithm */
void calc_Proc_waiting(Process_List *C_Queue, struct Processes_Node *curr_Proc)
{
    /*check on first node if not current proc to inc waiting time*/
    if (C_Queue->front == C_Queue->rear)
    {
        return;
    }
    if(C_Queue->front != curr_Proc)
    {
        ++C_Queue->front->Process_Data.Waiting_time;
    }
    /*check on rest of queue if not current so inc waiting*/
    struct Processes_Node *current = C_Queue->front->Next;
    while (current != C_Queue->front)
    {
        if (current != curr_Proc)
        {
            ++current->Process_Data.Waiting_time;
        }
        current = current->Next;
    }
}

/* Function to increase waiting time for not runing processes used for HPF Algorithm */
void calc_Proc_waiting_Prio(Priority_Process_List *P_Queue)
{
    /*inc waiting for not runing processes*/
    struct Processes_Node *current = P_Queue->head->Next;
    while (current != NULL)
    {
        ++current->Process_Data.Waiting_time;
        current = current->Next;
    }
}
