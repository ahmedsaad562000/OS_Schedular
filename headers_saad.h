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

void PRINT_MEMORY_ALLOC(Process *curr_Proc, int Time, FILE *processess_file)
{
    char line_to_print[256];
    int start = curr_Proc->memindex * 32; 
    int end = ((curr_Proc->memindex + curr_Proc->no_of_blocks)*32 -1);
    snprintf(line_to_print, 200, "At time %d allocated %d bytes %d for process %d from %d to %d", Time, curr_Proc->memsize ,curr_Proc->Process_ID,start , end); // puts string into buffer
    strcat(line_to_print, "\n");
    // printf(line_to_print);
    fputs(line_to_print, processess_file); // writing data into file
}

void PRINT_MEMORY_DEALLOC(Process *curr_Proc, int Time, FILE *processess_file)
{
    char line_to_print[256];
    int start = curr_Proc->memindex * 32; 
    int end = ((curr_Proc->memindex + curr_Proc->no_of_blocks)*32 -1);
    snprintf(line_to_print, 200, "At time %d freed %d bytes %d from process %d from %d to %d", Time, curr_Proc->memsize ,curr_Proc->Process_ID,start , end); // puts string into buffer
    strcat(line_to_print, "\n");
    // printf(line_to_print);
    fputs(line_to_print, processess_file); // writing data into file
}



void COPY_then_DEQUEUE_HEAD(struct Processes_Node *curr_Proc, Process_List *P_Queue)
{
    curr_Proc->Process_Data = P_Queue->front->Process_Data;
    popFromQueue(P_Queue);
    curr_Proc->Process_Data.State = STARTED;
}



int get_no_of_blocks(int mem_size)
{
    if(mem_size<32){return 1;}
    int size = ceil(log2((double)mem_size));
    printf("------the size is %d---------" , size);
    return (pow(2,size) / 32);
    
}


int check_first_empty_location_if_odd(bool* mem)
{
for (int i = 1; i < 32; i = i+1)
{
    if (mem[i-1] == true && (i%2)==1 && mem[i] == false)
    {
        mem[i]=true;
        return i;
    }
}
return -1;
}

int get_first_place(bool* mem  ,int blocks_to_reserve)
{
    printf("\nI'm in\n");
bool check = true;
for (int i = 0; i < 32; i = i+blocks_to_reserve)
{
    check = true;
    for (int k = i; k < i+blocks_to_reserve; k++) //check if this place vacant
    {
        if (mem[k] == true){check=false;break;}
    }
    //found
    if(check)
    {
    for (int k = i; k < i+blocks_to_reserve; k++) //check if this place vacant
    {
        mem[k] = true;
    }
        return i;
    }

}

}

int mem_alloc(bool* mem , int blocks_to_reserve)
{
 int op1= 32; //mt7wata mn el na7itin
 int op2 = 32; //mt7awta mn wra bs

 bool check = true;
 bool check2 = true;
 bool check4 = true;
 bool check5 = true;
 int blocks = blocks_to_reserve;

/*if(blocks_to_reserve==1)
    {
    int check3 = check_first_empty_location_if_odd(mem);
    if(check3 != -1){ return check3; }
    }*/


//while (blocks != 32)
//{
    /* code */


    /*if(i==0){blocks = blocks_to_reserve;i++;}
    else{blocks = blocks*2;i++;}*/
    if(blocks == 16){return get_first_place(mem ,blocks_to_reserve);}
    check2 = false;

    for ( int j = 0; j < 32; j = j+blocks) //try to find empty (blocks_to_reserve) which is reserved from both sides
    {
        check = true;
        for (int k = j; k < j+blocks; k++) //check if this place vacant
        {
             if (mem[k] == true){check=false;break;}
        }
        
        if(check)//found empty space now check if it's reserved from both sides
        {
            check2 = true;
            if (j==0) //the empty place is at first place in array -> check it's left only 
            {
                            
                check4 = true;
                for (int k = j+blocks; k < j+blocks+blocks; k++) //check if this place vacant
                {
                    if (mem[k] == true){check4=false;break;}
                }

                 if(check4==false)//found
                {

                    for (int k = j; k < j+blocks_to_reserve; k++) //reserve
                        {
                            mem[k] = true;
                        }        
                    return j;
                }
                
            }
            else if (j>0 && j<32-blocks)
            {
                check4 = true;
                for (int k = j+blocks; k < j+blocks+blocks; k++) //check if this place vacant
                {
                    if (mem[k] == true){check4=false;break;}
                }
                check5 = true;
                for (int k = j-blocks; k < j; k++) //check if this place vacant
                {
                    if (mem[k] == true){check5=false;break;}
                }

                if(check4==false && check5==false)//found
                {
                    for (int k = j; k < j+blocks_to_reserve; k++) //check if this place vacant
                        {
                            mem[k] = true;
                        }
                    return j;
                }
                else if (check4==false)
                {
                    op1 = (j<op1)?j:op1;
                }
                else if (check5==false)
                {
                    op2 = (j<op2)?j:op2;
                } 

            }
            else if (j == 32-blocks) //the empty place is at second place in array -> check it's right only 
            {
                check4 = true;
                for (int k = j-blocks; k < j; k++) //check if this place vacant
                {
                    if (mem[k] == true){check4=false;break;}
                }

                

                if(check4==false)//found
                {
                    for (int k = j; k < j+blocks_to_reserve; k++) //reserve
                        {
                            mem[k] = true;
                        }                
                    return j;
                }  
            }



        }
    }
    if(!check2) //didnt find any space 
    {return -1;}

    if(op1==32 && op2==32){return get_first_place(mem ,blocks_to_reserve);}
    else
        { int best =  (op1>op2)?op2:op1;
            for (int i = best; i < best+blocks_to_reserve; i++)
            {
                mem[i]=true;
            }
            return best;
        }
    /* code */
//}




}

void mem_dealloc(bool* mem, int blocks_to_free , int index)
{
    for (int i = index; i < index+blocks_to_free; i++)
        {
            mem[i]=false;
        }
}