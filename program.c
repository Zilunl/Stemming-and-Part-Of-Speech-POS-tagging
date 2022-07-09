/****** Include libraries ******/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

/****** Definitions of constants******/

#define NUM_STAGELINE 26    /* The number of '=' in stage header of each side */
#define NEWLINE '\n'
#define MAX_NUM_WORD 101    /* Max number of unique words */       

/* Definitions used in typedef */
#define MAX_NUM_LETTER 23   /* Max number of lower-case English letters in each word */
#define MAX_NUM_POS 25      /* Max number of elements in pos */
#define MAX_NUM_FORM 102    /* Max number of elements in form */
#define MAX_FORM 4          /* Max number of forms in each word */
#define MAX_LETTER_FORM 26  /* Max number of words in each form */
#define NUM_STAR 11         /* The number of stars */

#define BS_NOT_FOUND (-1)   
#define BS_FOUND 0          

/****** Type definitions ******/

typedef char data_t;
typedef struct node node_t;

/* Holds all components of a word */
typedef struct{
    char word[MAX_NUM_LETTER];
    char pos[MAX_NUM_POS];
    char form[MAX_NUM_FORM];
    char form_2d[MAX_FORM][MAX_LETTER_FORM];
}word_t;

/* define a struct with two members: data and next */
struct node {
    char data[MAX_LETTER_FORM];
    node_t *next;
};

/* define a struct with two members: head and foot of a list */
typedef struct {
    node_t *head;
    node_t *foot;
} list_t;

/****** Function Prototypes ******/

/* Functions of each stage */
void stage1(word_t words[]);
int stage2(word_t words[]);
void stage3(int word_num, node_t* temp, word_t words[]);
void stage4(int word_num, node_t* temp, word_t words[]);

/* Functions used in each stage */
void print_stage_header(int stage_num);
int getword(char W[]);
node_t* linklist(word_t words[]);
node_t* makeNode(char word[]);
int binary_search(data_t A[], int left, int right, data_t *key);
void change_to_2dform (int count, word_t words[]);
void free_list(list_t *list);


/****** The main function of the program ******/

int main(int argc, char *argv[]){
    
    word_t words[MAX_NUM_WORD]; // Word information
    
    // Stage1
    stage1(words);
    
    // Stage 2 and store number of words
    int word_num = stage2(words);

    //Store all words and get the linkedlist
    node_t* temp = linklist(words);

    // Stage 3
    stage3(word_num, temp, words);
    
    // Stage 4
    stage4(word_num, temp, words);
    
    // Free the linkedlist
    free(linklist(words));
    
    return 0;
}

/* print stage header given stage number */
void print_stage_header(int stage_num) {

    for (int i=0; i<NUM_STAGELINE*2; i++){
        if (i == NUM_STAGELINE) {
            printf("Stage %d", stage_num);
        }
        printf("=");
    }
    printf("\n");

}

/********* Stage 1 Functions *********/

void stage1(word_t words[]){

    /* Print the header of stage 1 */
    int stage_num = 1; 
    print_stage_header(stage_num); 

    /* Get the first word information from input file */
    scanf("$%s\n", words[0].word);
    scanf("%s\n", words[0].pos);
    /* Get char in form and careful of no variation form */
    char c;
    int form_index=0;
    while ((c = getchar()) != '\n'){
        if (c=='#'){
        } else {
            words[0].form[form_index] = c;
            form_index++;
        }
    }
    
    /* Print the first word information */
    printf("Word 0: %s\n", words[0].word);
    printf("POS: %s\n", words[0].pos);
    printf("Form: %s\n", words[0].form);
    
}

/********* Stage 2 Functions *********/

int stage2(word_t words[]){

    /* Print the header of stage 2 */
    int stage_num = 2;
    print_stage_header(stage_num);

    /* Scanf each line of word, store them and find the number of words */
    int index = 1, word_num = 1;
    float form_num = 0.0;
    while (getchar() == '$'){
        word_num++;
        scanf("%s\n", words[index].word);
        scanf("%[^\n]", words[index].pos);
        scanf("%s\n", words[index++].form);
    }
    
    /* Find the number of forms */
    for (int i=0; i<word_num; i++){
        for (int j=0; words[i].form[j] != '\0'; j++){
            if (isdigit(words[i].form[j])){
                form_num++;
            }
        }      
    }
    
    float average = form_num / word_num;
    printf("Number of words: %d\n", word_num);
    printf("Average number of variation forms per word: %.2f\n", average);

    return word_num;

}

