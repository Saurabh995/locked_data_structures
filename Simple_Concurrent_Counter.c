//
//  Simple_Concurrent_Counter.c
//  Implementaion
//
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


typedef struct __counter_t{
    int value;
    pthread_mutex_t lock;
}counter_t;

typedef struct __my_args_t{
    counter_t *counter;
    int threads;
} my_args_t;

void init(counter_t *c){
    c->value =0;
    pthread_mutex_init(&c->lock,NULL);
}
void increment(counter_t *c){
    pthread_mutex_lock(&c->lock);
    c->value++;
    pthread_mutex_unlock(&c->lock);
}
void decrement(counter_t *c){
    pthread_mutex_lock(&c->lock);
    c->value--;
    pthread_mutex_unlock(&c->lock);
}
int get(counter_t *c){
    int value;
    pthread_mutex_lock(&c->lock);
    value = c->value;
    pthread_mutex_unlock(&c->lock);
    return value;
}
void *myfunc(void *args){
    my_args_t *myargs = args;
    for (int i=0;i<ONE_MILLION/myargs->threads;i++){
        increment(myargs->counter);
    }
    pthread_exit(0);
}
int main(){
    for(int i=1;i<5;i++){
        pthread_t threads[i-1];
        struct timeval start, end;
        counter_t *counter = malloc(sizeof(counter_t));
        if(counter == NULL)
            return -1;
        init(counter);
        my_args_t args;
        args.counter = counter;
        args.threads = i;
        gettimeofday(&start, NULL);
        for(int j=0;j<i;j++){
            pthread_create(&threads[j], NULL, &myfunc, &args);
        }
        for(int j=0; j<i;j++){
            pthread_join(threads[j],NULL); 
        }
        gettimeofday(&end, NULL);
        printf("%d threads\n",i);
        printf("Time (seconds): %f\n\n", (float) (end.tv_usec - start.tv_usec + (end.tv_sec - start.tv_sec) * ONE_MILLION) / ONE_MILLION);
        free(counter);
    }
    return 0;
}



