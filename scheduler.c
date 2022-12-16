#include "headers.h"
#include "Semaphore.h"

/**************************** Global Variables **************************/
int mode;
int q;
int finished_process_count = 0;
int cpu_waiting_time = 0;
int total_waiting_time = 0;
float total_WTA_time = 0;

const char* states[4] = {"STARTED" , "FINISHED", "STOPPED" , "RESUMED" }; 
/*Semaphore IDs array*/
int semaphore_IDs[8]; /*array of semaphore IDs where each process has a semaphore ID*/

/***********************FILES************************/
FILE *processess_file;
FILE *CPU_file;
char  Process_files_path[20] = "scheduler.log";
char CPU_file_path[20] = "scheduler.perf";


/*Round Robin Variables*/
Process_List circular_Queue_RR; /*queue for RR*/
struct Processes_Node *curr_Proc = NULL;
int quanta;

/*HPF-SJF Variables*/
Priority_Process_List Priority_List_HPF_SJF;
/**************************** Functions Declarations **************************/
/*
******** Round Robin Algorithm functions *****
*/
void Round_Robin(int *Process_Semaphore , int Time);

/*
******** HPF Algorithm functions *****
*/
void HPF_Algo(int *Process_Semaphore , int Time);

/*
******** SJF Algorithm functions *****
*/
void SJF_Algo(int *Process_Semaphore , int Time);

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
    
    processess_file = fopen(Process_files_path, "wt");
    fputs("#At time x process state arr w total z remain y wait k\n",processess_file);

    CPU_file = fopen(CPU_file_path, "wt");

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
                        /* code */
                        pushIntoPriorityQueue(&Priority_List_HPF_SJF, &process_to_be_recieved.Process_Data);
                        break;
                    case HPF:
                        /* code */
                        if (isPriorityQueueEmpty(&Priority_List_HPF_SJF))
                        {
                            printf("\n from If condition \n");
                            pushIntoPriorityQueue(&Priority_List_HPF_SJF, &process_to_be_recieved.Process_Data);
                            curr_Proc = Priority_List_HPF_SJF.head;
                            break;
                        }
                        pushIntoPriorityQueue(&Priority_List_HPF_SJF, &process_to_be_recieved.Process_Data);
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

            switch (mode)
            {
            case SJF:

                /* code */
                SJF_Algo(semaphore_IDs , follow);
                break;

            case HPF:
                /* code */
                if (!isPriorityQueueEmpty(&Priority_List_HPF_SJF))
                {
                    HPF_Algo(semaphore_IDs , follow);
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
                    Round_Robin(semaphore_IDs , follow);
                }
                else
                {
                    ++cpu_waiting_time;
                    /*Increase the waiting time for schedular*/
                }

                break;

            case MLFL:
                /* code */
                break;

            default:
                break;
            }

            /*Make rec_value = 1 to re-enter the check recieved loop again*/
            rec_value = 1;
        }

        /*IF Schedular finished its job it Terminates*/
        

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
void Round_Robin(int *Process_Semaphore , int Time)
{
    printf("******************************************************************\n");
    printf("State of  %d  is: %s and remaining is %d\n", curr_Proc->Process_Data.Process_ID, states[curr_Proc->Process_Data.State], curr_Proc->Process_Data.Remaining_time);
    printf("******************************************************************\n");

    /*Variable to prevent enter resumed condition if started*/
    static int startt = 0;
    up(Process_Semaphore[curr_Proc->Process_Data.Process_ID - 1]); /*up the current semaphore*/
    calc_Proc_waiting(&circular_Queue_RR, curr_Proc);              /*Function to increase waiting time for not runing processes in ready queue*/

    /*Check if we reach the quantum to switch or Remaining time is zero*/
    if ((quanta == 0 || curr_Proc->Process_Data.Remaining_time == 0))
    {
        if (quanta == 0)
            quanta = q;
        /* ------------ switch to next pr ----------------------- */
        struct Processes_Node *to_delete;
        to_delete = curr_Proc;

        /*Enter only if there is more thaan 1 process*/
        if (circular_Queue_RR.front != circular_Queue_RR.rear)
        {
            curr_Proc->Process_Data.State = STOPPED;
            startt = 0;

            /*Print switch process information*/
            if (curr_Proc->Process_Data.Remaining_time != 0)
            {
                PRINT_CURR_PROCESS(curr_Proc,Time, processess_file);
            }

            curr_Proc = curr_Proc->Next;
           
            

        }

        /*Enter if current process finishes execution*/
        if (to_delete->Process_Data.Remaining_time == 0)
        {
            /*remove the process from ready circular queue*/
            printf("To be removed is: %d and waiting is %d\n", to_delete->Process_Data.Process_ID, to_delete->Process_Data.Waiting_time);

            to_delete->Process_Data.State = FINISHED;
            
            /*Print finish process information*/
            PRINT_CURR_PROCESS(to_delete,Time, processess_file);
            total_waiting_time+= to_delete->Process_Data.Waiting_time;
            total_WTA_time+= to_delete->Process_Data.W_TA;


            remove_From_Circular(&circular_Queue_RR, to_delete->Process_Data.Process_ID);

            if (IsEmpty_Queue(&circular_Queue_RR))
            {
                curr_Proc = NULL;
            }
        }
    }

    /*Enter only if the removed element is not last one in the circular queue*/
    if (curr_Proc != NULL)
    {
        if (curr_Proc->Process_Data.Remaining_time == curr_Proc->Process_Data.Running_time)
        {
            curr_Proc->Process_Data.State = STARTED;
            
            startt = 1;
        }
        else if (startt != 1)
        {
            curr_Proc->Process_Data.State = RESUMED;
        }
        PRINT_CURR_PROCESS(curr_Proc,Time, processess_file);
        --curr_Proc->Process_Data.Remaining_time; /*decrease remaining time for the runing process*/
    }
    --quanta;
    // printf("\n*_*_*_*_**_*_ Current is: %p *_*_*_*_**_*_\n",curr_Proc);
}

