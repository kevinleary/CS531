/*
Kevin Leary 
G01357635
*/

/*
Running instructions: 
 1. Open terminal 
 2. `gcc -o HW3-Gnum HW3-Gnum.c`
 3. ./HW3-Gnum

 OR 

 1. use the runner.sh
*/


#include <stdio.h> 
#include <string.h>
#include <stdlib.h>

struct address_t
{
    int octet[4];
    char alias[11];
    struct address_t *leftChild, *rightChild, *parent;
    int height, depth;
};
struct address_t *head = NULL;
//ONE GLOBAL VAR
int global_count = 0;

//Function Headers: 
void createListFromFile();              // -- working
void displayList(struct address_t* node); // --  working
void addAddress();                      // -- working 
struct address_t *lookUpAddress();      // -- working
void updateAddress();                   // -- not really working
void deleteAddress();                   // -- working
void displayAliasforLocation();         // -- working
void saveToFile();                      // -- working
void menu();      

//BST Functions
struct address_t *createNode(int octet0, int octet1, int octet2, int octet3, char alias[11]);
struct address_t* insert(struct address_t* node, int octet0, int octet1, int octet2, int octet3, char alias[11]);
int findDepth(struct address_t* node);
int findHeight(struct address_t* node);
void searchForDups(struct address_t* root, int octet[4], char alias[11]);
struct address_t* searchForAlias(struct address_t* node, char alias[11]);
void searchForLocality(struct address_t* root, int loc0, int loc1);
struct address_t* minValueNode(struct address_t* node);
void copyNode(struct address_t* dest, struct address_t* src);
struct address_t* deleteNode(struct address_t* node, char alias[11]);



int main() {

    menu();

    return 0;
}


void menu() {

    int exit, choice = 0;
    printf("\nWelcome to the IPv4 Program!\nWe will start by reading in the CS531_Inet.txt file!\n");
    //Start with file list...
    createListFromFile();

    //Then do the menu prompting...
    while (exit != 1) {
        printf("\n\nIPv4 Menu:\n1) Add address\n2) Look up address\n3) Update address\n4) Delete address\n5) Display list\n6) Display aliases for location\n7) Save to file\n8) Quit\n\n");
        printf("Please enter a choice from the above menu: ");
        scanf("%d", &choice);
        getchar(); // - used as buffer absorber for input first keyboard input not working

        switch(choice) {
            case 1 :
                addAddress();
                break;
            case 2 : 
                lookUpAddress();
                break;
            case 3 : 
                updateAddress();
                break;
            case 4 :
                deleteAddress();
                break;
            case 5 :
                displayList(head);
                break;
            case 6 : 
                displayAliasforLocation();
                break;
            case 7 : 
                saveToFile();
                break;
            case 8 :
                exit = 1;
                break;
            default : 
                printf("Error! Invalid Input- Please re-enter your choice!\n");
                break;
        }
    }
}


/****
 * 
 * 
 *  BST Functions
 * 
 * ***/

struct address_t* deleteNode(struct address_t* node, char alias[11]) {

    // base case
    if (node == NULL) 
        return node;
    // If the key to be deleted is smaller than the root's key,
    // then it lies in left subtree
    
    if ( strcmp(alias, node->alias) < 0 ) {
        
        node->leftChild = deleteNode(node->leftChild, alias);
    }

    // If the key to be deleted is greater than the root's key,
    // then it lies in right subtree
    else if (strcmp(alias, node->alias) > 0) {
        // free(node);
        node->rightChild = deleteNode(node->rightChild, alias);
    }

    //actually do some deleting
    else {
        if (node->leftChild == NULL) {
            struct address_t* temp = node->rightChild;
            free(node);
            return temp;
        }
        else if (node->rightChild == NULL) {
            struct address_t* temp = node->leftChild;
            free(node);
            return temp;
        }

        struct address_t* temp = minValueNode(node->rightChild);
        node->octet[0] = temp->octet[0];
        node->octet[1] = temp->octet[1];
        node->octet[2] = temp->octet[2];
        node->octet[3] = temp->octet[3];
        strcpy(node->alias, temp->alias);
        node->rightChild = deleteNode(node->rightChild, temp->alias);
    }
    return node;
}

//Copy to the destination from the source
void copyNode(struct address_t* dest, struct address_t* src) {

    dest->octet[0] = src->octet[0];
    dest->octet[1] = src->octet[1];
    dest->octet[2] = src->octet[2];
    dest->octet[3] = src->octet[3];
    strcpy(dest->alias, src->alias);
    dest->leftChild = src->leftChild;
    dest->rightChild = src->rightChild;
    dest->parent = src->parent;
    dest->depth = src->depth;
    dest->height = src->height;
}

