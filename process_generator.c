#include "headers.h"

/*

* Variables
*/
int mode;
int q = -1;
int PG_TO_SCH_KEY;
int Count_OF_Processes=0;
Process_List Processes;
int PG_TO_SCH_MSG_QUE_ID;

void clearResources(int);
int main(int argc, char *argv[])
{
    PG_TO_SCH_KEY = 120;
    // MSG_QUEUE between Process generator and schedular.
    PG_TO_SCH_MSG_QUE_ID = msgget(PG_TO_SCH_KEY, 0666 | IPC_CREAT);
    if (PG_TO_SCH_MSG_QUE_ID == -1)
    {
        perror("Error in create msg queue");
        exit(-1);
    }

    signal(SIGINT, clearResources);
    signal(SIGUSR1, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    Read_file(&Processes, argv[1]);
    struct Processes_Node *curr = Processes.front;
    while(curr!=NULL)
    {
        // printf(" %d %d %d %d \n",curr->Process_Data.Process_ID,curr->Process_Data.Arrival_time,curr->Process_Data.Running_time,curr->Process_Data.Priority);
        ++ Count_OF_Processes;
        curr=curr->Next;
    }
    curr = Processes.front;
    // 2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.

    mode = atoi(argv[1]);

    if (mode == RR || mode == MLFL)
    {
        q = atoi(argv[2]);
    }
    // 3. Initiate and create the scheduler and clock processes.

    printf("\nmy pid = %d\n", getpid());
    pid_t clk, sch;

    clk = fork();

    if (clk == 0)
    {
        printf("\nI'm the clock hellooo\n");
        // clock
        char *args[] = {"./clk.out", NULL};
        execv(args[0], args);
    }
    else
    {
        sch = fork();

        if (sch == 0)
        {
            // sch_process
            printf("\nI'm the schedular Helloo!\n");
            // schedular
            char mode_str[8];
            char key_str[8];
            char q_str[8];
            char countProc_str[8];
            sprintf(mode_str, "%d", mode);
            sprintf(key_str, "%d", PG_TO_SCH_KEY);
            sprintf(q_str, "%d", q);
            sprintf(countProc_str, "%d", Count_OF_Processes);

            char *args[] = {"./sch.out", key_str, mode_str, q_str, countProc_str, NULL};
            execv(args[0], args);
        }
        else
        {
            /* Parent Code */

            printf("\nI am the Process_gen\n");

            initClk();
            // To get time use this function.
            int x = getClk();
            int follow = x;
            // TODO Generation Main Loop
            // 5. Create a data structure for processes and provide it with its parameters.
            // Already done by A.Amin
            // 6. Send the information to the scheduler at the appropriate time.
            curr = Processes.front;
            int send_value = 1;
            printf("PG values: mode = %d , msg_key = %d, q = %d , msg_queue_id = %d\n", mode, PG_TO_SCH_KEY, q, PG_TO_SCH_MSG_QUE_ID);
            while (1)
            {
                x = getClk();
                if (follow != x) // new_second
                {
                    follow = x;
                    // printf("Process Generator: Current Time is %d\n", x);
                    if (curr != NULL && x == curr->Process_Data.Arrival_time)
                    {
                        send_value = 1;
                        while (send_value != -1 && curr != NULL && x == curr->Process_Data.Arrival_time)
                        {
                            send_value = msgsnd(PG_TO_SCH_MSG_QUE_ID, curr, sizeof(curr->Process_Data), IPC_NOWAIT);
                            if (send_value != -1)
                            {
                                printf("------PG sent process with id = %d in time %d\n", curr->Process_Data.Process_ID, x);
                                curr = curr->Next;
                            }
                        }
                    }
                }
            }

            // 7. Clear clock resources
            destroyClk(true);
        }
    }
}

void clearResources(int signum)
{
    printf("\npg killed\n");
    
    destroyClk(true);
    // TODO Clears all resources in case of interruption or finished
    msgctl(PG_TO_SCH_MSG_QUE_ID, IPC_RMID, NULL);
    killpg(getpgrp(), SIGKILL);
}
