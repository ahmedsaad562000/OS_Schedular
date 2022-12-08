#include "main_header.h"

/*****************************************************************************/

/**************************** Functions Definitions **************************/

/* Function to Read from file and insert it into linked list*/
void Read_file(Process_List* LinkedList,char* FileName)
{
    Process* proc = (Process*)malloc(sizeof(Process));
    FILE *pFile;
    pFile=fopen("processes.txt","r");
    char line[256];
    int ff[4];
    while (fgets(line,sizeof(line),pFile))
    {
        if(line[0]=='#')
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
        proc->Process_ID=ff[0];
        proc->Arrival_time=ff[1];
        proc->Running_time=ff[2];
        proc->Remaining_time=proc->Running_time;
        proc->Priority=ff[3];
        proc->Waiting_time=0;
        proc->TA=0;
        proc->W_TA=0;
        proc->State=STOPPED;

        /*Insert process into linked list*/
        Insert_Process(LinkedList,proc);
    }
    /*Close file*/
    fclose(pFile);
}






