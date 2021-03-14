#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>



typedef struct node_binomial_tree { // binomial heap consists of binomial trees.
    int k;
    int number_of_words;
    char *filename;
    struct node_binomial_tree *child;
    struct node_binomial_tree *sibling;
} node_tree;

typedef struct node_binomial_heap {
    node_tree *root;
} node_heap;


node_tree *anUnion(node_heap *heap1,
                   node_heap *heap2) { // main operation of binomal heaps. it is used for enqueue and dequeue operations.


    if (heap1->root == NULL) return heap2->root;
    else if (heap2->root == NULL) return heap1->root;


    node_tree *forH1 = heap1->root;
    node_tree *forH2 = heap2->root;
    node_tree *root;
    node_tree *tail;

    if (heap1->root->k > heap2->root->k) {
        root = heap2->root;
        forH2 = forH2->sibling;
    } else {
        root = heap1->root;
        forH1 = forH1->sibling;
    }
    tail = root;
    while (forH1 != NULL && forH2 != NULL) {
        if (forH1->k > forH2->k) {
            tail->sibling = forH2;
            forH2 = forH2->sibling;
        } else {
            tail->sibling = forH1;
            forH1 = forH1->sibling;
        }
        tail = tail->sibling;
    }

    if (forH1 != NULL) { tail->sibling = forH1; }
    else { tail->sibling = forH2; }


    node_tree *mergedHeap = root;

    heap1->root = NULL;
    heap2->root = NULL;


    if (mergedHeap != NULL) {
        node_tree *next = mergedHeap->sibling;
        node_tree *prev = NULL;
        node_tree *temp = mergedHeap;
        while (next != NULL) {
            if ((next->sibling != NULL && temp->k == next->sibling->k) || (temp->k != next->k)) {
                prev = temp;
                temp = next;
            } else {
                if (temp->number_of_words > next->number_of_words) {
                    temp->sibling = next->sibling;
                    next->sibling = temp->child;
                    temp->child = next;
                    temp->k++;
                } else {
                    if (prev != NULL)
                        prev->sibling = next;
                    else
                        mergedHeap = next;
                    temp->sibling = next->child;
                    next->k++;
                    next->child = temp;
                    temp = next;
                }
            }

            next = temp->sibling;
        }
        return mergedHeap;
    } else return NULL;


}

void enqueue(node_heap *heap, int number_of_words, char *filename) {   //insert element

    node_tree *tree = malloc(sizeof(node_tree));
    tree->child = NULL;
    tree->sibling = NULL;
    tree->number_of_words = number_of_words;
    tree->filename = filename;
    tree->k = 0;


    node_heap *newheap = malloc(sizeof(node_heap));


    newheap->root = tree;
    heap->root = anUnion(heap, newheap);
    free(newheap);
}


void dequeue(node_heap *heap, node_tree *root, node_tree *prev) { // remove element. it is used in findMostRelevant function.
    if (root != heap->root)
        prev->sibling = root->sibling;

    else
        heap->root = root->sibling;

    node_tree *currentCh = root->child;
    node_tree *rn = NULL;
    while (currentCh != NULL) {
        currentCh->sibling = rn;
        rn = currentCh;
        currentCh = currentCh->sibling;
    }

    node_heap *newHeap = malloc(sizeof(node_heap));
    newHeap->root = rn;
    heap->root = anUnion(heap, newHeap);
    free(newHeap);
}


node_tree *findMostRelevant(node_heap *heap) { // find maximum element and remove it from heap
    if (heap->root != NULL) {

        node_tree *MAX = heap->root;
        node_tree *previous_of_next = MAX;
        node_tree *next = MAX->sibling;


        node_tree *MAX_prev = NULL;
        while (next != NULL) {
            if (next->number_of_words > MAX->number_of_words) {
                MAX = next;
                MAX_prev = previous_of_next;
            }

            previous_of_next = next;
            next = next->sibling;
        }

        dequeue(heap, MAX, MAX_prev);
        return MAX;
    } else return NULL;
}


int main() {
    struct dirent *currentFile;
    DIR *directory = opendir(
            "/Users/morsi/workdir/projects/BinomialHeapForPriorityQueue/documents"); // TODO: please give full path.



    char word[50];
    printf("Enter word: ");
    scanf("%s", word);

    node_heap *heap = malloc(sizeof(node_heap));
    heap->root = NULL;

    while ((currentFile = readdir(directory))) {

        if (!strcmp(currentFile->d_name, "."))
            continue;
        if (!strcmp(currentFile->d_name, ".."))
            continue;

        char fullPath[100] = "/Users/morsi/workdir/projects/BinomialHeapForPriorityQueue/documents"; //TODO: please give full path
        strcat(fullPath, "/");
        strcat(fullPath, currentFile->d_name);
        FILE *f = fopen(fullPath, "rb");
        char *s = 0;
        long length;

        if (f) {
            fseek(f, 0, SEEK_END);
            length = ftell(f);
            fseek(f, 0, SEEK_SET);
            s = malloc(length);
            if (s) {
                fread(s, 1, length, f);
            }
            fclose(f);
        }


        int n, a[1000], i, j, k = 0, l, occurences = 0, t = 0;


        for (i = 0; s[i]; i++) {
            if (s[i] == ' ') {
                a[k++] = i;
            }
        }
        a[k++] = i;
        j = 0;
        for (i = 0; i < k; i++) {
            n = a[i] - j;
            if (n == strlen(word)) {
                t = 0;
                for (l = 0; word[l]; l++) {
                    if ((s[l + j] == word[l]) || (tolower(word[l]) == s[l + j]) || (toupper(word[l]) == s[l + j])) {
                        t++;
                    }
                }
                if (t == strlen(word)) {
                    occurences++;

                }
            }

            j = a[i] + 1;
        }
        enqueue(heap, occurences, currentFile->d_name);
    }


    int count = 1;
    node_tree *relevant;
    int numberOfRelevantDocuments = 0;
    while ((relevant = findMostRelevant(heap)) != NULL && count <= 5) {

        if (relevant->number_of_words > 0) {
            numberOfRelevantDocuments++;



            printf("%d. Filename: %s   Occurences: %d \n", count, relevant->filename, relevant->number_of_words);

            int c;
            FILE *file;
            char fullPath[100] = "/Users/morsi/workdir/projects/BinomialHeapForPriorityQueue/documents"; //TODO: please give full path.
            strcat(fullPath,"/");
            strcat(fullPath,relevant->filename);
            file = fopen(fullPath, "rd");
            if (file) {
                while ((c = getc(file)) != EOF)
                    putchar(c);
                fclose(file);
            }

            printf("\n\n");
            count++;


        }

    }
    printf("\nTHE NUMBER OF RELEVANT DOCUMENTS IS %d. ", numberOfRelevantDocuments);

    return 0;
}