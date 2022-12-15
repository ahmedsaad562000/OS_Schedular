#include "headers.h"
#include "Semaphore.h"

/**************************** Global Variables **************************/
int mode;
int q;
/*Semaphore IDs array*/
int semaphore_IDs[8]; /*array of semaphore IDs where each process has a semaphore ID*/
/*Round Robin Variables*/
Process_List circular_Queue_RR; /*queue for RR*/
struct Processes_Node *curr_Proc;
int quanta;

/**************************** Functions Declarations **************************/
/*
******** Round Robin Algorithm functions *****
*/
void Round_Robin(int *Process_Semaphore);

/*
******** Handler to remove all semaphores *****
*/
void handler(int signum);

/**************************** Main Program **************************/
int main(int argc, char *argv[])
{
    signal(SIGINT, handler);
    initClk();
    printf("I'm the schedular from inside my file :)\n ");
    int x = getClk();

    /*General Variables*/
    int follow = x;
    int msg_queue_key = atoi(argv[1]);
    int pid = 1; /*fork return value 0:child / +ve num:Parent(schedular)*/
    mode = atoi(argv[2]);

    /*Semaphore Variables*/
    const int Count_OF_Processes = atoi(argv[4]); /*count of processes in system*/
    key_t semaphore_Keys[Count_OF_Processes];     /*array of semaphore keys where each process has a semaphore key*/
    union Semun semun;                            /*Semaphpre union*/

    // for (int i = 0; i < 8; i++)  intialize semaphore ids by -1 at first
    // {
    //     semaphore_IDs[i] = -1;
    // }

    int rec_value = 1;
    struct Processes_Node process_to_be_recieved;

    /*ROUND ROBIN VARIABLES INITIALIZE*/
    quanta = q;

    if (mode == RR || mode == MLFL)
    {
        q = atoi(argv[3]);
    }
    // MSG_QUEUE between Process generator and schedular.
    int PG_TO_SCH_MSG_QUE_ID = msgget(msg_queue_key, 0666 | IPC_CREAT);
    printf("Schedular values: mode = %d , msg_key = %d, q = %d , msg_queue_id = %d\n", mode, msg_queue_key, q, PG_TO_SCH_MSG_QUE_ID);
    if (PG_TO_SCH_MSG_QUE_ID == -1)
    {
        exit(-1);
    }
    while (1)
    {
        x = getClk();

        // printf("\nX is %d and follow is: %d \n", x, follow);

        /*Enter loop each one second only*/
        if (follow != x)
        {
            follow = x;
            printf("Schedular: Current Time is %d\n", x);

            /*Enter when recieve new processes from generator*/
            while (rec_value != -1)
            {
                // printf("\n------------- Stucked --------------\n");
                rec_value = msgrcv(PG_TO_SCH_MSG_QUE_ID, &process_to_be_recieved, sizeof(process_to_be_recieved.Process_Data), 0, IPC_NOWAIT);

                // printf("\n********* Recieved val= %d ***********\n", rec_value);

                if (rec_value != -1)
                {
                    printf("\n-----------Schedular_process_recieved with id: %d in time: %d\n", process_to_be_recieved.Process_Data.Process_ID, x);

                    /*Fork only if Schedular (Parent)*/
                    pid = fork();
                    if (pid != 0)
                    {
                        /*Create semaphore for each recieved process for Schedular (Parent only run this part)*/
                        process_to_be_recieved.Process_Data.PID = pid;
                        semaphore_Keys[process_to_be_recieved.Process_Data.Process_ID - 1] = process_to_be_recieved.Process_Data.Process_ID;
                        semaphore_IDs[process_to_be_recieved.Process_Data.Process_ID - 1] = semget(semaphore_Keys[process_to_be_recieved.Process_Data.Process_ID - 1], 1, 0666 | IPC_CREAT);
                        if (semaphore_IDs[process_to_be_recieved.Process_Data.Process_ID - 1] == -1)
                        {
                            perror("Error in create sem");
                            exit(-1);
                        }
                        semun.val = 0;
                        if (semctl(semaphore_IDs[process_to_be_recieved.Process_Data.Process_ID - 1], 0, SETVAL, semun) == -1)
                        {
                            perror("Error in semctl");
                            exit(-1);
                        }
                        printf("Sechdular: Semaphore is %d\n", semaphore_IDs[process_to_be_recieved.Process_Data.Process_ID - 1]);
                    }
                    else
                    {
                        /*Child (process) program only
                         *Run process.c program
                         */
                        printf("\nI'm the Process hellooo!!!\n");
                        char pID_str[8];
                        char Proc_RemTime_str[8];
                        sprintf(pID_str, "%d", process_to_be_recieved.Process_Data.Process_ID);
                        sprintf(Proc_RemTime_str, "%d", process_to_be_recieved.Process_Data.Running_time);
                        char *args[] = {"./process.out", pID_str, Proc_RemTime_str, NULL};
                        execv(args[0], args);
                    }

                    /*switch case on modes of operation to put process in specified queue*/
                    switch (mode)
                    {
                    case SJF:
                    case HPF:
                        /* code */
                        break;

                    case RR:
                        /* code */
                        if (IsEmpty_Queue(&circular_Queue_RR)) /*first insertion in ready queue*/
                        {
                            Insert_Circular_Queue(&circular_Queue_RR, &process_to_be_recieved.Process_Data);
                            curr_Proc = circular_Queue_RR.front; /*intialize current pointer to be pointing to head of circular queue*/
                            break;
                        }
                        Insert_Circular_Queue(&circular_Queue_RR, &process_to_be_recieved.Process_Data);
                        break;

                    case MLFL:
                        /* code */
                        break;

                    default:
                        break;
                    }

                    // store
                }
            }

            /*switch case on modes of operation*/
            if (!IsEmpty_Queue(&circular_Queue_RR))
            {
                switch (mode)
                {
                case SJF:
                    /* code */
                    break;

                case HPF:
                    /* code */
                    break;

                case RR:
                    /* code */
                    Round_Robin(semaphore_IDs);
                    break;

                case MLFL:
                    /* code */
                    break;

                default:
                    break;
                }
            }

            /*Make rec_value = 1 to re-enter the check recieved loop again*/
            rec_value = 1;
        }
    }
    // TODO: implement the scheduler.
    // TODO: upon termination release the clock resources.

    // printf("--------------- Finished Schedular --------------\n");
    destroyClk(true);
}

