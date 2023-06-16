#include <stdio.h>
#include <stdlib.h>
#include "files.h"

int main()
{
    Product_list *head = (Product_list*)malloc(sizeof(Product_list));
    head->data = (Product*)malloc(sizeof(Product));
    head->next = NULL;

    readTxt(head, "products.txt");
    printData(head);

    return 0;
}
