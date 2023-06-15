#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "errors.h"
#include "workers.h"
#include "settings.h"
#include "resolv.h"

void *threadWorker(void *arg);
char *stringIP(uint32_t entierIP);
int indexFreeWorker(Worker* workers);

int descriptor_logs;
sem_t sem_free_workers;

int main(int argc, char const *argv[])
{
    /* Variables */
    int descriptor_socket, channel, status, address_length, index_free_worker;
    struct sockaddr_in bind_address, client_address;
    Worker *worker_array;

    /* Main args */
    if (argc != 2)
    {
        error("Usage: %s port\n", argv[0]);
    }

    short port = (short)atoi(argv[1]);

    /* Logging */
    descriptor_logs = open(LOG_NAME, O_CREAT | O_WRONLY | O_APPEND, 0600);
    if (descriptor_logs == -1)
        error_IO("Logging");

    /* Worker cration */
    worker_array = initWorkerArray(threadWorker, NB_WORKERS);

    status = sem_init(&sem_free_workers, 0, NB_WORKERS); // sem_free_workers is a global variable
    if (status != 0)
        error_IO("Semaphore init");

    printf("Server : creating a socket\n");
    descriptor_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (descriptor_socket < 0)
        error_IO("socket");

    /* Initialising server */
    bind_address.sin_family = AF_INET;
    bind_address.sin_addr.s_addr = INADDR_ANY;
    bind_address.sin_port = htons(port);
    printf("Server : binding to INADDR_ANY address on port %d\n", port);
    status = bind(descriptor_socket, (struct sockaddr *)&bind_address, sizeof(bind_address));
    if (status < 0)
        error_IO("Bind");

    printf("Server : listening to socket\n");
    status = listen(descriptor_socket, WAIT_QUEUE_SIZE);
    if (status < 0)
        error_IO("Listen");

    /* Infinite loop */
    while ("feur")
    {
        printf("Server : accepting connections \n");
        address_length = sizeof(client_address);
        channel = accept(descriptor_socket, (struct sockaddr *)&client_address, &address_length);
        if (channel < 0)
            error_IO("Accept");

        printf("Client connected (adress : %s, port: %hu) \n", 
            stringIP(ntohl(client_address.sin_addr.s_addr)), 
            ntohs(client_address.sin_port));

        sem_wait(&sem_free_workers);
        if (status != 0)
            error_IO("Semaphore wait");

        index_free_worker = indexFreeWorker(worker_array);
        if(index_free_worker == -1)
            error("Error : no available worker \n");
        worker_array[index_free_worker].channel = channel;
        status = sem_post(&(worker_array[index_free_worker].sem));
        if(status < 0)
            error_IO("Semaphore post");
    }

    if(close(descriptor_socket) == -1)
        error_IO("Close socket");

    if(close(descriptor_logs) == -1)
        error_IO("Close logs");

    return 0;
}

void clientSession(int channel)
{
    int ended = 0;
    char line[LINE_SIZE];
    int status;

    while(!ended)
    {
        // add things here
    }

}

void *threadWorker(void *arg)
{
    Worker *worker_ptr = (Worker *)arg;
    int status;
    while (1)
    {
        status = sem_wait(&(worker_ptr->sem));
        if (status != 0)
            error_IO("Semaphore wait");

        printf("A client joined \n");
        clientSession(worker_ptr->channel);
        printf("A client left \n");
        worker_ptr->channel = -1;

        status = sem_post(&sem_free_workers);
        if (status != 0)
            error_IO("Semaphore post");
    }

    pthread_exit(NULL);
}