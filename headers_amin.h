#include "main_header.h"

/*****************************************************************************/

/**************************** Functions Definitions **************************/

/* Function to Read from file and insert it into linked list*/
void Read_file(Process_List* LinkedList,char* FileName)
{
    Process* proc = (Process*)malloc(sizeof(Process));
    FILE *pFile;
    pFile=fopen(FileName,"r");
    char line[256];
    while(fgets(line, sizeof(line), pFile))
    {
        if(line[0]=='#')
        {
            continue;
        }
        /*set process parameters*/
        proc->Process_ID=atoi(line[0]);
        proc->Arrival_time=atoi(line[2]);
        proc->Running_time=atoi(line[4]);
        proc->Remaining_time=proc->Running_time;
        proc->Priority=atoi(line[6]);
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






