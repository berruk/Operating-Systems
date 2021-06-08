/*  
   BERRU KARAKAŞ
   
                  */

#define _GNU_SOURCE
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

key_t KEYSEM;
key_t KEYSEM2;
key_t KEYSHM;

//  SIGNAL SENDING
void sem_signal(int semid, int val)
{
    struct sembuf semaphore;
    semaphore.sem_num = 0;
    semaphore.sem_op = val;  
    semaphore.sem_flg = 0;   
    semop(semid, &semaphore, 1);
}


//  SIGNAL WAITING
void sem_wait(int semid, int val)
{
    struct sembuf semaphore;
    semaphore.sem_num = 0;
    semaphore.sem_op = (-1*val);  
    semaphore.sem_flg = 0;  
    semop(semid, &semaphore, 1);
}

int main(int argc, char *argv[])
{
    int memid = 0, semap = 0, semac = 0;  //  IDS
    int *array = NULL;  // ARRAY
    int f;  //  FORK RETURN
    int i,process;  //  CHILDREN LABEL
    
  
/************************ INPUT FILE OPERATIONS *******************************/

    //initKeys(argv);
    FILE* in;
    if(argc==1)
         in = fopen("input.txt","r");
    else
        in = fopen(argv[1],"r");
    
     int n,M;

      fscanf (in, "%d", &M);   
      fscanf (in, "%d", &n);   
  
    KEYSEM = ftok("KEY", 'a') + 2;
    KEYSEM2 = ftok("KEY", 'b') + 3;
    KEYSHM = ftok("KEY", 'c') + 4;

/************************ END *******************************/
  
    //CREATION OF CHILDREN
    for (i = 0; i < 2; ++i)
    {
        f = fork();
        if (f < 0)
        {
            printf("ERROR\n");
            exit(1);
        }
        if (f == 0)
            break;
    }
    
//////////////////////////////////PARENT PROCESS OPERATIONS  
  
if (f != 0){
        //SEMAPHORE_1 FOR SYNCHRONIZATION BETWEEN PARENT AND CHILDREN
        semap = semget(KEYSEM2, 1, 0700|IPC_CREAT);
        semctl(semap, 0, SETVAL, 0);
        
        //SEMAPHORE_2 FOR SYNCHRONIZATION BETWEEN CHILDREN
        semac = semget(KEYSEM, 1, 0700|IPC_CREAT);
        semctl(semac, 0, SETVAL, 0);
        
        //SHARED MEMORY WITH SIZE OF THE ARRAY
        memid = shmget(KEYSHM, sizeof(int)*(2*n+4), 0700|IPC_CREAT);
  
        //ATTACH THE MEMORY TO PARENT
        array = (int*)shmat(memid, 0, 0);
        
        /***************** INPUT FILE OPERATIONS *******************************/

        array[0] = n;
        array[1] = M; 
        int c;
        i=0;
         while(1){
           fscanf (in, "%d", &c);       
          array[i+4] = c;
          if(feof(in))
             break;  
          i++;
       }
      
      fclose(in);
  
      /************************ END *******************************/

        //DETACH MEMORY FROM PARENT
        shmdt(array);
    
        //  WAIT FOR 2 CHILDREN
        sem_wait(semap, 2);
        
        //ATTACH MEMORY TO PARENT
       array = (int*)shmat(memid, 0, 0);
      
       /******************* OUTPUT FILE OPERATIONS *******************************/
       FILE *out;
       if(argc==1)
         out = fopen("output.txt","w+");
       else
         out = fopen(argv[1],"w+");
        
      //WRITING ARRAY A
        fprintf(out,"%d\n",array[0]);
        fprintf(out,"%d\n",array[1]);  
      
        for(int j=4;j<n+4;j++)
            fprintf(out,"%d ",array[j]);
        
      //WRITING ARRAY B
        fprintf(out,"\n%d\n",array[2]);       
      
        for(int j=n+4;j<n+4+array[2];j++)
            fprintf(out,"%d ",array[j]);
      
      //WRITING ARRAY C
        fprintf(out,"\n%d\n",array[3]);       

        for(int j=n+4+array[2];j<2*n+4;j++)
            fprintf(out,"%d ",array[j]);
      
      fclose(out);
       /************************ END *******************************/
      
        //DETACH MEMORY
         shmdt(array);
      
        //REMOVE SEMAPHORES AND MEMORY
        semctl(semap, 0, IPC_RMID, 0);
        semctl(semac, 0, IPC_RMID, 0);
        shmctl(memid, IPC_RMID, 0);
        
        exit(0);
    }
  
//////////////////////////////////CHILD PROCESS OPERATIONS  
else{
        
        //GET SEMAPHORE AND MEMORY SPACE ID'S
        semac = semget(KEYSEM, 1, 0);
        semap = semget(KEYSEM2, 1, 0); 
        memid = shmget(KEYSHM, sizeof(int)*(2*n+4), 0);
        
        //ATTACH MEMORY TO CHILD
        array = (int*)shmat(memid, 0, 0);
        
        process = i;

      
      /************************ FIRST CHILD ZONE *******************************/
      if(i==0){
            
            //TRAVERSING THE ARRAY 
          int x=0;
          for (i = 0; i < array[0]; ++i){
            if(array[i+4]<=array[1]) //CONDITION FOR CHILD ONE
                 x++;
              
              //WRITE X & Y TO MEMORY
             array[2] = x; 
             array[3] = array[0] -x;
           }
        
        sem_signal(semac, 1); //SEND SIGNAL TO CHILD TWO

        //WRITING TO ARRAY
        int j = 0;
        for (i = 0; i < array[0]; ++i){
             if(array[i+4]<=array[1]){
                array[4+array[0]+j] = array[4+i];
                  j++;
               
             }
            }

      }
      /****************************  END  *********************************/
      
      
      /********************** SECOND CHILD ZONE *****************************/
        if(process==1){
        sem_wait(semac, 1); //WAIT FOR SIGNAL FROM CHILD ONE
        int j=0;
        for (i = 0; i < array[0]; ++i){
             if(array[i+4]>array[1]){
               array[j+array[2]+array[0]+4] = array[i+4];       
               j++;
             }
           }
         }
      /****************************  END  *********************************/

        
        //DETACH MEMORY FROM CHILDREN
        shmdt(array);
        
        //SIGNAL FOR PARENT
        sem_signal(semap, 1);

        exit(0);
    }
    return 0;
}