/* Given a non-empty binary search tree, return the node with the minimum
key value found in that tree. */
struct address_t * minValueNode(struct address_t* node) {
    struct address_t* current = node;
    /* loop down to find the leftmost leaf */
    while (current && current->leftChild != NULL)
        current = current->leftChild;
    
    return current;
}

struct address_t* searchForAlias(struct address_t* node, char alias[11]) {
    // base case
    if (node == NULL) 
        return node;
    // If the key to be deleted is smaller than the root's key,
    // then it lies in left subtree
    if ( strcmp(alias, node->alias) < 0 ) {
        
        return searchForAlias(node->leftChild, alias);
    }

    // If the key to be deleted is greater than the root's key,
    // then it lies in right subtree
    else if (strcmp(alias, node->alias) > 0) {
        // free(node);
        return searchForAlias(node->rightChild, alias);
    }

    else if (strcmp(alias, node->alias) == 0 ) {
        struct address_t* found = createNode(node->octet[0], node->octet[1], node->octet[2], node->octet[3], node->alias);
        found->parent = node->parent;
        found->depth = node->depth;
        found->height = node->height;
        return found;
    }

    else 
        return node;

}

int findHeight(struct address_t* node) {

    //Height algorithm is n-1 so need to start at -1 
    if (node == NULL){
        return -1;
    } else {
        // simple max function to determine which height to add based on children
        if ( findHeight(node->leftChild) > findHeight(node->rightChild) )
            return findHeight(node->leftChild) + 1;
        else
            return findHeight(node->rightChild) + 1;
    }

}

int findDepth(struct address_t* node) {
    //If node has no parents its depth is 0
    if (node->parent == NULL) 
        return 0;
    //Go up the tree adding 1 for each parent until no parents
    return findDepth(node->parent) + 1;
}

struct address_t *createNode(int octet0, int octet1, int octet2, int octet3, char alias[11]) {
    struct address_t *temp= (struct address_t*)malloc(sizeof(struct address_t));
    temp->octet[0] = octet0;
    temp->octet[1] = octet1;
    temp->octet[2] = octet2;
    temp->octet[3] = octet3;
    strcpy(temp->alias, alias);
    temp->leftChild = temp->rightChild = temp->parent = NULL;
    temp->depth = temp->height = -1;
    return temp;
}


struct address_t* insert(struct address_t* node, int octet0, int octet1, int octet2, int octet3, char alias[11]) {
    
    //If the tree is empty return a new node
    if (node == NULL) return createNode(octet0, octet1, octet2, octet3, alias);
    
    //Otherwise, recur down the tree
    if ( strcmp(alias, node->alias) < 0 ) {
        //insertion changes order so need to reset depth and height
        node->depth = node->height = -1;
        node->leftChild = insert(node->leftChild, octet0, octet1, octet2, octet3, alias);
        node->leftChild->parent = node;
    }
    else {
        //insertion changes order so need to reset depth and height
        node->depth = node->height = -1;
        node->rightChild = insert(node->rightChild, octet0, octet1, octet2, octet3, alias);
        node->rightChild->parent = node;
    }
    
    // return node pointer
    return node;
}

void saveToFileInorderTraversal(struct address_t* root, FILE *file) {
        
    if (root != NULL) {
        saveToFileInorderTraversal(root->leftChild, file);
        if ( root->parent == NULL ) {
            // root->depth = findDepth(head, root->alias);
            fprintf(file, "%d.%d.%d.%d %s\n", root->octet[0], root->octet[1], root->octet[2], root->octet[3], root->alias);
        } else {
            // root->depth = findDepth(head, root->alias);
            fprintf(file, "%d.%d.%d.%d %s\n", root->octet[0], root->octet[1], root->octet[2], root->octet[3], root->alias);
        }
        saveToFileInorderTraversal(root->rightChild, file);
    }
}

void saveToFile() {

    char filename[50];
    // struct address_t *ptr = head;
    //Prompt for file name
    printf("Please enter a name for the file: ");
    fgets(filename, 50, stdin);
    filename[strcspn(filename, "\n")] = 0;
    strcat(filename, ".txt");
    //Open file
    FILE *address_file = fopen(filename, "w");

    //Put list into file
    saveToFileInorderTraversal(head, address_file);

    printf("File %s ready!\n", filename);
    fclose(address_file);
}


void searchForLocality(struct address_t* root, int loc0, int loc1) {
    
    if (root != NULL) {    
        searchForLocality(root->leftChild, loc0, loc1);
        if ( (root->octet[0] == loc0) && (root->octet[1] == loc1)) {
            // root->depth = findDepth(head, root->alias);
            printf("%s\n", root->alias);
            global_count = 1;
        }
        // else if (root->parent == NULL && )
        // if (root->parent == NULL)
        //     printf("Parent: NULL\n");
        // else
        //     printf("Parent: %s\n", root->parent->alias);
        searchForLocality(root->rightChild, loc0, loc1);
    } 
    // return 0;

    //     printf("Error: Locality %d.%d does not exist in the list!\n", loc1, loc2);
    // printf("\nAliases with range %d.%d found are: \n", loc1, loc2);
}


