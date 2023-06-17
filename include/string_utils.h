#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "settings.h"

char* intToString(int num);
int compareFirstWord(const char* str, const char* word);
void deleteFirstLetters(char* str, int n);
void cleanString(char* str);
int extractBidNumber(const char* str);

#endif