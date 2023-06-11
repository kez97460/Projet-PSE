#include "files.h"
#include <stdio.h>

#define NB_ELEMENTS 3

void readTxt(Product_list *head, char* txt_name)
{   
    FILE *fichier;

    /* ouverture du fichier en écriture */
    fichier = fopen(txt_name,"r");
    
    /* vérifier que le fichier a bien été ouvert */
    if(fichier != NULL)
    {
        /* fscanf renvoie le nombre d’items effectivement lus */
        for (int i=0; i<NB_ELEMENTS; i++)
        fscanf(fichier , "%s %d %s", head->data->title, &(head->data->price), head->data->desc);
        fclose(fichier ); // fermeture du fichier
    }
}

void printData(Product_list *head)
{
    printf("%s %d %s\n", head->data->title, head->data->price, head->data->desc);
}

void addNode(Product_list *head)
{

}

