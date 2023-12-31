#ifndef WORKERS_H
#define WORKERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>

#include "settings.h"
#include "errors.h"

typedef struct _Worker
{
    pthread_t id;               
    int channel;                
    sem_t sem;
    pthread_mutex_t mutex;
    char name[64];
} Worker;

Worker *initWorkerArray(void *threadWorker(void *), int size);
int indexFreeWorker(Worker* worker_array);
int activeWorkerCount(Worker* worker_array);


#endif