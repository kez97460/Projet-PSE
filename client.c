#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "settings.h"
#include "resolv.h"
#include "errors.h"
#include "ligne.h"
#include "string_utils.h"

void *recvThread(void *arg);

int ended; // end everything once this is 1

int main(int argc, char const *argv[])
{
    int descriptor_socket, status;
    struct sockaddr_in *server_address;
    ended = 0;
    char line[LINE_SIZE];
    char buffer[LINE_SIZE];
    int line_length;

    pthread_t thread_recv;

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    // Output an error message if we write while the server is down
    signal(SIGPIPE, SIG_IGN);

    if (argc != 3)
        error("Usage: %s machine port\n", argv[0]);

    printf("Client : creating a socket\n");
    descriptor_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (descriptor_socket < 0)
        error_IO("Socket creation");

    printf("Client : DNS resolving for %s, port %s\n", argv[1], argv[2]);
    server_address = resolv(argv[1], argv[2]);
    if (server_address == NULL)
        error("adresse %s port %s inconnus\n", argv[1], argv[2]);

    printf("Client : address %s, port %d \n", stringIP(ntohl(server_address->sin_addr.s_addr)), ntohs(server_address->sin_port));

    printf("Client : connecting socket \n");
    status = connect(descriptor_socket, (struct sockaddr *)server_address, sizeof(struct sockaddr_in));
    if(status < 0)
        error_IO("Connection");

    // create a thread to receive data
    pthread_create(&thread_recv, NULL, recvThread, &descriptor_socket);

    /* Infinite loop to send data */
    while (!ended)
    {
        printf(">");
        if(fgets(line, LINE_SIZE, stdin) == NULL) // saisie de CTRL-D
            ended = 1;
        else
        {
            pthread_mutex_lock(&mutex);
            line_length = send(descriptor_socket, line, sizeof(buffer), 0);
            pthread_mutex_unlock(&mutex);

            if(line_length < 0)
                error_IO("Socket write");
            
            else if(!strcmp(line, "quit\n") || !strcmp(line, "exit\n"))
            {
                printf("Quitting...\n");
                ended = 1;
            }
        }
    }

    if (close(descriptor_socket) == -1)
        error_IO("fermeture socket");

    return 0;
}

// Thread that receives messages from the server and prints them
void *recvThread(void *arg)
{
    char buffer[LINE_SIZE];
    int line_length;

    int *socket_ptr = (int *) arg;

    while(!ended)
    {
        line_length = recv(*socket_ptr, buffer, sizeof(buffer), 0);
        cleanString(buffer);
        printf("\r%s \n>", buffer);
        fflush(stdout); // needed to print the '>', idk why
    }
}
