#ifndef ERRORS_H
#define ERRORS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

void error_IO(const char *message);
void error_pthread_IO(const char *message);
void error(const char *format, ...);

#endif