#include "headers.h"

/*
* Variables
*/
Process_List Processes;

void clearResources(int);

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    Read_file(&Processes,argv[1]);
    Processes_Node* curr=Processes.front;
    while(curr!=NULL)
    {
        printf(" %d %d %d %d \n",curr->Process_Data.Arrival_time,curr->Process_Data.Priority,curr->Process_Data.Running_time,curr->Process_Data.Process_ID);
        curr=curr->Next;
    }
    // 2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.
    // 3. Initiate and create the scheduler and clock processes.
    // 4. Use this function after creating the clock process to initialize clock.
    initClk();
    // To get time use this function. 
    while(1)
	{
	int x = getClk();
    printf("Current Time is %d\n", x);
    }
	// TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}
