#include "headers.h"
#include "Semaphore.h"

/**************************** Global Variables **************************/
int mode;
int q;
int finished_process_count = 0;
int cpu_waiting_time = 0;
int total_waiting_time = 0;
float total_WTA_time = 0;
int Count_OF_Processes;
int follow;

const char *states[4] = {"STARTED", "FINISHED", "STOPPED", "RESUMED"};
/*Semaphore IDs array*/
int *semaphore_IDs; /*array of semaphore IDs where each process has a semaphore ID*/

/***********************FILES************************/
FILE *processess_file;
FILE *CPU_file;
char Process_files_path[20] = "scheduler.log";
char CPU_file_path[20] = "scheduler.perf";

/*Round Robin Variables*/
Process_List circular_Queue_RR; /*queue for RR*/
struct Processes_Node *curr_Proc = NULL;
int quanta;

/*HPF-SJF Variables*/
Process_List Priority_List_HPF_SJF;

/*Multi-Level Variables*/
int currentLevel = 0;
int quantamsLeft = 0;
MultiLevel m;
Process *currentProcess;
/**************************** Functions Declarations **************************/
/*
******** Round Robin Algorithm functions *****
*/
void Round_Robin(int *Process_Semaphore, int Time);

/*
******** HPF Algorithm functions *****
*/
void HPF_Algo(int *Process_Semaphore, int Time);

/*
******** SJF Algorithm functions *****
*/
void SJF_Algo(int *Process_Semaphore, int Time);

