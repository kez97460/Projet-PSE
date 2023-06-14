#include "files.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NB_ELEMENTS 3

void readTxt(Product_list *head, char* txt_name)
{   
    FILE *fichier;

    char temp_title[30];
    unsigned int temp_price; //in dollars
    char temp_desc[280];

    /* ouverture du fichier en écriture */
    fichier = fopen(txt_name,"r");

    
    /* vérifier que le fichier a bien été ouvert */
    if(fichier != NULL)
    {
        while(fscanf(fichier, "%30s%u%240[^\n]", temp_title, &(temp_price), temp_desc)==3) 
        {

            // Allocation de memoire au nouvelle maillon de la liste
            Product_list *newNode = (Product_list*)malloc(sizeof(Product_list));
            newNode->data = (Product*)malloc(sizeof(Product));
            newNode->next = NULL;

            //Introduction de la lecture dans la structure
            strcpy(newNode->data->title, temp_title);
            newNode->data->price = temp_price;
            strcpy(newNode->data->desc, temp_desc);
        
            // Mise à jour de la tête afin de pointer vers la nouvelle maille
            if (head == NULL) {
                head = newNode;
            } 
            
            else {
                Product_list *current = head;
                while (current->next != NULL) {
                    current = current->next;
                }
                current->next = newNode;
            }
        }

        fclose(fichier);
    }
}

void printData(Product_list *head)
{
    Product_list *current = head;
    while (current->next != NULL) 
    {
        current = current->next;
        printf("%s %u %s\n", current->data->title, current->data->price, current->data->desc);
    }
}