/*
******** HPF Algorithm functions *****
*/
void HPF_Algo(int *Process_Semaphore , int Time)
{
    /*Functions to be performed each clk*/
    printf("******************************************************************\n");
    printf("State of  %d  is: %d and remaining is %d\n", Priority_List_HPF_SJF.head->Process_Data.Process_ID, Priority_List_HPF_SJF.head->Process_Data.State, Priority_List_HPF_SJF.head->Process_Data.Remaining_time);
    printf("******************************************************************\n");

    /*Variable to prevent enter resumed condition if started*/
    static int startt = 0;
    up(Process_Semaphore[Priority_List_HPF_SJF.head->Process_Data.Process_ID - 1]); /*up the current semaphore*/

    /*Check if process primited and stopped*/
    if (curr_Proc->Process_Data.Process_ID != Priority_List_HPF_SJF.head->Process_Data.Process_ID)
    {
        // print information for Preemtid Process
        PRINT_CURR_PROCESS(Priority_List_HPF_SJF.head , Time , processess_file);

        curr_Proc = Priority_List_HPF_SJF.head;
    }

    /*Check if remaining time for runing process is zero */
    if (Priority_List_HPF_SJF.head->Process_Data.Remaining_time == 0)
    {
        /*switch to next process and remove the process from ready queue*/
        Priority_List_HPF_SJF.head->Process_Data.State = FINISHED;
        startt = 0;

        /*Print finish process information*/
        PRINT_CURR_PROCESS(Priority_List_HPF_SJF.head , Time , processess_file);
        total_waiting_time+=Priority_List_HPF_SJF.head->Process_Data.Waiting_time;
        total_WTA_time+=Priority_List_HPF_SJF.head->Process_Data.W_TA;

        popFromPriorityQueue(&Priority_List_HPF_SJF);
        // make pointer points to new head
        curr_Proc = Priority_List_HPF_SJF.head;
    }

    if (Priority_List_HPF_SJF.head != NULL)
    {
        /*Set States of runing Process*/
        if (Priority_List_HPF_SJF.head->Process_Data.Remaining_time == Priority_List_HPF_SJF.head->Process_Data.Running_time)
        {
            Priority_List_HPF_SJF.head->Process_Data.State = STARTED;
            startt = 1;
        }
        else if (startt != 1)
        {
            Priority_List_HPF_SJF.head->Process_Data.State = RESUMED;
        }
        calc_Proc_waiting_Prio(&Priority_List_HPF_SJF);            /*Function to increase waiting time for not runing processes in ready queue*/
        --Priority_List_HPF_SJF.head->Process_Data.Remaining_time; /*decrease remaining time for the runing process*/
    }
}


