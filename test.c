#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
typedef short bool;
#define true 1
#define false 0
//#include "main_header.h"
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
 int op3 = 32; //mt7awta mn 2odam bs
 bool check = true;
 bool check2 = true;
 bool check4 = true;
 bool check5 = true;
 int blocks = blocks_to_reserve;
int i = 0;

if(blocks_to_reserve==1)
    {
    int check3 = check_first_empty_location_if_odd(mem);
    if(check3 != -1){ return check3; }
    }


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

int main()
{
    int ex[6];
    ex[0] = 128;
    ex[1] = 256;
    ex[2] = 512;
    ex[3] = 256;
    ex[4] = 128;
    ex[5] = 16;  
    
    int indexes[6] = {-1};


    bool mem[32] = {false};
    // mem[0] =1;
    // mem[1] =1;
    // mem[2] =1;
    // mem[3] =1;
    // mem[4] =1;
    // mem[5] =1;
    // mem[0] =1;
    // mem[0] =1;
    // mem[0] =1;
   /* for ( int i = 0; i < 6; i++)
    {*/
    int no_blocks[6];
        no_blocks[0] = get_no_of_blocks(ex[0]);
        //printf("\nex[%d] requires %d blocks\n" , i ,no_blocks);
        
        indexes[0] = mem_alloc(mem ,no_blocks[0]);
        printf("memory content is: \n");
        for (int j = 0; j < 32; j++)
        {
            printf("%d ", mem[j]);
        }
        printf("\n");

//////////////////////////////////////////////////////////////////////////////////////
        no_blocks[1] = get_no_of_blocks(ex[1]);
        //printf("\nex[%d] requires %d blocks\n" , i ,no_blocks);
        
        indexes[1] = mem_alloc(mem ,no_blocks[1]);
        printf("memory content is: \n");
        for (int j = 0; j < 32; j++)
        {
            printf("%d ", mem[j]);
        }
        printf("\n");
/////////////////////////////////////////////////////////////////
        no_blocks[2] = get_no_of_blocks(ex[2]);
        //printf("\nex[%d] requires %d blocks\n" , i ,no_blocks);
        
        indexes[2] = mem_alloc(mem ,no_blocks[2]);
        printf("memory content is: \n");
        for (int j = 0; j < 32; j++)
        {
            printf("%d ", mem[j]);
        }
        printf("\n");
//////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
        no_blocks[3] = get_no_of_blocks(ex[3]);
        //printf("\nex[%d] requires %d blocks\n" , i ,no_blocks);
        
        indexes[3] = mem_alloc(mem ,no_blocks[3]);
        printf("memory content is: \n");
        for (int j = 0; j < 32; j++)
        {
            printf("%d ", mem[j]);
        }
        printf("\n");
//////////////////////////////////////////////////////////////////
for (int i = indexes[2]; i < indexes[2]+no_blocks[2]; i++)
{
    mem[i]=false;
}
 printf("memory content is: \n");
        for (int j = 0; j < 32; j++)
        {
            printf("%d ", mem[j]);
        }
        printf("\n");
//////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
        no_blocks[4] = get_no_of_blocks(ex[4]);
        //printf("\nex[%d] requires %d blocks\n" , i ,no_blocks);
        
        indexes[3] = mem_alloc(mem ,no_blocks[4]);
        printf("memory content is: \n");
        for (int j = 0; j < 32; j++)
        {
            printf("%d ", mem[j]);
        }
        printf("\n");
//////////////////////////////////////////////////////////////////

    
    



return 0;
}