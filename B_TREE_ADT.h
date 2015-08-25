
#ifndef B_TREE_ADT_H_INCLUDED
#define B_TREE_ADT_H_INCLUDED

#define ORDER 5

#include<stdlib.h>
#include<stdbool.h>

const int MIN_ENTRIES = ( ( (ORDER+1)/2)-1 );

// STRUCTURES
struct node;

typedef struct {
    void* dataPtr;
    struct node* rightPtr;
} ENTRY;

typedef struct node {
    struct node* firstPtr;
    int numEntries;
    ENTRY entries[ ORDER-1 ];
} NODE;

typedef struct {
    int count;
    NODE* root;
    int (*compare) (void* argu1, void* argu2);
} BTREE;

// Prototype Declarations

//__User interfaces
BTREE* BTree_Create   (int (*compare)(void* argu1, void* argu2));
void   BTree_Traverse (BTREE* tree, void (*process)(void* dataPtr));
BTREE* BTree_Destroy  (BTREE* tree);
void   BTree_Insert   (BTREE* tree, void* dataInPtr);
bool   BTree_Delete   (BTREE* tree, void* dltKey);
void*  BTree_Search   (BTREE* tree, void* dataPtr);
bool   BTree_Empty    (BTREE* tree);
bool   BTree_Full     (BTREE* tree);
int    BTree_Count    (BTREE* tree);

//__Internal BTree functions
static void* _search      (BTREE* tree, void* targetPtr, NODE* root);
static int   _searchNode  (BTREE* tree, NODE* nodePtr,void* target);
static bool  _delete      (BTREE* tree, NODE* root, void* dltKeyPtr, bool* success);
static bool  _insert      (BTREE* tree, NODE* root, void* dataInPtr, ENTRY* upEntry);
static void  _traverse    (NODE* root, void (*process)(void* dataPtr));
static void  _splitNode   (NODE* node, int entryNdx, int compResult, ENTRY* upEntry);
static void  _insertEntry (NODE* root, int entryNdx, ENTRY upEntry);
static bool  _deleteEntry (NODE* node, int entryNdx);
static bool  _deleteMid   (NODE* root, int entryNdx, NODE* leftPtr);
static bool  _reFlow      (NODE* root, int entryNdx);
static void  _borrowLeft  (NODE* root, int entryNdx, NODE* leftTree, NODE* rightTree);
static void  _borrowRight (NODE* root, int entryNdx, NODE* leftTree, NODE* rightTree);
static void  _combine     (NODE* root, int entryNdx, NODE* leftTree, NODE* rightTree);
static void  _destroy     (NODE* root);

#endif // B_TREE_ADT_H_INCLUDED

BTREE* BTree_Create   (int (*compare)(void* argu1, void* argu2)) {

    BTREE* tree;
    tree= (BTREE*)malloc(sizeof(BTREE));

    tree->count=0;
    tree->root=NULL;
    tree->compare=compare;

    return tree;
} //BTree_Create

void BTree_Traverse(BTREE* tree, void(*process)(void* dataPtr)) {
    if(tree->root) _traverse(tree->root, process);
    return;
} //end BTree_Traverse

BTREE* BTree_Destroy  (BTREE* tree) {

    if(tree==NULL) return tree;

    _destroy(tree->root);
    free(tree);
    tree=NULL;
    return tree;
} //BTree_Destroy

void BTree_Insert (BTREE* tree, void* dataInPtr) {

    bool taller;
    NODE* newPtr;
    ENTRY upEntry;
    int i;

    if(tree->root == NULL) {
        if( (newPtr = (NODE*)malloc(sizeof(NODE))) ) {
            newPtr->firstPtr =NULL;
            newPtr->numEntries=1;
            newPtr->entries[0].dataPtr=dataInPtr;
            newPtr->entries[0].rightPtr=NULL;
            tree->root = newPtr;
            (tree->count)++;

            for(i=1; i<ORDER-1; i++) {
                newPtr->entries[i].dataPtr  = NULL;
                newPtr->entries[i].rightPtr = NULL;
            } //for
        return;
        }
        else
            printf("Error 100 in BTREE_Insert\a\n"),exit(100);
    }

    taller = _insert(tree, tree->root, dataInPtr, &upEntry);

    if(taller) {
        //Tree has grown. Create new root.
        newPtr = (NODE*)malloc(sizeof(NODE));
        if(newPtr) {
            newPtr->entries[0].dataPtr=upEntry.dataPtr;
            newPtr->entries[0].rightPtr=upEntry.rightPtr;
            newPtr->firstPtr=tree->root;
            newPtr->numEntries=1;
            tree->root=newPtr;
        }
        else printf("Overflow error 101\a\n"),exit(100);
    }
    (tree->count)++;
    return;
} //BTree_Insert