/* Store  all word to linkedlist */
node_t* linklist(word_t words[]){

    char word[MAX_NUM_LETTER];
    list_t linkedlist;
    linkedlist.head = NULL;
    linkedlist.foot = NULL;

    while (getword(word)==0){

        node_t* newnode = makeNode(word);

        if (linkedlist.head==NULL && linkedlist.foot==NULL){
            linkedlist.head=newnode;
            linkedlist.foot=newnode;
        }
        else{
            (linkedlist.foot)->next=newnode;
            linkedlist.foot=newnode;
        }
    }

    node_t* temp = linkedlist.head;

    return temp;
}

/* Make an empty list */
node_t* makeNode(char word[]){

    node_t* newnode = (node_t*)malloc(sizeof(node_t));
    assert(newnode);
    for (int i=0; i<MAX_NUM_LETTER; i++){
        newnode->data[i] = word[i];
    }
    newnode->next = NULL;
    return newnode;

}

/********* Stage 3 Functions *********/

void stage3(int word_num, node_t* temp, word_t words[]){

    /* Print the header of stage 3 */
    int stage_num = 3;
    print_stage_header(stage_num);

    /* Got the star line */
    char star_line[NUM_STAR];
    scanf("%s\n",star_line);

    /* Cheak if it exist in word and print the POS */
    while (temp != NULL){

        int flag = 0;
            
        for (int i=0; i<word_num; i++){

            if (binary_search(words[i].word, 0, word_num, temp->data) == BS_FOUND){
                printf("%-26s%s\n", temp->data, words[i].pos);
                flag = 1;
            }
        }

        if (flag == 0){
            printf("%-26sNOT_FOUND\n", temp->data);
        }
        temp = temp->next;
    }
    
}

/* Extract a single word out of the standard input, of not
   more than limit characters. */
int getword(char W[]) {

    //char W[26];
    int c, len=0;
    /* first, skip over any non alphabetics */
    while ((c=getchar())!=EOF && !isalpha(c)) {
        /* do nothing more */
    }
    if (c==EOF) {
        return EOF;
    }
    /* first character of next word has been found */
    W[len++] = c;
    while (len<MAX_LETTER_FORM && (c=getchar())!=EOF && isalpha(c)) {
        /* another character to be stored */
        W[len++] = c;
    }
    /* close off the string */
    W[len] = '\0';
    return 0;

}

/* Check if key is in A use binary search */
int binary_search(data_t A[], int left, int right, data_t *key) {
    int mid, outcome;
    /* if key is in A, it is between left and right */
    if (left>=right) {
        return BS_NOT_FOUND;
    }
    mid = (left+right)/2;
    if ((outcome = strcmp(key, A)) < 0) {
        //return BS_NOT_FOUND;
        return binary_search(A, left, mid, key);
    } else if (outcome > 0) {
        //return BS_NOT_FOUND;
        return binary_search(A, mid+1, right, key);
    } else {
        return BS_FOUND;
    }
}

/********* Stage 4 Functions *********/

void stage4(int word_num, node_t* temp, word_t words[]){

    /* Print the header of stage 4 */
    int stage_num = 4;
    print_stage_header(stage_num);

    change_to_2dform (word_num, words);

    while (temp!=NULL){
        int flag = 0;
        for (int i=0; i<word_num; i++){
            /* first check if it exist in root*/
            if (binary_search(words[i].word, 0, word_num, temp->data) == BS_FOUND){
                    printf("%-26s%-26s%s\n", temp->data, words[i].word, words[i].pos);
                    flag=1;
            }
            /* then compare with form of each word */
            for (int j=0; j<MAX_FORM; j++){
                if (binary_search(words[i].form_2d[j], 0, word_num, temp->data) == BS_FOUND){
                    printf("%-26s%-26s%s\n", temp->data, words[i].word, words[i].pos);
                    flag=1;
                }
            }
        }
        
        /* can not found the root */
        if (flag == 0){
            printf("%-26s%-26sNOT_FOUND\n", temp->data, temp->data);
        }
        temp = temp->next;
    }
}

/* Change form array to 2D array */
void change_to_2dform (int word_num, word_t words[]){

    /* i, j, k, m are all index number */
    int k = 0;
    for (int i=0; i<word_num; i++){
        int j = -1; // The first is -1 as before char must be digit
        int index = 0;

        while (words[i].form[index] != '\0'){

            if (isalpha(words[i].form[index])){
                words[i].form_2d[j][k] = words[i].form[index];
                k++;
            }

            else if (isdigit(words[i].form[index])){
                j++;
                k=0;
            }
            index++;
        }
        
    }
}

/* Free all memory */
void free_list(list_t* list) {
    node_t *curr, *prev;
    assert(list != NULL);
    curr = list->head;
    while (curr) {
        prev = curr;
        curr = curr->next;
        free(prev);
    }
    free(list);
}

///////////////////
/* The worst case is: s*d+s*d*f
comparing with if it exist in root or form.
It has time complexity because in most cases, 
the number will be less than the maximum, 
and there will be many useless empty loops. */