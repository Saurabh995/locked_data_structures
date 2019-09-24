//
//  Scalable_Approximate_counter
//  Implementaion
//  @razor123
//  Created by Saurabh Shukla on 21/09/19.
//  Copyright © 2019 Saurabh Shukla. All rights reserved.
//


#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#define ONE_MILLION 1000000
#define NUMCPU 4
int threads[NUMCPU];
typedef struct __counter_t {
    int             global;
    pthread_mutex_t glock;
    int             local[NUMCPU];
    pthread_mutex_t llock[NUMCPU];
    int             threshold;
} counter_t;

typedef struct __myargs_t{
    counter_t *c;
    int amt;
    int threadno;
} myargs_t;

void init(counter_t *c, int t){
    c->global = 0;
    c->threshold = t;
    pthread_mutex_init(&c->glock, NULL);
    for(int i=0;i<NUMCPU; i++){
        c->local[i]=0;
        pthread_mutex_init(&c->llock[i], NULL);
    }
}

int get(counter_t *c ) {
    pthread_mutex_lock(&c->glock);
    int value = c->global;
    pthread_mutex_unlock(&c->glock);
    return value;
}

void update(counter_t *c, int threadId, int amt){
    int thread =0;
    for(int i =0 ;i <NUMCPU;i++){
        if(threadId == threads[i]){
            thread=i;
            break;
        }
    }
    pthread_mutex_lock(&c->llock[thread]);
    c->local[thread]+=amt;
    if(c->local[thread] >=c->threshold){
        pthread_mutex_lock(&c->glock);
        c->global+=c->local[thread];
        pthread_mutex_unlock(&c->glock);
        c->local[thread]=0;
    }
    pthread_mutex_unlock(&c->llock[thread]);
}

void *thread_func(void *args){
    myargs_t *myargs = (myargs_t*)args;
    pthread_t threadId = pthread_self();
    for(int i=1;i<=ONE_MILLION/myargs->threadno;i++){
        update(myargs->c, (int)threadId, myargs->amt);
    }
    pthread_exit(0);
}

int main(){
    for(int i=1; i<=5;i++){
        int threshold=i;
        for(int j=1;j<=NUMCPU;j++){
            counter_t *c =(counter_t*)malloc(sizeof(counter_t));
            init(c, threshold);
            myargs_t args;
            args.amt=1;
            args.threadno = j;
            args.c = c;
            struct timeval start,end;
            pthread_t threads[j];
            gettimeofday(&start, NULL);
            for(int k=0; k<j; k++){
                pthread_create(&threads[k], NULL, &thread_func, &args);
            }
            for( int k =0; k<j ;k++)
            {
                pthread_join(threads[k],NULL);
            }
            gettimeofday(&end, NULL);
            printf("%d threads, %d threshold\n", j, threshold);
            printf("Time (seconds): %f\n\n", (float) (end.tv_usec - start.tv_usec + (end.tv_sec - start.tv_sec) * ONE_MILLION) / ONE_MILLION);
            free(c);
        }
    }
}