bool BTree_Delete (BTREE* tree, void* dltKey) {

    bool success;
    NODE* dltPtr;

    if(!tree->root)
        return false;

    _delete(tree,tree->root,dltKey,&success);

    if(success) {
        (tree->count)--;
        if(tree->root->numEntries ==0) {
            dltPtr=tree->root;
            tree->root=tree->root->firstPtr;
            free(dltPtr);
        } //root empty
    } //success
    return success;
} //BTree_Delete

void* BTree_Search (BTREE* tree, void* targetPtr) {

    if(tree->root) return _search(tree, targetPtr, tree->root);
    else return NULL;
}   //BTree_Search

bool   BTree_Empty    (BTREE* tree) {

    if (tree->count == 0) return true;
    else return false;

} //BTree_Empty

bool   BTree_Full     (BTREE* tree) {

    ENTRY* space = (ENTRY*)malloc(sizeof(ENTRY));

    if(!space)  // if full
        return true;
    else {
        free(space);
        return false;
    }

} //BTree_Full

int    BTree_Count    (BTREE* tree) {
    return tree->count;
} //BTree_Count

static void* _search(BTREE* tree, void* targetPtr, NODE* root) {
    //Local definitions
    int entryNo;

    //statements
    if(!root) return NULL;

    if(tree->compare(targetPtr,root->entries[0].dataPtr)<0)
        return _search(tree, targetPtr,root->firstPtr);

    entryNo=root->numEntries-1;
    while(tree->compare(targetPtr,root->entries[entryNo].dataPtr)<0)
        entryNo--;
    if(tree->compare(targetPtr,root->entries[entryNo].dataPtr)==0)
        return (root->entries[entryNo].dataPtr);

    return(_search(tree,targetPtr,root->entries[entryNo].rightPtr));

} //_search

static int _searchNode(BTREE* tree, NODE* nodePtr, void* target) {

    int entryNdx;

    if( tree->compare(target,nodePtr->entries[0].dataPtr) <0 ) return 0;

    entryNdx = nodePtr->numEntries-1;

    while( tree->compare(target,nodePtr->entries[entryNdx].dataPtr) < 0  ) {
        entryNdx--;
    }
    return entryNdx;
} // _searchNode

static bool _delete(BTREE* tree, NODE* root, void* dltKeyPtr, bool* success) {
    NODE* leftPtr;
    NODE* subTreePtr;
    int entryNdx;
    int underflow;

    if(!root) {
        *success = false;
        return false;
    } //null tree

    entryNdx=_searchNode(tree,root,dltKeyPtr);
    if(tree->compare(dltKeyPtr,root->entries[entryNdx].dataPtr)==0) {
        *success=true;
        if(root->entries[entryNdx].rightPtr == NULL)
            underflow = _deleteEntry(root,entryNdx);
        else {
            if(entryNdx>0) leftPtr= root->entries[entryNdx-1].rightPtr;
            else leftPtr=root->firstPtr;

            underflow = _deleteMid(root, entryNdx,leftPtr);
            if(underflow)
                underflow = _reFlow(root, entryNdx);
        }
    }
    else {
        if(tree->compare(dltKeyPtr,root->entries[0].dataPtr)<0)
            subTreePtr = root->firstPtr;
        else
            subTreePtr=root->entries[entryNdx].rightPtr;

        underflow= _delete(tree,subTreePtr,dltKeyPtr,success);
        if(underflow)
            underflow = _reFlow(root, entryNdx);
    }
    return underflow;
} // _delete

