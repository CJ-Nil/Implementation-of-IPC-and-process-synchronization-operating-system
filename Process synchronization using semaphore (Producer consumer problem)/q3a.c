#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))
typedef struct product{
    int id;
    int pid;
    int payload;
}product;
sem_t empty;
sem_t full;
int in = 0;
int out = 0;
int MaxItems; // Maximum items a producer can produce or a consumer can consume
int BufferSize; // Size of the buffer
product ptable[20];
product buffer[20];
pthread_mutex_t mutex;
int pt=0;
void *produce(void *producerno)
{   
    int pid = *((int *)producerno);
    int id = 10*pid;
    int item;
    for(int i = 0; i < MaxItems; i++) 
    {
        item = 1000+(rand()%9000);
        product p;
        p.id = id;
        p.payload = item;
        p.pid = pid;
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        ptable[pt++]=p;
        buffer[in] = p;
        id++;
        in = (in+1)%BufferSize;
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}
void *consume(void *consumerno)
{  
     int cid = *((int *)consumerno); 
    for(int i = 0; i < MaxItems; i++) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        product p = buffer[out];
        printf("%-10d\t\t %-10d\t\t %-10d\t %-12d\n\n",p.id,p.pid,cid,p.payload);
        out = (out+1)%BufferSize;
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
}

int main()
{   
    int producerNo,consumerNo;
    printf("Enter number Producer and Cosumer:");
    scanf("%d%d",&producerNo,&consumerNo);
    printf("Enter buffer size:");
    scanf("%d",&BufferSize);
    printf("Enter no of product wants to produced:");
    scanf("%d",&MaxItems);
    pthread_t pro[5],con[5];
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty,0,BufferSize);
    sem_init(&full,0,0);
    int p[producerNo],c[consumerNo];
    for(int i=0;i<producerNo;i++)
    p[i]=i+1;
    for(int i=0;i<consumerNo;i++)
    c[i]=i+1;

    gotoxy(0, 10);
    printf("\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2 COSUMER TABLE  \xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2");

    gotoxy(0, 11);
    printf("========================================================================================");

    gotoxy(0, 12);
    printf("Product ID\t\t Producer ID\t\t Consumer ID\t Product Payload\n");

    gotoxy(0, 13);
    printf("========================================================================================");
    gotoxy(0, 15);

    

    for(int i = 0; i < producerNo; i++) {
        pthread_create(&pro[i], NULL, (void *)produce, (void *)&p[i]);
    }
    for(int i = 0; i < consumerNo; i++) {
        pthread_create(&con[i], NULL, (void *)consume, (void *)&c[i]);
    }

    for(int i = 0; i < producerNo; i++) {
        pthread_join(pro[i], NULL);
    }
    for(int i = 0; i < consumerNo; i++) {
        pthread_join(con[i], NULL);
    }
    gotoxy(100,10);
    printf("\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2 PRODUCER TABLE  \xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2");

    gotoxy(100,11);
    printf("=========================================================================================");

    gotoxy(100,12);
    printf("Product ID\t\t Producer ID\t\t Product Payload\n");

    gotoxy(100,13);
    printf("=========================================================================================");
   // gotoxy(100,11);
    for(int i=0;i<producerNo*MaxItems;i++){
        product p = ptable[i];
        gotoxy(100,14+(i*2));
        printf("%-10d\t\t %-10d\t\t %-10d\n\n",p.id,p.pid,p.payload);
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
    
}