void displayAliasforLocation() {

    //This needs to be ordered pair - first two values for locality... 172.16...
    //List ALL alias that map to 172.16 ... 

    char locality[8];
    // char list[100][11];
    // struct address_t *ptr = head;
    // struct address_t *tmp = (struct address_t*)malloc(sizeof(struct address_t));
    int loc0, loc1 = 0;

    //Prompt for locality
    printf("Please enter a locality (XXX.XXX) to see if it exists in the list: ");
    fgets(locality, 8, stdin);  
    //TODO:REPROMPT IF ERROR
    locality[strcspn(locality, "\n")] = 0;
    sscanf(locality, "%d.%d\n", &loc0, &loc1);
        
    printf("Locality is %d.%d\n", loc0, loc1);
    searchForLocality(head, loc0, loc1);

    if ( global_count == 0) {
        printf("Error: Locality %d.%d does not exist in the list!\n", loc0, loc1);
    }

    //reset global var
    global_count = 0;

}


void deleteAddress() { 

    char confirm;
    struct address_t *del = lookUpAddress();
    // struct address_t *prev = head;

    if (del != NULL) {
        
        printf("Please confirm that you want to delete alias %s with ip %d.%d.%d.%d...\nEnter 'Y' for Yes or 'N' for No: ", del->alias, del->octet[0], del->octet[1], del->octet[2], del->octet[3]);
        scanf("%c", &confirm);
        
        if ( confirm == 'Y' || confirm == 'y' ) {
            deleteNode(head, del->alias);

        } else if ( confirm == 'N' || confirm == 'n' ) {
            displayList(head);
            
        } else {        // - This is for error input
            printf("\nError: %c is not 'Y' or 'N' - try again...\n", confirm);
            //Empty Character constant
            confirm = '\0';
            displayList(head);
        }

    }

}


void updateAddress() {

    //searching will be the same as the look-up it will just do something different when it finds it
    //I think that lookup should be reused here
    // char alias[] = lookUpAddress();
    int exit = 0;
    char ip[17];
    struct address_t *found = lookUpAddress();
    // struct address_t *ptr = head;
    printf("here\n");
    struct address_t *update = (struct address_t*)malloc(sizeof(struct address_t));

    if (found != NULL) {
        while ( exit != 1 ) {
            printf("Enter the updated IPv4 address for %s: ", found->alias);
            fgets(ip, 17, stdin);  
            ip[strcspn(ip, "\n")] = 0;
            sscanf(ip, "%d.%d.%d.%d\n", &update->octet[0], &update->octet[1], &update->octet[2], &update->octet[3]);
            //check for octet out of range 0-255
            strcpy(update->alias, found->alias);
            printf("%d.%d.%d.%d %s\n", update->octet[0], update->octet[1], update->octet[2], update->octet[3], update->alias);
            for (int i = 0; i < 4; i++) {

                if ( update->octet[i] > 255 || update->octet[i] < 0 ) {
                    printf("Please re-enter IPv4! It is outside of the valid range 0-255\n");
                    break;
                } else if ( i == 3 && update->octet[i] < 256 && update->octet[i] > 0 ){
                    exit = 1;
                    break;
                } 

            }

            searchForDups(head, update->octet, "");
            if (global_count == 1) {
                printf("Error: Address is a duplicate!\n");
                exit = 1;
            } 

        }

        if ( global_count != 1 ) {
            printf("Alias %s IPv4 address %d.%d.%d.%d is now %d.%d.%d.%d\n", found->alias, found->octet[0], found->octet[1], found->octet[2], found->octet[3], update->octet[0], update->octet[1], update->octet[2], update->octet[3]);
            //set new octet values
            for (int i = 0; i < 4; i++) {
                found->octet[i] = update->octet[i];
            }
            copyNode(update, found);
            deleteNode(head, update->alias);
            insert(head, update->octet[0], update->octet[1], update->octet[2], update->octet[3], update->alias);
        }
        //reset global var
        global_count = 0;
    }

}


struct address_t *lookUpAddress() {

    //this might need to return an int for usage in update and delete address functions
    char alias[50];
    // struct address_t *ptr = head;
    // struct address_t *tmp = (struct address_t*)malloc(sizeof(struct address_t));
    
    //Prompt for alias...
       
    //Get input - Alias
    printf("Enter a the alias to search for: ");
    fgets(alias, 50, stdin); 

    printf("%s\n", alias);
    //I believe that this adds a whitespace that we need so I am including it here too so strings stay consistent
    sscanf(alias, "%s\n", alias);