/*
******** MultiLevel Algorithm functions *****
*/
void MLF_Algo(int *Process_Semaphore, int Time);

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
    cpu_waiting_time = x;
    processess_file = fopen(Process_files_path, "wt");
    fputs("#At time x process state arr w total z remain y wait k\n", processess_file);

    CPU_file = fopen(CPU_file_path, "wt");

    /*General Variables*/
    follow = x;
    int msg_queue_key = atoi(argv[1]);
    int pid = 1; /*fork return value 0:child / +ve num:Parent(schedular)*/
    mode = atoi(argv[2]);

    /*Semaphore Variables*/
    Count_OF_Processes = atoi(argv[4]);       /*count of processes in system*/
    key_t semaphore_Keys[Count_OF_Processes]; /*array of semaphore keys where each process has a semaphore key*/
    union Semun semun;                        /*Semaphpre union*/
    semaphore_IDs = (int *)malloc(Count_OF_Processes * sizeof(int));
    // for (int i = 0; i < 8; i++)  intialize semaphore ids by -1 at first
    // {
    //     semaphore_IDs[i] = -1;
    // }

    int rec_value = 1;
    struct Processes_Node process_to_be_recieved;

    /*ROUND ROBIN VARIABLES INITIALIZE*/
    quanta = q;
    /*MultiLevel VARIABLES INITIALIZE*/
    for (int i = 0; i < 11; i++)
    {
        m.listOfQueues[i].front = NULL;
    }
    m.toBeReturnedToItsLevel.front = NULL;

    if (mode == RR || mode == MLFL)
    {
        q = atoi(argv[3]);
    }
    quantamsLeft = q;
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

        /*Enter loop each one second only*/
        if (follow != x)
        {
            follow = x;
            printf("Schedular: Current Time is %d\n real_count_of_process = %d , current count = %d\n", x, Count_OF_Processes, finished_process_count);

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
                        /* code */
                        pushIntoPriorityQueue(&Priority_List_HPF_SJF, &process_to_be_recieved.Process_Data);
                        break;
                    case HPF:
                        /* code */
                        if (isPriorityQueueEmpty(&Priority_List_HPF_SJF))
                        {
                            printf("\n from If condition \n");
                            pushIntoPriorityQueue(&Priority_List_HPF_SJF, &process_to_be_recieved.Process_Data);
                            curr_Proc = Priority_List_HPF_SJF.front;
                            break;
                        }
                        pushIntoPriorityQueue(&Priority_List_HPF_SJF, &process_to_be_recieved.Process_Data);
                        break;

                    case RR:
                        /* code */
                        if (IsEmpty_Queue(&circular_Queue_RR)) /*first insertion in ready queue*/
                        {
                            Insert_Circular_Queue(&circular_Queue_RR, &process_to_be_recieved.Process_Data);
                            curr_Proc = circular_Queue_RR.front; /*intialize current pointer to be pointing to front of circular queue*/
                            break;
                        }
                        Insert_Circular_Queue(&circular_Queue_RR, &process_to_be_recieved.Process_Data);
                        break;

                    case MLFL:
                        /* code */
                        pushIntoMultiLevel(&m, &process_to_be_recieved.Process_Data);
                        break;

                    default:
                        break;
                    }

                    // store
                }
            }

            /*switch case on modes of operation*/

            switch (mode)
            {
            case SJF:

                /* code */
                SJF_Algo(semaphore_IDs, follow);
                break;

            case HPF:
                /* code */
                if (!isPriorityQueueEmpty(&Priority_List_HPF_SJF))
                {
                    HPF_Algo(semaphore_IDs, follow);
                }
                else
                {
                    ++cpu_waiting_time;
                    /*Increase the waiting time for schedular*/
                }
                break;

            case RR:
                /* code */
                if (!IsEmpty_Queue(&circular_Queue_RR))
                {
                    Round_Robin(semaphore_IDs, follow);
                }
                else
                {
                    ++cpu_waiting_time;
                    /*Increase the waiting time for schedular*/
                }

                break;

            case MLFL:
                /* code */
                MLF_Algo(semaphore_IDs, follow);
                break;

            default:
                break;
            }

            /*Make rec_value = 1 to re-enter the check recieved loop again*/
            rec_value = 1;

            /*IF Schedular finished its job it Terminates*/
            if (finished_process_count == Count_OF_Processes)
            {
                // if(mode== SJF){follow++;cpu_waiting_time++;}
                char line_to_print[256];

                sprintf(line_to_print, "CPU utilization = %.2f %%\nAvg WTA = %.2f\nAvg Waiting = %.2f \n", ((float)(follow - cpu_waiting_time) * 100) / (float)follow, total_WTA_time / Count_OF_Processes, (float)total_waiting_time / (float)Count_OF_Processes);
                fputs(line_to_print, CPU_file);
                printf("cpu_wainting_time= %d , curr_time = %d", cpu_waiting_time, follow);
                fclose(CPU_file);
                fclose(processess_file);

                kill(getppid(), SIGINT);
            }
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
void Round_Robin(int *Process_Semaphore, int Time)
{
    printf("******************************************************************\n");
    printf("State of  %d  is: %s and remaining is %d\n", curr_Proc->Process_Data.Process_ID, states[curr_Proc->Process_Data.State], curr_Proc->Process_Data.Remaining_time);
    printf("******************************************************************\n");

    /*Variable to prevent enter resumed condition if started*/
    static int startt = 0;
    up(Process_Semaphore[curr_Proc->Process_Data.Process_ID - 1]); /*up the current semaphore*/


    /*enter if it is the first run for the process*/
    if (curr_Proc->Process_Data.Remaining_time == curr_Proc->Process_Data.Running_time)
    {
        curr_Proc->Process_Data.State = STARTED;
        startt = 1;  /*make state started*/
        PRINT_CURR_PROCESS(&curr_Proc->Process_Data, Time, processess_file);
    }

    /*Check if we reach the quantum to switch or Remaining time is zero*/
    if (quanta == 0 || curr_Proc->Process_Data.Remaining_time == 0)
    {
        quanta = q;

        /* ------------ switch to next pr pointer----------------------- */
        struct Processes_Node *to_delete;
        to_delete = curr_Proc;

        /*Enter only if there is more than 1 process*/
        if (circular_Queue_RR.front != circular_Queue_RR.rear)
        {
            curr_Proc->Process_Data.State = STOPPED;
            startt = 0;

            /*if process is finished*/
            if (curr_Proc->Process_Data.Remaining_time == 0)
            {
                to_delete->Process_Data.State = FINISHED;
                finished_process_count++;
            }

            /*Print stopped process information*/
            PRINT_CURR_PROCESS(&to_delete->Process_Data, Time, processess_file);

            /*Switch process*/
            curr_Proc = curr_Proc->Next;

            /*Print switch new process information*/
            if (curr_Proc->Process_Data.Remaining_time == curr_Proc->Process_Data.Running_time)
            {
                curr_Proc->Process_Data.State = STARTED;
                startt = 1;
            }
            else if (startt != 1)
            {
                curr_Proc->Process_Data.State = RESUMED;
            }
            PRINT_CURR_PROCESS(&curr_Proc->Process_Data, Time, processess_file);
        }

        /*Enter if current process finishes execution(remaining time is 0)*/
        if (to_delete->Process_Data.Remaining_time == 0)
        {
            if (circular_Queue_RR.front == circular_Queue_RR.rear)  /*so one element in the queue*/
            {
                /*Print finish process information*/
                to_delete->Process_Data.State = FINISHED;
                finished_process_count++;
                PRINT_CURR_PROCESS(&to_delete->Process_Data, Time, processess_file);
            }
            /*Calculate waiting time & WTA time for schedular*/
            total_waiting_time += to_delete->Process_Data.Waiting_time;
            total_WTA_time += to_delete->Process_Data.W_TA;

            remove_From_Circular(&circular_Queue_RR, to_delete->Process_Data.Process_ID);

            if (IsEmpty_Queue(&circular_Queue_RR))
            {
                curr_Proc = NULL;
                ++cpu_waiting_time;
            }
        }
    }

    /*Enter only if the removed element is not last one in the circular queue*/
    if (curr_Proc != NULL)
    {
        calc_Proc_waiting(&circular_Queue_RR, curr_Proc); /*Function to increase waiting time for not runing processes in ready queue*/
        --curr_Proc->Process_Data.Remaining_time;         /*decrease remaining time for the runing process*/
    }
    /*decrease quantum each call*/
    --quanta;
    // printf("\n*_*_*_*_**_*_ Current is: %p *_*_*_*_**_*_\n",curr_Proc);
}
/*
******** HPF Algorithm functions *****
*/
void HPF_Algo(int *Process_Semaphore, int Time)
{
    /*Functions to be performed each clk*/
    printf("******************************************************************\n");
    printf("State of  %d  is: %d and remaining is %d\n", Priority_List_HPF_SJF.front->Process_Data.Process_ID, Priority_List_HPF_SJF.front->Process_Data.State, Priority_List_HPF_SJF.front->Process_Data.Remaining_time);
    printf("******************************************************************\n");

    /*Variable to prevent enter resumed condition if started*/
    static int startt = 0;
    static Process_States P_state = STOPPED;

    up(Process_Semaphore[Priority_List_HPF_SJF.front->Process_Data.Process_ID - 1]); /*up the current semaphore*/

    /*Check if process primited and stopped*/
    if (curr_Proc->Process_Data.Process_ID != Priority_List_HPF_SJF.front->Process_Data.Process_ID && curr_Proc->Process_Data.Remaining_time!=0)
    {
        // print information for Preemtid Process
        curr_Proc->Process_Data.State = STOPPED;
        P_state = STOPPED;
        // if(P_state != curr_Proc->Process_Data.State)
        // {
        //     PRINT_CURR_PROCESS(Priority_List_HPF_SJF.front, Time, processess_file);
        //     P_state = curr_Proc->Process_Data.State;
        // }
        if (curr_Proc)
        {
            PRINT_CURR_PROCESS(&curr_Proc->Process_Data, Time, processess_file);
        }
        curr_Proc = Priority_List_HPF_SJF.front;
    }

    /*Check if remaining time for runing process is zero */
    if (Priority_List_HPF_SJF.front->Process_Data.Remaining_time == 0)
    {
        /*switch to next process and remove the process from ready queue*/
        Priority_List_HPF_SJF.front->Process_Data.State = FINISHED;
        finished_process_count++;
        P_state = FINISHED;
        startt = 0;

        /*Print finish process information*/
        PRINT_CURR_PROCESS(&Priority_List_HPF_SJF.front->Process_Data, Time, processess_file);
        total_waiting_time += Priority_List_HPF_SJF.front->Process_Data.Waiting_time;
        total_WTA_time += Priority_List_HPF_SJF.front->Process_Data.W_TA;
        up(Process_Semaphore[Priority_List_HPF_SJF.front->Process_Data.Process_ID - 1]); /*up the current semaphore*/
        popFromQueue(&Priority_List_HPF_SJF);
        // make pointer points to new front
        curr_Proc = Priority_List_HPF_SJF.front;
    }

    if (Priority_List_HPF_SJF.front != NULL)
    {
        /*Set States of runing Process*/
        if (Priority_List_HPF_SJF.front->Process_Data.Remaining_time == Priority_List_HPF_SJF.front->Process_Data.Running_time)
        {
            Priority_List_HPF_SJF.front->Process_Data.State = STARTED;
            if (P_state != STARTED)
            {
                PRINT_CURR_PROCESS(&Priority_List_HPF_SJF.front->Process_Data, Time, processess_file);
                P_state = STARTED;
            }
            startt = 1;
        }
        else if (startt != 1)
        {
            Priority_List_HPF_SJF.front->Process_Data.State = RESUMED;
            if (P_state != RESUMED)
            {
                PRINT_CURR_PROCESS(&Priority_List_HPF_SJF.front->Process_Data, Time, processess_file);
                P_state = RESUMED;
            }
        }
        // PRINT_CURR_PROCESS(Priority_List_HPF_SJF.front, Time, processess_file);
        calc_Proc_waiting_Prio(&Priority_List_HPF_SJF);             /*Function to increase waiting time for not runing processes in ready queue*/
        --Priority_List_HPF_SJF.front->Process_Data.Remaining_time; /*decrease remaining time for the runing process*/
    }
    if (isPriorityQueueEmpty(&Priority_List_HPF_SJF))
        cpu_waiting_time++;
}

