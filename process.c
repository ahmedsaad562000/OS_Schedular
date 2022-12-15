#include "headers.h"
#include "Semaphore.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char *argv[])
{
    initClk();
    int x = getClk();
    int follow = x;
    int pid = atoi(argv[1]);
    remainingtime = atoi(argv[2]);
    union Semun semun; /*Semaphpre union*/
    key_t sID;
    int sem;

    printf("Process %d: Current Time is %d\n", pid, x);

    /*Create semaphore for each recieved process*/
    sem = semget(pid, 1, 0666 | IPC_CREAT);
    if (sem == -1)
    {
        perror("Error in create sem");
        exit(-1);
    }
    semun.val = 0;
    if (semctl(sem, 0, SETVAL, semun) == -1)
    {
        perror("Error in semctl");
        exit(-1);
    }

    // TODO The process needs to get the remaining time from somewhere
    // remainingtime = ??;
    while (remainingtime > 0)
    {
        x = getClk();
        // printf("\nX is %d and follow is: %d \n", x, follow);
        /*Enter loop each one second only*/
        if (follow != x)
        {
            follow = x;
            down(sem);
            --remainingtime;
            printf("------------------------------------------------------------\n");
            printf("Process %d: Semaphore %d Remaining is %d\n", pid, sem,remainingtime);
            printf("Process %d: Clock is: %d\n", pid, getClk());
            printf("------------------------------------------------------------\n");
        }
    }
    
    destroyClk(false);

    return 0;
}