static bool _insert(BTREE* tree, NODE* root, void* dataInPtr, ENTRY* upEntry) {

    int compResult,entryNdx;
    bool taller;

    NODE* subtreePtr;

    if(root == NULL) {
        (*upEntry).dataPtr = dataInPtr;
        (*upEntry).rightPtr= NULL;
        return true;
    }

    entryNdx = _searchNode(tree,root,dataInPtr);
    compResult= tree->compare(dataInPtr,root->entries[entryNdx].dataPtr);

    if(entryNdx <=0 && compResult <0)
        subtreePtr = root->firstPtr;
    else
        subtreePtr=root->entries[entryNdx].rightPtr;
    taller=_insert(tree,subtreePtr,dataInPtr,upEntry);

    if(taller) {
        if(root->numEntries >= ORDER -1) {
            _splitNode(root,entryNdx,compResult,upEntry);
            taller=true;
        } // node full
        else {
            if(compResult>=0)
                _insertEntry(root,entryNdx+1,*upEntry);
            else
                _insertEntry(root,entryNdx,*upEntry);
            (root->numEntries)++;
            taller=false;
        }
    }
    return taller;
} // _insert

static void _traverse(NODE* root, void(*process)(void* dataPtr)) {

    int scanCount;
    NODE* ptr;

    scanCount=0;
    ptr=root->firstPtr;

    while(scanCount<=root->numEntries) {

        if(ptr) _traverse(ptr,process);

        if(scanCount < root->numEntries) {
            process(root->entries[scanCount].dataPtr);
            ptr=root->entries[scanCount].rightPtr;
        }
        scanCount++;
    }
    return;
}

static void _splitNode(NODE* node,int entryNdx, int compResult, ENTRY* upEntry) {
    int fromNdx;
    int toNdx;
    NODE* rightPtr;

    rightPtr=(NODE*)malloc(sizeof(NODE));
    if(!rightPtr)
        printf("Overflow Error 101 in _splitNode\a\n"),exit(100);

    //Build right subtree node
    if(entryNdx<MIN_ENTRIES)
        fromNdx=MIN_ENTRIES;
    else
        fromNdx=MIN_ENTRIES+1;
    toNdx=0;
    rightPtr->numEntries=node->numEntries-fromNdx;
    while(fromNdx < node->numEntries)
        rightPtr->entries[toNdx++]=node->entries[fromNdx++];
    node->numEntries=node->numEntries-rightPtr->numEntries;

    //Insert new entry
    if(entryNdx < MIN_ENTRIES) {
        if(compResult <0)
            _insertEntry(node,entryNdx,*upEntry);
        else
            _insertEntry(node,entryNdx+1,*upEntry);
    }
    else {
        _insertEntry(rightPtr,entryNdx-MIN_ENTRIES,*upEntry);
        (rightPtr->numEntries)++;
        (node->numEntries)--;
    }

    upEntry->dataPtr=node->entries[MIN_ENTRIES].dataPtr;
    upEntry->rightPtr=rightPtr;
    rightPtr->firstPtr=node->entries[MIN_ENTRIES].rightPtr;

    return;
}  //_splitNode

static void _insertEntry(NODE* root, int entryNdx, ENTRY upEntry) {

    int shifter;

    shifter=root->numEntries;

    while( shifter > entryNdx ) {
        root->entries[shifter].dataPtr=root->entries[shifter-1].dataPtr;
        root->entries[shifter].rightPtr=root->entries[shifter-1].rightPtr;
        shifter--;
    }

    root->entries[entryNdx].dataPtr=upEntry.dataPtr;
    root->entries[entryNdx].rightPtr=upEntry.rightPtr;
} // _insertEntry

static bool _deleteEntry (NODE* node, int entryNdx) {

    int shifter;

    shifter=entryNdx;

    free(node->entries[shifter].dataPtr);

    while(shifter< node->numEntries-1) {
        node->entries[shifter].dataPtr = node->entries[shifter+1].dataPtr;
        shifter++;
    }
    --node->numEntries;

    if(node->numEntries< MIN_ENTRIES) return true;
    else return false;
} // _deleteEntry

static bool _deleteMid(NODE* root, int entryNdx, NODE* subtreePtr) {
    int dltNdx;
    int rightNdx;
    bool underflow;

    if(subtreePtr->firstPtr == NULL ) {
        dltNdx = subtreePtr->numEntries-1;
        free(root->entries[entryNdx].dataPtr); // I added.
        root->entries[entryNdx].dataPtr=subtreePtr->entries[dltNdx].dataPtr;
        --subtreePtr->numEntries;
        underflow=subtreePtr->numEntries<MIN_ENTRIES;
    }
    else {
        rightNdx = subtreePtr->numEntries-1;
    underflow=_deleteMid(root,entryNdx,subtreePtr->entries[rightNdx].rightPtr);
    if(underflow)
        underflow=_reFlow(subtreePtr,rightNdx);
    }
    return underflow;
} //deleteMid