void SJF_Algo(int *Process_Semaphore, int Time)
{
    // added for timing
    static int process_count = 0;
    
    finished_process_count = process_count;
    
    // added for last_up in the schedular
    static bool last_up = true;
    
    //added to check if process is finished
    static int check = 0;

    // there is no process to run in the queue;

    if (isPriorityQueueEmpty(&Priority_List_HPF_SJF))
    {
        // there is no process to run in the queue;
        if (curr_Proc == NULL) // no_currently_running_process
        {
            /*add_waiting_time*/
            cpu_waiting_time++;
        }
        else if (curr_Proc->Process_Data.State == FINISHED) // Current_Process_is_finished_and_no_process_to_run
        {
            /* make last up*/
            if (last_up)
            {
                last_up = false;
                up(Process_Semaphore[curr_Proc->Process_Data.Process_ID - 1]);
            }
            /*add_waiting_time*/
            cpu_waiting_time++;
        }
        else // There is a process running
        {
            check = RUN_CURR_PROCESS(curr_Proc, Process_Semaphore, &Priority_List_HPF_SJF, Time, processess_file);
            process_count += check;
            if (check == 1)
            {
                total_waiting_time += curr_Proc->Process_Data.Waiting_time;
                total_WTA_time += curr_Proc->Process_Data.W_TA;
            }
        }
    }
    else
    {
        /*there is processess in the queue*/
        if (curr_Proc == NULL) // no_currently_running_process aka First_process in the system
        {
            /////////////////////////////////////////////////////////////////////////////////////////
            // allocate a new node and make a copy of the head data since it will be freed after dequeue
            curr_Proc = malloc(sizeof(struct Processes_Node));
            COPY_then_DEQUEUE_HEAD(curr_Proc, &Priority_List_HPF_SJF);
            /////////////////////////////////////////////////////////////////////////////////////////

            /*print_process_start*/
            PRINT_CURR_PROCESS(&curr_Proc->Process_Data, Time, processess_file);

            /*run_process*/
            check = RUN_CURR_PROCESS(curr_Proc, Process_Semaphore, &Priority_List_HPF_SJF, Time, processess_file);
            process_count += check;
            if (check == 1)
            {
                total_waiting_time += curr_Proc->Process_Data.Waiting_time;
                total_WTA_time += curr_Proc->Process_Data.W_TA;
            }

            // if(finished_process_count == Process_COUNT){/*finish*/}
        }
        else if (curr_Proc->Process_Data.State == FINISHED) // Current_Process_is_finished_so_switch_to_another
        {
            last_up = true; // make last up
            up(Process_Semaphore[curr_Proc->Process_Data.Process_ID - 1]);

            COPY_then_DEQUEUE_HEAD(curr_Proc, &Priority_List_HPF_SJF);
            /*print_process_start*/
            PRINT_CURR_PROCESS(&curr_Proc->Process_Data, Time, processess_file);

            check = RUN_CURR_PROCESS(curr_Proc, Process_Semaphore, &Priority_List_HPF_SJF, Time, processess_file);
            process_count += check;
            if (check == 1) // process_is_finished
            {
                total_waiting_time += curr_Proc->Process_Data.Waiting_time;
                total_WTA_time += curr_Proc->Process_Data.W_TA;
            }

            // if(finished_process_count == Process_COUNT){/*finish*/}
        }
        else // There is a process running
        {
            check = RUN_CURR_PROCESS(curr_Proc, Process_Semaphore, &Priority_List_HPF_SJF, Time, processess_file);
            process_count += check;
            if (check == 1)
            {
                total_waiting_time += curr_Proc->Process_Data.Waiting_time;
                total_WTA_time += curr_Proc->Process_Data.W_TA;
            }
            // if(finished_process_count == Process_COUNT){/*finish*/}
        }
    }
}