void SJF_Algo(int *Process_Semaphore , int Time)
{
    static int check = 0;
    static bool started = false;
    /*Functions to be performed each clk*/
    //printf("******************************************************************\n");
    // printf("State of  %d  is: %d and remaining is %d\n", Priority_List_HPF_SJF.head->Process_Data.Process_ID, Priority_List_HPF_SJF.head->Process_Data.State, Priority_List_HPF_SJF.head->Process_Data.Remaining_time);
    //printf("******************************************************************\n");
    ///////////////////////////////////////////////////////////////////



    //there is no process to run in the queue;
    
    if(isPriorityQueueEmpty(&Priority_List_HPF_SJF))
    {
    //there is no process to run in the queue;
     if(curr_Proc == NULL) //no_currently_running_process
     {
    /*add_waiting_time*/
     cpu_waiting_time++;
     }
    else if(curr_Proc->Process_Data.State == FINISHED) //Current_Process_is_finished_and_no_process_to_run
    {
        /*add_waiting_time*/
        cpu_waiting_time++;
    }
    else //There is a process running
    {
        //finished_process_count +=RUN_CURR_PROCESS(curr_Proc , Process_Semaphore , &Priority_List_HPF_SJF ,Time, processess_file);
        check =RUN_CURR_PROCESS(curr_Proc , Process_Semaphore , &Priority_List_HPF_SJF ,Time , processess_file);
        finished_process_count += check;
        if(check == 1)
        {
        total_waiting_time+=curr_Proc->Process_Data.Waiting_time;
        total_WTA_time+=curr_Proc->Process_Data.W_TA;
        }
        if(finished_process_count == 8){fclose(processess_file);}
    } 
    }
    else
    {
        /*there is processess in the queue*/
        if(curr_Proc == NULL) //no_currently_running_process aka First_process in the system
        {
        /////////////////////////////////////////////////////////////////////////////////////////
        //allocate a new node and make a copy of the head data since it will be freed after dequeue
        curr_Proc =malloc(sizeof(struct Processes_Node));   
        COPY_then_DEQUEUE_HEAD(curr_Proc , &Priority_List_HPF_SJF);
        /////////////////////////////////////////////////////////////////////////////////////////
        
        /*print_process_start*/
        PRINT_CURR_PROCESS(curr_Proc,Time, processess_file);

        /*run_process*/
        check =RUN_CURR_PROCESS(curr_Proc , Process_Semaphore , &Priority_List_HPF_SJF ,Time , processess_file);
        finished_process_count += check;
        if(check == 1)
        {
        total_waiting_time+=curr_Proc->Process_Data.Waiting_time;
        total_WTA_time+=curr_Proc->Process_Data.W_TA;
        }
        
        //if(finished_process_count == Process_COUNT){/*finish*/}

        }
        else if(curr_Proc->Process_Data.State == FINISHED) //Current_Process_is_finished_so_switch_to_another
        {
        COPY_then_DEQUEUE_HEAD(curr_Proc , &Priority_List_HPF_SJF);
        /*print_process_start*/
        PRINT_CURR_PROCESS(curr_Proc,Time, processess_file);


        check =RUN_CURR_PROCESS(curr_Proc , Process_Semaphore , &Priority_List_HPF_SJF ,Time , processess_file);
        finished_process_count += check;
        if(check == 1) //process_is_finished
        {
        total_waiting_time+=curr_Proc->Process_Data.Waiting_time;
        total_WTA_time+=curr_Proc->Process_Data.W_TA;
        }

        //if(finished_process_count == Process_COUNT){/*finish*/}
        }
        else //There is a process running
        {
        check =RUN_CURR_PROCESS(curr_Proc , Process_Semaphore , &Priority_List_HPF_SJF ,Time , processess_file);
        finished_process_count += check;
        if(check == 1)
        {
        total_waiting_time+=curr_Proc->Process_Data.Waiting_time;
        total_WTA_time+=curr_Proc->Process_Data.W_TA;
        }
        //if(finished_process_count == Process_COUNT){/*finish*/}
        }  
    }


}









/*
******** Handler to remove all semaphores *****
*/
void handler(int signum)
{
    //if(finished_process_count != 8){fclose(processess_file);}
    printf("\nSCh: ana matt with CPU_WT = %d , TOTAL_WTA = %.2f, TOTAL_Processess_WT = %d \n",cpu_waiting_time,total_WTA_time,total_waiting_time);
    for (size_t i = 0; i < 8; i++)
    {
        semctl(semaphore_IDs[i], 0, IPC_RMID, NULL);
    }
    exit(0);
}
