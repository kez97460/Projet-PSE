#include "files.h"

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

            // Introduction de la lecture dans la structure
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

char* productString(Product *product)
{
    char* res = (char*) malloc(LINE_SIZE * sizeof(char));
    sprintf(res, "Name : %s \nPrice : %u \nDescription : %s\n", product->title, product->price, product->desc);
    return res;
}

void printData(Product_list *head)
{
    Product_list *current = head;
    while (current != NULL) 
    {
        printf("%s\n", productString(current->data));
        current = current->next;
    }
}

void writeToLogs(const char* str) 
{
    FILE* file = fopen(LOG_NAME, "a"); // Open the file in append mode
    
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return;
    }
    
    fputs(str, file); // Write the string to the file
    
    fclose(file); // Close the file
}

void emptyFile(char* filename) 
{
    FILE* file = fopen(filename, "w"); // Open the file in write mode
    
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return;
    }
    
    fclose(file); // Close the file
}

void addProduct(Product_list** head, Product* product) {
    // Create a new node
    Product_list* newNode = (Product_list*)malloc(sizeof(Product_list));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    // Set the data and next pointer of the new node
    newNode->data = product;
    newNode->next = *head;

    // Update the head pointer to point to the new node
    *head = newNode;
}

Product_list* testList()
{
    Product p1;
    strcpy(p1.title, "La fatigue de Kezia");
    strcpy(p1.desc, "...");
    p1.price = 1;

    Product p2;
    strcpy(p2.title, "Un pipou");
    strcpy(p2.desc, "Très recherché pendant la Coupe de France de Robotique");
    p2.price = 100;

    Product p3;
    strcpy(p3.title, "AAAAA");
    strcpy(p3.desc, "Oui.");
    p3.price = 10;

    Product_list* list = NULL;
    addProduct(&list, &p1);
    addProduct(&list, &p2);
    addProduct(&list, &p3);

    printData(list);

    return list;
}