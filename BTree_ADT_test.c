#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "B_TREE_ADT.h"

int custom_compare(void* argu1, void* argu2);
void print_tree(NODE* root, int level);

int main() {
    BTREE* the_tree;
    char mode;
    int i=0,*ptr,check;

    //srand(time(NULL));

    the_tree = BTree_Create(custom_compare);

    for(i=0;i<15;i++) {
        ptr=(int*)malloc(sizeof(int));
        *ptr=( 1 + rand()/999 );
        BTree_Insert(the_tree,ptr);

        printf("\n%d inserted into B-Tree.\n",*ptr);
        print_tree(the_tree->root,0);
    }



    while(1) {
        printf("\n######### B-Tree menu-driven interface #########\n");
        printf("Choose the operation you want to execute\n");
        printf("d : delete      \t");
        printf("i : insert      \n");
        printf("p : print B-tree\t");
        printf("c : tree count  \n");
        printf("else : quit     \n");
        fscanf(stdin,"%c",&mode);fflush(stdin);

        switch(mode)
        {
        case 'd': //delete
            printf("\nEnter the key to delete. ");
            fscanf(stdin,"%d",&check);fflush(stdin);
            BTree_Delete(the_tree,&check);
            printf("\n%d deleted from B-Tree.\n",check);
        break;

        case 'i': //insert
            ptr=(int*)malloc(sizeof(int));
            printf("\nEnter the key you'd like to insert. ");
            fscanf(stdin,"%d",ptr);fflush(stdin);
            BTree_Insert(the_tree,ptr);
            printf("\n%d inserted into B-Tree.\n",*ptr);
        break;

        case 'p': // print B-Tree
            print_tree(the_tree->root,0);
        break;

        case 'c': // tree count.
            printf("B-TREE COUNT : %d entries \n",BTree_Count(the_tree));
        break;

        default: exit(0);
        }
    }

    return 0;
}

int custom_compare(void* argu1, void* argu2) {

    int a1 = *(int*)argu1;
    int a2 = *(int*)argu2;

    if( a1 >a2 )
        return 1;
    else if(a1 <a2)
        return -1;
    else return 0;
} //compare

void print_tree(NODE* root, int level) {
    int i,j;
 	if(root != NULL) {
            print_tree(root->firstPtr,level+1);
 		for(i=0; i<root->numEntries; i++) {
 			for(j=0; j < level; j++)
 				printf("|  ");
 			printf("+[%d]\n", *(int*)(root->entries[i].dataPtr) );
 			print_tree(root->entries[i].rightPtr,level+1);
 		}
 	}
} // print_tree
