#ifndef LISTS_H
#define LISTS_H

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
void printData(Product_list *head);

#endif
