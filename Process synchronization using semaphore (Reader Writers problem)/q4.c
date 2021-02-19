#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
sem_t wrt;
pthread_mutex_t mutex,waitng_lock;
int data = 1;
int numreader = 0;
int waiting = 0;

void *writer(void *wno)
{   
    //for waiting on write process count
    pthread_mutex_lock(&waitng_lock);
    waiting++;
    printf("\nwaiting writer %d at waiting list:%d\n",*((int *)wno),waiting);
    pthread_mutex_unlock(&waitng_lock);

    sem_wait(&wrt);
    waiting--;
    printf("\nWaitng list:%d",waiting);
    data = data*2;
    printf("\nWriter %d modified data to %d\n",(*((int *)wno)),data);
    sem_post(&wrt);
}
void *reader(void *rno)
{   
   // if writer process is waiting
    while(waiting > 0);
     // Reader acquire the lock before modifying numreader
    pthread_mutex_lock(&mutex);
    numreader++;
    if(numreader == 1) {
        sem_wait(&wrt); // If this id the first reader, then it will block the writer
    }
    pthread_mutex_unlock(&mutex);
    // Reading Section
    printf("Reader %d: read data as %d\n",*((int *)rno),data);

    // Reader acquire the lock before modifying numreader
    pthread_mutex_lock(&mutex);
    numreader--;
    if(numreader == 0) {
        sem_post(&wrt); // If this is the last reader, it will wake up the writer.
    }
    pthread_mutex_unlock(&mutex);
}
int max(int x,int y){
    if(x>y) return x;
    else return y;
}
int main()
{   
    int reader_process,writer_process;
    printf("Enter number of reader process:");
    scanf("%d",&reader_process);
    printf("\nEnter number of writer process:");
    scanf("%d",&writer_process);
    pthread_t read[reader_process],write[writer_process];
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&waitng_lock,NULL);
    sem_init(&wrt,0,1);
    int a[max(reader_process,writer_process)];
    for(int i=0;i<max(reader_process,writer_process);i++)
        a[i] = i+1;
    int maxitem = max(reader_process,writer_process);
    for(int i = 0; i < maxitem; i++) {
        if(i<reader_process)
        pthread_create(&read[i], NULL, (void *)reader, (void *)&a[i]);
        if(i<writer_process)
        pthread_create(&write[i], NULL, (void *)writer, (void *)&a[i]);
    }
    for(int i = 0; i < reader_process; i++) {
        pthread_join(read[i], NULL);
    }
    for(int i = 0; i < writer_process; i++) {
        pthread_join(write[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&wrt);
    pthread_mutex_destroy(&waitng_lock);
    return 0;
}