static bool _reFlow(NODE* root, int entryNdx) {
    NODE* leftTreePtr;
    NODE* rightTreePtr;
    bool underflow;

    if(entryNdx == 0)
        leftTreePtr = root->firstPtr;
    else
        leftTreePtr = root->entries[entryNdx-1].rightPtr;
    rightTreePtr = root->entries[entryNdx].rightPtr;

    if(rightTreePtr->numEntries > MIN_ENTRIES) {
        _borrowRight(root,entryNdx,leftTreePtr,rightTreePtr);
        underflow = false;
    } //if borrow right
    else {
        if(leftTreePtr->numEntries > MIN_ENTRIES) {
            _borrowLeft(root,entryNdx,leftTreePtr,rightTreePtr);
            underflow=false;
        } //if borrow left
        else { //can't borrow. Must combine nodes.
            _combine(root, entryNdx,leftTreePtr,rightTreePtr);
            underflow=( root->numEntries < MIN_ENTRIES );
        } //else combine
    } //else borrow right
    return underflow;
} // _reFlow

static void _borrowLeft(NODE* root,int entryNdx, NODE* leftTreePtr, NODE* rightTreePtr) {

    int shifter;

    shifter=rightTreePtr->numEntries;

    while(shifter>0) {
        rightTreePtr->entries[shifter].dataPtr=rightTreePtr->entries[shifter-1].dataPtr;
        rightTreePtr->entries[shifter].rightPtr=rightTreePtr->entries[shifter-1].rightPtr;
        shifter--;
    }

    rightTreePtr->entries[0].dataPtr=root->entries[entryNdx].dataPtr;
    rightTreePtr->firstPtr=leftTreePtr->entries[leftTreePtr->numEntries-1].rightPtr;
    ++rightTreePtr->numEntries;

    root->entries[entryNdx].dataPtr=leftTreePtr->entries[leftTreePtr->numEntries-1].dataPtr;
    --leftTreePtr->numEntries;

    return;
} // _borrowLeft

static void _borrowRight(NODE* root, int entryNdx, NODE* leftTreePtr, NODE* rightTreePtr) {
    int toNdx;
    int shifter;

    toNdx=leftTreePtr->numEntries;
    leftTreePtr->entries[toNdx].dataPtr=root->entries[entryNdx].dataPtr;
    leftTreePtr->entries[toNdx].rightPtr=rightTreePtr->firstPtr;
    ++leftTreePtr->numEntries;

    //Move right data to parent
    root->entries[entryNdx].dataPtr=rightTreePtr->entries[0].dataPtr;

    // Set right tree first pointer. shift entries left
    rightTreePtr->firstPtr = rightTreePtr->entries[0].rightPtr;
    shifter = 0;

    while(shifter < rightTreePtr->numEntries - 1) {
        rightTreePtr->entries[shifter] = rightTreePtr->entries[shifter+1];
        ++shifter;
    }
    --rightTreePtr->numEntries;
    return;
} // _borrowRight

static void _combine (NODE*root, int entryNdx, NODE* leftTreePtr, NODE* rightTreePtr) {
    int toNdx, fromNdx, shifter;

    toNdx = leftTreePtr->numEntries;
    leftTreePtr->entries[toNdx].dataPtr=root->entries[entryNdx].dataPtr;
    leftTreePtr->entries[toNdx].rightPtr=rightTreePtr->firstPtr;
    ++leftTreePtr->numEntries;
    --root->numEntries;

    fromNdx = 0;
    toNdx++;
    while(fromNdx<rightTreePtr->numEntries)
        leftTreePtr->entries[toNdx++] = rightTreePtr->entries[fromNdx++];

    leftTreePtr->numEntries += rightTreePtr->numEntries;
    free(rightTreePtr);

    shifter = entryNdx;
    while(shifter < root->numEntries) {
        root->entries[shifter] = root->entries[shifter+1];
        shifter++;
    }
    return;
} // _combine

static void  _destroy     (NODE* root) {

    int shifter=0;

    if(root==NULL) return;

    _destroy(root->firstPtr);
    while(shifter  <  root->numEntries) {
            free(root->entries[shifter].dataPtr);
            _destroy(root->entries[shifter].rightPtr);
            shifter++;
    }
    free(root);
    return;
} // _destroy
