#include "errors.h"

void error_IO(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

void error_pthread_IO(const char *message)
{
    perror(message);
    pthread_exit(NULL);
}

void error(const char *format, ...)
{
    va_list list_arg;
    fflush(stdout);
    fprintf(stderr, "[ERROR] ");
    va_start(list_arg, format);
    vfprintf(stderr, format, list_arg);
    va_end(list_arg);
    fflush(stderr);
    exit(EXIT_FAILURE);
}