void MLF_Algo(int *Process_Semaphore, int Time)
{
    //check if multilevel levels and buffer are empty
    if (isMultiLevelEmpty(&m) && currentProcess == NULL)
        cpu_waiting_time++;
    else
    {
        //check if there is no current process running
        if (currentProcess == NULL)
        {
            //check if levels are empty and buffer is not
            if (AreAllLevelsEmpty(&m))
                pushAllProcessBackToItsLevel(&m);
            //reset quantam, get next process to run, and set process state
            quantamsLeft = q;
            currentProcess = getNextProcessFromMultiLevel(&m, &currentLevel);
            setProcessState(currentProcess);
            PRINT_CURR_PROCESS(currentProcess, Time, processess_file);
        }
        //run process
        runMultiLevelProcess(&currentProcess, currentLevel, Process_Semaphore, &m, Time, processess_file, &finished_process_count , &total_waiting_time , &total_WTA_time);
        quantamsLeft--;
        //check if quantam is finsihed
        if (quantamsLeft == 0 && currentProcess != NULL)
        {
            quantamsLeft = q;
            currentProcess->State = STOPPED;
            PRINT_CURR_PROCESS(currentProcess, Time+1, processess_file);
            pushIntoNextLevel(currentLevel, currentProcess, &m);
            currentProcess = NULL;
        }
    }
}
/*
******** Handler to remove all semaphores *****
*/
void handler(int signum)
{
    if (finished_process_count != Count_OF_Processes)
    {
        char line_to_print[256];
        sprintf(line_to_print, "CPU utilization = %.2f %%\nAvg WTA = %.2f\nAvg Waiting = %.2f \n", ((float)cpu_waiting_time * 100) / (float)follow, total_WTA_time / Count_OF_Processes, (float)total_waiting_time / (float)Count_OF_Processes);
        fputs(line_to_print, CPU_file);
        fclose(CPU_file);
        fclose(processess_file);
    }
    // printf("\nSCh: ana matt with CPU_WT = %d , TOTAL_WTA = %.2f, TOTAL_Processess_WT = %d \n", cpu_waiting_time, total_WTA_time, total_waiting_time);
    for (size_t i = 0; i < 8; i++)
    {
        semctl(semaphore_IDs[i], 0, IPC_RMID, NULL);
    }
    exit(0);
}
