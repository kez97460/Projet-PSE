#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "errors.h"
#include "workers.h"
#include "settings.h"
#include "resolv.h"
#include "ligne.h"
#include "string_utils.h"
#include "files.h"

/* Functions */
void *threadWorker(void *arg);
char *stringIP(uint32_t entierIP);
void sendToAll(Worker* worker_array, char *message);
void clientSession(Worker* worker_ptr);
void *auctionThread(void *arg);


/* Global variables */
int descriptor_logs;
sem_t sem_free_workers;
Worker *worker_array;
Product_list* product_list;
char password[50];
int auction_started;

Product current_product;
char current_product_owner[100];

int main(int argc, char const *argv[])
{
    /* Variables */
    int descriptor_socket, channel, status, address_length, index_free_worker;
    struct sockaddr_in bind_address, client_address;
    worker_array = initWorkerArray(threadWorker, NB_WORKERS);

    // readTxt(product_list, FILENAME);

    Product p1;
    strcpy(p1.title, "La fatigue de Kezia");
    strcpy(p1.desc, "...");
    p1.price = 1;

    Product p2;
    strcpy(p2.title, "Un pipou");
    strcpy(p2.desc, "Très recherché pendant la Coupe de France de Robotique");
    p2.price = 100;

    Product p3;
    strcpy(p3.title, "AAAAA");
    strcpy(p3.desc, "Oui.");
    p3.price = 10;

    product_list = NULL;
    addProduct(&product_list, &p1);
    addProduct(&product_list, &p2);
    addProduct(&product_list, &p3);

    if(product_list != NULL)
        printData(product_list); // Is this the definition of insanity ?

    auction_started = 0;

    /* Main args */
    if (argc != 2)
    {
        error("Usage: %s port\n", argv[0]);
    }

    short port = (short)atoi(argv[1]);

    /* Logging */
    emptyFile(LOG_NAME);
    descriptor_logs = open(LOG_NAME, O_CREAT | O_WRONLY | O_APPEND, 0600);
    if (descriptor_logs == -1)
        error_IO("Logging");

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

//--------------------------------------------------------------------------------
void clientSession(Worker* worker_ptr)
{
    int ended = 0;
    char line[LINE_SIZE];
    char buffer[LINE_SIZE];
    int status;
    int line_length;
    int n;

    pthread_t thread_auction;
    

    int channel = worker_ptr->channel;
    pthread_mutex_t mutex = worker_ptr->mutex;
    

    while(!ended)
    {
        pthread_mutex_lock(&mutex);
        line_length = recv(channel, line, sizeof(line), 0);
        pthread_mutex_unlock(&mutex);


        if(line_length < 0)
            error_IO("Reading line");
        
        else if(line_length == 0)
        {
            printf("<%s offline>\n", worker_ptr->name);
            ended = 1;
        }

        else if(!strcmp(line, "quit\n") || !strcmp(line, "exit\n")) // quit command
        {
            // ecrireLigne(channel, "Quitting...");
            printf("<%s asked to leave>\n", worker_ptr->name);
            ended = 1;
        }

        else if(!strcmp(line, "count\n")) // count command
        {
            printf("<%s asked for : count>\n", worker_ptr->name);
            n = activeWorkerCount(worker_array);

            strcpy(buffer, "Clients online : ");
            strcat(buffer, intToString(n));

            pthread_mutex_lock(&mutex);
            send(channel, buffer, sizeof(buffer), 0);
            pthread_mutex_unlock(&mutex);
        }

        else if(!strcmp(line, "ping\n")) // count command
        {
            printf("<%s asked for : ping>\n", worker_ptr->name);

            sendToAll(worker_array, "feur");
        }

        else if(!strcmp(line, "help\n")) // help command
        {
            printf("<%s asked for : help>\n", worker_ptr->name);
            n = activeWorkerCount(worker_array);

            strcpy(buffer, "Commands : \n[help] show this \n[count] show the number of clients \n[setname NAME] set your name to NAME (you are anonymous by default)\n[start PASSWORD] start the bidding process \n[info] get info about the current item \n[bid VALUE] bid on the current item");

            pthread_mutex_lock(&mutex);
            send(channel, buffer, sizeof(buffer), 0);
            pthread_mutex_unlock(&mutex);
        }

        else if(!compareFirstWord(line, "setname")) // setname command
        {
            printf("<%s asked for : setname>\n", worker_ptr->name);

            strcpy(worker_ptr->name, line);
            deleteFirstLetters(worker_ptr->name, 8); // deletes the "setname " at the beginning
            cleanString(worker_ptr->name); // deletes any \n or useless spaces

            strcpy(buffer, "Your name has been set to ");
            strcat(buffer, worker_ptr->name);

            pthread_mutex_lock(&mutex);
            send(channel, buffer, sizeof(buffer), 0);
            pthread_mutex_unlock(&mutex);
        }

        else if(!compareFirstWord(line, "start") && auction_started == 0) // start command
        {
            printf("<%s asked for : start>\n", worker_ptr->name);

            // strcpy(password, line);
            // deleteFirstLetters(password, 6); // deletes the "setname " at the beginning
            // cleanString(password); // deletes any \n or useless spaces

            if(/*!strcmp(password, PASSWORD) &&*/product_list != NULL)
            {   
                printf("Starting auction \n");
                pthread_create(&thread_auction, NULL, auctionThread, product_list);
                auction_started = 1;
            }
        }

        else if(!strcmp(line, "info\n")) // count command
        {
            printf("<%s asked for : info>\n", worker_ptr->name);
            n = activeWorkerCount(worker_array);

            if(auction_started)
                sprintf(buffer, "%s",productString(&current_product));
            else
                sprintf(buffer, "Auction has not started yet");

            pthread_mutex_lock(&mutex);
            send(channel, buffer, sizeof(buffer), 0);
            pthread_mutex_unlock(&mutex);
        }

        else if(!compareFirstWord(line, "bid")) // start command
        {
            printf("<%s asked for : bid>\n", worker_ptr->name);

            if(!auction_started)
            {
                sprintf(buffer, "Auction hasn't started yet");

                pthread_mutex_lock(&mutex);
                send(channel, buffer, sizeof(buffer), 0);
                pthread_mutex_unlock(&mutex);
            }
            else
            {
                cleanString(line);
                n = extractBidNumber(line);
                printf("New proposed price : %d\n", n);
                if(n > current_product.price)
                {
                    strcpy(current_product_owner, worker_ptr->name);
                    printf("Price changed\n");
                    current_product.price = n;

                    sprintf(buffer, "%s wants to buy %s for %d€", worker_ptr->name, current_product.title, n);
                    sendToAll(worker_array, buffer);
                }
            }
        }

        else
        {
            printf("<%s> %s", worker_ptr->name, line);

            strcpy(buffer, "Invalid command : ");
            strcat(buffer, line);
            strcat(buffer, "Type \"help\" for a list of commands");

            pthread_mutex_lock(&mutex);
            send(channel, buffer, sizeof(buffer), 0);
            pthread_mutex_unlock(&mutex);
        }
    }

    if (close(channel) == -1)
        error_IO("Closing channel"); 
}
//---------------------------------------------------------------------------------

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
        clientSession(worker_ptr);
        printf("A client left \n");
        worker_ptr->channel = -1;

        status = sem_post(&sem_free_workers);
        if (status != 0)
            error_IO("Semaphore post");
    }

    pthread_exit(NULL);
}

