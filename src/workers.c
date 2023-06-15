#include "workers.h"

Worker *initWorkerArray(void *threadWorker(void *) ,int size)
{
    int status;
    Worker *worker_array = (Worker *)malloc(size * sizeof(Worker));

    // init each worker
    for (int i = 0; i < size; i++)
    {
        worker_array[i].channel = -1;

        status = sem_init(&worker_array[i].sem, 0, 0);
        if (status != 0)
            error_IO("Semaphore init");

        status = pthread_create(&worker_array[i].id, NULL, threadWorker, &worker_array[i]);
        if (status != 0)
            error_IO("Worker init");
    }
}

// Returns the index of a free worker, or -1 if there isn't one
int indexFreeWorker(Worker* worker_array)
{
    for(int i = 0; i < NB_WORKERS; i++)
    {
        if(worker_array[i].channel == -1)
        {
            return i;
        }
    }
    return -1;
}