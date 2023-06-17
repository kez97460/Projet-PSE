#include "string_utils.h"

char* intToString(int num) 
{
    char* str = (char *) malloc(LINE_SIZE * sizeof(char));
    sprintf(str, "%d", num);
    return str;
}

int compareFirstWord(const char* str, const char* word) 
{
    char firstWord[100]; // Assuming the maximum length of the first word is 100
    sscanf(str, "%s", firstWord);
    
    return strcmp(firstWord, word);
}

void deleteFirstLetters(char* str, int n) 
{
    int length = strlen(str);
    
    if (n >= length) {
        // If n is greater than or equal to the length of the string,
        // set the string to an empty string
        str[0] = '\0';
    } else {
        // Shift the characters to the left by n positions
        memmove(str, str + n, length - n + 1);
    }
}

void cleanString(char* str) 
{
    int length = strlen(str);
    int start = 0;
    int end = length - 1;

    // Find the start index by skipping leading whitespace
    while (isspace(str[start]))
        start++;

    // Find the end index by skipping trailing whitespace
    while (end >= start && isspace(str[end]))
        end--;

    // Shift the characters to the left
    int i, j;
    for (i = 0, j = start; j <= end; i++, j++)
        str[i] = str[j];

    // Set the null terminator
    str[i] = '\0';
}

int extractBidNumber(const char* str) 
{
    int number;
    sscanf(str, "bid %d", &number);
    return number;
}