    struct address_t *tmp = searchForAlias(head, alias);
    // printf("%s\n", tmp->alias);
    if ( tmp != NULL && strcmp(tmp->alias, alias) == 0) {
        printf("The address for alias:%s is %d.%d.%d.%d\n", tmp->alias, tmp->octet[0], tmp->octet[1], tmp->octet[2], tmp->octet[3]);
        return tmp;
    } else {
        printf("\nError! Alias does not exist in list!\n");
        return NULL;
    }
}


void searchForDups(struct address_t* root, int octet[4], char alias[11]) {
    
    if (root != NULL) {    
        searchForDups(root->leftChild, octet, alias);
        
        int count = 0;
        for (int i = 0; i < 4; i++) {
            if (head->octet[i] == octet[i]) {
                count++;
            }
        }
        if (count == 4) {
            if (global_count != 1)
                printf("\nError: IPv4 already exists in list!\n");
            global_count = 1;
            return;
        }
        
        //now check for alias
        if ( strcmp(root->alias, alias) == 0 && strcmp(alias, "") != 0) {
            printf("\nError: Alias already exists in list!\n");
            global_count = 1;
            return;
        }

        searchForDups(root->rightChild, octet, alias);
    } 
}

//When addAddress is first selection it makes the menu reprompt and auto populates a garbage address - mind boggling (maybe only on the mac?)
void addAddress() {
        
    char ip[17];
    char alias[50];
    char ipAlias[50];
    int octet[4];
    int exit, reprompt, iter = 0;
    
    //working...
    while (exit != 1) {
        //Get IP and alias
        printf("Enter IPv4 Address followed by a 10 character Alias: ");
        fgets(ipAlias, 50, stdin);  
        // ipAlias[strcspn(ipAlias, "\n")] = 0;
        sscanf(ipAlias, "%d.%d.%d.%d %s", &octet[0], &octet[1], &octet[2], &octet[3], alias);
        //check for octet out of range 0-255
        for (int i = 0; i < 4; i++) {
            if ( octet[i] > 255 || octet[i] < 0 ) {
                printf("Please re-enter IPv4! It is outside of the valid range 0-255\n");
                break;
            } else if ( i == 3 && octet[i] < 256 && octet[i] > 0 ) {
                
                if ( strlen(alias) > 10 ) {
                    printf("Please re-enter the alias! It's more than 10 digits!\n");
                    break;
                } else {
                    exit = 1;
                }
                break;
            } 
        }

    }

    //Search for duplicates
    searchForDups(head, octet, alias);
    if ( global_count == 0) {
        // printf("\nError: IPv4 or Alias already exists in list!\n");
        insert(head, octet[0], octet[1], octet[2], octet[3], alias);

    }
    //reset global var
    global_count = 0;
        
}

//Order is based on alias!!!
void createListFromFile() {

    //init node - needs to be NULL or garbage value populated
    struct address_t *init= NULL;
    //Create new vars
    int initOctet[4];
    char initAlias[11];

    // Open the file
    FILE *address_file = fopen("CS531_Inet.txt", "r+");
    
    // Check if the file was opened successfully.
    if (address_file == NULL) {
        printf("\nError opening file.\n");
    }  
    
    // Read the contents of the file line by line.
    char line[100];

    while (fgets(line, sizeof(line), address_file)) {
        // //Populate empty node
        // if (temp == NULL) {
        //     head = temp
        // } 

        //Get line input
        sscanf(line, "%d.%d.%d.%d %s\n", &initOctet[0], &initOctet[1], &initOctet[2], &initOctet[3], initAlias);
        init = insert(init, initOctet[0], initOctet[1], initOctet[2], initOctet[3], initAlias);
    }
    //Set head to the init BST because this function will always be ran first
    head = init;

}

void displayList(struct address_t *root) {
    
    //Inorder traversal

    if (root != NULL) {
        //check if height and depth not already calculated or have been shifted
        if ( (root->depth == -1) || (root->height == -1) ) {
            root->depth = findDepth(root);
            root->height = findHeight(root);
        }
        displayList(root->leftChild);
        if ( root->parent == NULL ) {
            // root->depth = findDepth(head, root->alias);
            printf("%d.%d.%d.%d %s height:%d depth:%d parent:NONE\n", root->octet[0], root->octet[1], root->octet[2], root->octet[3], root->alias, root->height, root->depth);
        } else {
            // root->depth = findDepth(head, root->alias);
            printf("%d.%d.%d.%d %s height:%d depth:%d parent:%s\n", root->octet[0], root->octet[1], root->octet[2], root->octet[3], root->alias, root->height, root->depth, root->parent->alias);
        }
        // if (root->parent == NULL)
        //     printf("Parent: NULL\n");
        // else
        //     printf("Parent: %s\n", root->parent->alias);
        displayList(root->rightChild);
    }

}