/**************************** Functions Definitions **************************/

/*
******** Round Robin Algorithm functions *****
*/
void Round_Robin(int *Process_Semaphore)
{
    /*Functions to be performed each clk*/
    up(Process_Semaphore[curr_Proc->Process_Data.Process_ID - 1]); /*up the current semaphore*/
    --curr_Proc->Process_Data.Remaining_time;                     /*decrease remaining time for the runing process*/
    calc_Proc_waiting(&circular_Queue_RR,curr_Proc);             /*Function to increase waiting time for not runing processes in ready queue*/

    /*Check if we reach the quantum to switch*/
    if (quanta == 0 || curr_Proc->Process_Data.Remaining_time == 0)
    {
        if (quanta == 0)
            quanta = q;
        /*switch to next pr*/
        struct Processes_Node *to_delete;
        to_delete = curr_Proc;
        curr_Proc = curr_Proc->Next;
        if (to_delete->Process_Data.Remaining_time == 0)
        {
            /*remove the process from readycircular queue*/
            remove_From_Circular(&circular_Queue_RR, to_delete->Process_Data.Process_ID);
        }
    }
}

/*
******** Handler to remove all semaphores *****
*/
void handler(int signum)
{
    printf("\nSCh: ana matt\n");
    for (size_t i = 0; i < 8; i++)
    {
        semctl(semaphore_IDs[i], 0, IPC_RMID, NULL);
    }
    exit(0);
}
