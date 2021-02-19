#include  <stdio.h>
#include  <pthread.h>

#include  "buf-m-2.h"
#include <semaphore.h>
#define   MAX_ITEMS    20
#define   MAX_THREADS   5
#define   EOD          -1

pthread_mutex_t ScreenLock;

int       Max_Run;                       /* max # of deposit/withdraw*/

/* ----------------------------------------------------------------- */
/* FUNCTION  Fill()                                                  */
/*    This function fills a char array with 'n' spaces.              */
/* ----------------------------------------------------------------- */

void  Fill(char x[], int n)
{
     int  i;

     for (i = 0; i < n*2; i++)
          x[i] = ' ';
     x[i] = '\0';
}

/* ----------------------------------------------------------------- */
/* FUNCTION  Producer()                                              */
/*    This is the producer thread.  It sends out i*10000_j into the  */
/* buffer, where i is the producer number and j is in the range of   */
/* 0 and Max_Run-1.  After this, it sends in EOD.                    */
/* ----------------------------------------------------------------- */

void *Producer(void *voidPTR)
{
     int   *intPTR = (int *) voidPTR;
     int   ID      = *intPTR;
     int   i, data;
     char  filler[100];

     Fill(filler, ID);
     mutex_lock(&ScreenLock);
          printf("%sProducer %d started ...\n", filler, ID);
     mutex_unlock(&ScreenLock);

     for (i = 0; i < Max_Run; i++) {     /* for each iteration       */
          thr_yield();                   /* rest for unspecified time*/
          data = ID*10000 + i;           /* generate a data item     */
          PUT(data);
          mutex_lock(&ScreenLock);       /* lock the screen          */
               printf("%sProducer %d has added %d to the buffer\n",
                       filler, ID, data);/* display a message        */
          mutex_unlock(&ScreenLock);     /* release the screen       */

     }

     thr_yield();                        /* rest for unspecified time*/
     PUT(EOD);
     mutex_lock(&ScreenLock);            /* lock the screen          */
          printf("%sProducer %d adds EOD and exits\n", filler, ID);
     mutex_unlock(&ScreenLock);          /* release the screen       */
     thr_exit(0);
}

/* ----------------------------------------------------------------- */
/* FUNCTION  Consumer()                                              */
/*    This is the consumer thread.  It keeps taking a data item from */
/* the buffer until the EOD mark is seen.                            */
/* ----------------------------------------------------------------- */

void *Consumer(void *voidPTR)
{
     int   *intPTR = (int *) voidPTR;
     int   ID      = *intPTR;
     int   i, data;
     char  filler[100];

     Fill(filler, ID+10);
     mutex_lock(&ScreenLock);
          printf("%sConsumer %d started ...\n", filler, ID);
     mutex_unlock(&ScreenLock);

     do {                                /* iterate until EOD        */
          thr_yield();                   /* rest for unspecified time*/
          mutex_lock(&ScreenLock);       /* lock the screen          */
               printf("%sConsumer %d is waiting for an item\n",
                       filler, ID);      /* display a message        */
          mutex_unlock(&ScreenLock);     /* release the screen       */

          data = GET();
          if (data != EOD) {
               mutex_lock(&ScreenLock);
                    printf("%sConsumer %d has taken %d from the buffer\n",
                           filler, ID, data);      /* display data   */
               mutex_unlock(&ScreenLock);/* release the buffer       */
          }
     } while (data != EOD);              /* do until EOD is seen     */

     mutex_lock(&ScreenLock);
          printf("%sConsumer %d receives EOD and exits\n", filler, ID);
     mutex_unlock(&ScreenLock);
     thr_exit(0);
}

/* ----------------------------------------------------------------- */
/*                        The main program                           */
/* ----------------------------------------------------------------- */

void  main(int argc, char *argv[])
{
     pthread_t   pID[MAX_THREADS];        /* producer ID              */
     pthread_t   cID[MAX_THREADS];        /* consumer ID              */
     size_t     pStatus[MAX_THREADS];    /* producer status          */
     size_t     cStatus[MAX_THREADS];    /* consumer status          */
     int        pArg[MAX_THREADS];       /* producer argument        */
     int        cArg[MAX_THREADS];       /* consumer argument        */
     int        Threads;                 /* # of producers/consumers */
     int        i;

     if (argc != 3)  {
          printf("Use %s #-of-iterations #-of-producers/consumers\n", argv[0]);
          exit(0);
     }
     Max_Run = abs(atoi(argv[1]));
     Threads = abs(atoi(argv[2]));
     if (Threads > MAX_THREADS) {
          printf("The no. of producers/consumers is too large.  Reset to %d\n",
                 MAX_THREADS);
          Threads = MAX_THREADS;
     }

     printf("Parent started ...\n");

     pthread_mutex_init(&ScreenLock,(void *) NULL);
     BufferInit();

     for (i = 0; i < Threads; i++) {     /* start consumers first    */
          cArg[i] = i + 1;
          thr_create(NULL, 0, Consumer, (void *) &(cArg[i]),
                     0, (void *) &(cID[i]));
     }
     for (i = 0; i < Threads; i++) {     /* followed by producers    */
          pArg[i] = i + 1;
          thr_create(NULL, 0, Producer, (void *) &(pArg[i]),
                     0, (void *) &(pID[i]));
     }

     for (i = 0; i < Threads; i++)       /* wait producers/consumers */
          thr_join(cID[i], 0, (void *) &(cStatus[i]));
     for (i = 0; i < Threads; i++)
          thr_join(pID[i], 0, (void *) &(pStatus[i]));

     printf("Parent exits ...\n");
}
