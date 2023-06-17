#ifndef LISTS_H
#define LISTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "settings.h"

/*--------------------------------------------------------------------------------------------*/

typedef struct _product
{
    char title[30];
    unsigned int price; //in dollars
    char desc[280];

} Product;

typedef struct _product_list
{
    Product *data;
    struct _product_list* next;
} Product_list;

/*--------------------------------------------------------------------------------------------*/

void readTxt(Product_list *head, char* txt_name);
char* productString(Product *product);
void printData(Product_list *head);
void writeToLogs(const char* str);
void emptyFile(char* filename);
Product_list* testList();
void addProduct(Product_list** head, Product* product);

#endif
