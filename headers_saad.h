#include "main_header.h"

// Multi-Level uses this please do not change
void Add_waiting_SJF(Process_List *P_Queue) // add waiting for all processes in queue
{
    /*inc waiting for not runing processes*/
    struct Processes_Node *current = P_Queue->front;
    while (current != NULL)
    {
        ++current->Process_Data.Waiting_time;
        current = current->Next;
    }
}
int RUN_CURR_PROCESS(struct Processes_Node *curr_Proc, int *Process_Semaphore, Process_List *P_Queue, int Time, FILE *processess_file)
{

    /*run_curr_process_logic*/
    up(Process_Semaphore[curr_Proc->Process_Data.Process_ID - 1]);
    Add_waiting_SJF(P_Queue);                 /*Function to increase waiting time for not runing processes in ready queue*/
    --curr_Proc->Process_Data.Remaining_time; /*decrease remaining time for the runing process*
    ////////////////////////////
    /*check if this clock was process last_clock*/
    if (curr_Proc->Process_Data.Remaining_time == 0)
    {

        curr_Proc->Process_Data.State = FINISHED;
        /*Print finish process information*/
        PRINT_CURR_PROCESS(&curr_Proc->Process_Data, Time + 1, processess_file);
        return 1;
    }
    return 0;
}

void PRINT_CURR_PROCESS(Process *curr_Proc, int Time, FILE *processess_file)
{
    static char *state[4] = {"started", "finished", "stopped", "resumed"};
    char line_to_print[256];
    snprintf(line_to_print, 200, "At time %d process %d %s arr %d total %d remain %d wait %d", Time, curr_Proc->Process_ID, state[curr_Proc->State], curr_Proc->Arrival_time, curr_Proc->Running_time, curr_Proc->Remaining_time, curr_Proc->Waiting_time); // puts string into buffer
    if (curr_Proc->State == FINISHED)
    {
        /*calculate TA and WTA*/

        curr_Proc->TA = Time - curr_Proc->Arrival_time;
        curr_Proc->W_TA = (float)curr_Proc->TA / (float)curr_Proc->Running_time;
        /////////////////////////////////////////////
        char concatenated_line[100];
        sprintf(concatenated_line, " TA %d WTA %.2f", curr_Proc->TA, curr_Proc->W_TA); // puts string into buffer
        strcat(line_to_print, concatenated_line);
    }
    /*write to file*/
    strcat(line_to_print, "\n");
    // printf(line_to_print);
    fputs(line_to_print, processess_file); // writing data into file
}

void PRINT_CURR_MEMORY(Process *curr_Proc, int Time, FILE *processess_file)
{
    static char *state[4] = {"started", "finished", "stopped", "resumed"};
    char line_to_print[256];
    // snprintf(line_to_print, 200, "At time %d allocated %d bytes %d for %d process %d from %d to %d", Time, curr_Proc->Process_ID, state[curr_Proc->State], curr_Proc->Arrival_time, curr_Proc->Running_time, curr_Proc->Remaining_time, curr_Proc->Waiting_time); // puts string into buffer
}

void COPY_then_DEQUEUE_HEAD(struct Processes_Node *curr_Proc, Process_List *P_Queue)
{
    curr_Proc->Process_Data = P_Queue->front->Process_Data;
    popFromQueue(P_Queue);
    curr_Proc->Process_Data.State = STARTED;
}

/*void Open_file_to_write(char *FileName , FILE *file)
{
    file = fopen(FileName, "wt");
    fputs("HI BABY\n",file);

}*/

/*void Close_file(FILE *file)
{
    fclose(file);
}*/