// Sends a message to ALL the clients
void sendToAll(Worker* worker_array, char *message)
{
    char buffer[LINE_SIZE];
    strcpy(buffer, message);

    for(int i = 0; i < NB_WORKERS; i++)
    {
        if(worker_array[i].channel > 0)
        {
            pthread_mutex_lock(&worker_array[i].mutex);
            send(worker_array[i].channel, buffer, sizeof(buffer), 0);
            pthread_mutex_unlock(&worker_array[i].mutex);
        }
    }
}

// Initialises its arg (int*) value to 0 and puts it to 1 once timer ends
void *auctionThread(void *arg)
{
    Product_list* head = (Product_list *) arg;
    while(head != NULL)
    {
        char product_data[LINE_SIZE];
        char buffer[LINE_SIZE];
        strcpy(current_product_owner, "No one");
        current_product = *(head->data);

        head = head->next; // delete the product we're selling from the list

        sprintf(buffer, "Starting auction for item : \n%sTo bid, type \"bid VALUE\". To get item info, type \"info\".\nYou have %d s to bid on that item.", productString(&current_product), SELL_DURATION_SEC);
        sendToAll(worker_array, buffer);

        time_t start_time = time(NULL);
        time_t end_time = start_time + SELL_DURATION_SEC;

        while(time(NULL) < end_time); // wait for timer

        // Write the data to the logs
        sprintf(product_data, "Owner : %s \n%s\n", current_product_owner, productString(&current_product));
        writeToLogs(product_data);

        sprintf(buffer, "%s now owns %s. They paid %d€", current_product_owner, current_product.title, current_product.price);
        sendToAll(worker_array, buffer);

        sleep(5);
    }
}

    