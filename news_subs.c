#include <pthread.h> //Threads
#include <semaphore.h> //Semaphores
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <threads.h> //thrd_current()

//Set number of sources and subscribers
const int subscribers = 4;
#define subscribers 4
#define news_sources 4

sem_t exc, new;
pthread_mutex_t mtx;
sem_t waiter[subscribers];

int current_reader=subscribers, data=0;


void *read_news(void *ptr){ 
    
    int index = *((int *)ptr); //Get semaphore id
    int sub_name = thrd_current()%1000; //Get sub id

      //Each sub will request each publishers data
      for(int i=0;i<news_sources;i++){
      
      printf("Subscriber %d requests %d\n",sub_name,i+1);

      sem_wait(&waiter[index]); //Wait for a publish

      //Reader gets mutex for operation
      pthread_mutex_lock(&mtx);

      current_reader--; //Amount of subs who received the current publish
          
      //Reader releases mutex, simultaneous reading is allowed  
      pthread_mutex_unlock(&mtx);


    /******𝙧𝙚𝙖𝙙𝙞𝙣𝙜*****/
      
      printf("Subscriber %d recieved %d\n",sub_name,data);
      
    /*********************/

      //Reader gets mutex for operations
      pthread_mutex_lock(&mtx);

      // All subs received
      if(current_reader == 0){
          current_reader = subscribers; //Set for next publish
          sem_post(&new); //All read, new publisher is allowed
      }
      
      //Reader releases mutex for operations
      pthread_mutex_unlock(&mtx);

    }
}

void *publish(void *ptr){
  
    int id = thrd_current()%1000; //Get publisher id
    printf("PUBLISHER %d requests \n",id);
  
    sem_wait(&new); //Waiting all readers to finish
    sem_wait(&exc); //Waiting any other publisher
  

    /******𝙥𝙪𝙗𝙡𝙞𝙨𝙝𝙞𝙣𝙜*****/
  
          data++;
 
    /*********************/

    printf("***FRESH COPIES*** Source %d published %d\n",id,data);
    
    //Incrementing semaphore the amount of subs
    for(int i=0;i<subscribers;i++){
        sem_post(&waiter[i]);
    }
      
    sem_post(&exc); //Allowing other publishers

}

int main(){   
    
    //Array of threads for subscribes and sources
    pthread_t sub[subscribers],sor[news_sources];

    int i; 
    //Initialize semaphores
    sem_init(&exc,0,1);
    sem_init(&new,0,1);
    for(i=0;i<subscribers;i++)
        sem_init(&waiter[i],0,0);

    pthread_mutex_init(&mtx, NULL);

    int name[subscribers];
  

    //Create threads
    for(i=0;i<subscribers;i++){
      name[i] = i;
      pthread_create(&sub[i], NULL, (void *)read_news,(void *)&name[i]);}
  
    for(i=0;i<news_sources;i++)
    pthread_create(&sor[i], NULL, (void *)publish, "");   
    
    //Running threads
    for(i=0;i<news_sources;i++)
    pthread_join(sor[i], NULL);
    
    for(i=0;i<subscribers;i++) 
    pthread_join(sub[i], NULL);

  
    //Destroy semaphores
    sem_destroy(&exc);
    sem_destroy(&new);
    for(i=0;i<subscribers;i++)
        sem_destroy(&waiter[i]);

    pthread_mutex_destroy(&mtx);

    return 0;
    
}