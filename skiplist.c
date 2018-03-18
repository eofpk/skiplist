#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LEVEL 8

typedef struct Node{
    int key;
    int value;
    struct Node *forward[1];
}Node;
typedef struct Node* pNode;

typedef struct SkipList{
    int level;
    pNode header;
    pNode tail;
}SkipList;
typedef struct SkipList* pSkipList;

int randomLevel() {
    int k=0;
    while (rand()%2) {
        k++;
    }   
    k = k < MAX_LEVEL ? k : MAX_LEVEL;
    return k;
}

void printSL(pSkipList sl) {
    pNode p, q;
    int k=sl->level;
    for(int i=k; i >= 0; i--) {
        p = sl->header->forward[i];
        printf("level[%d]: ",i);
        while(p->forward[i] != sl->tail) {
            printf("(%d,%d) -> ", p->key, p->value);
            p = p->forward[i];
        }   
        printf("(%d,%d)\n", p->key, p->value);
    }   
    printf("\n");
}

pNode create(int level, int key, int value){
    int size = sizeof(Node) + level * sizeof(pNode);
    pNode node = (pNode)malloc(size);
    node->key = key;
    node->value = value;
    return node;
}

pSkipList createSkipList(){
    pSkipList sl = (pSkipList)malloc(sizeof(SkipList));
    sl->level = 0;
    sl->header = create(MAX_LEVEL, 0, 0);
    sl->tail = create(0, 0x7fffffff, 0);
    for(int i = 0 ; i <= MAX_LEVEL ; i ++){
        /* 注意此处虽然数组越界，但是由于申请的连续内存则无碍 */
        sl->header->forward[i]= sl->tail;
    }
    return sl;
}

int insert(pSkipList sl,int key,int value){
    pNode update[MAX_LEVEL];
    pNode p, q;
    p = sl->header;
    int k = sl->level;
    for(int i = k ; i >=0 ; i --){
        while(p->forward[i]->key < key) {
            p = p->forward[i];
        }
        update[i] = p;
    }
    q = p->forward[0];
    if(q->key == key){
        return 0;
    }

    k = randomLevel();
    printf("NewLevel: %d, CurLevel: %d\n", k, sl->level);
    if(k > sl->level){
        for(int i = sl->level + 1 ; i <= k ; i ++)
            update[i] = sl->header;
        sl->level = k;
    }

    q = create(k, key, value);
    for(int i = 0 ; i <= k ; i ++){
        q->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = q;
    }
    printSL(sl);
    return 1;
}

int delete(pSkipList sl,int key){
    pNode update[MAX_LEVEL];
    pNode p,q;
    p = sl->header;
    int k = sl->level;
    for(int i = k ; i >=0 ; i --){
        while(p->forward[i]->key < key) {
            p = p->forward[i];
        }
        update[i] = p;
    }
    q = p->forward[0];
    if(q && q->key == key){
        for(int i = 0 ; i <= sl->level; i ++){
            if(update[i]->forward[i] != q)
                break;
            update[i]->forward[i] = q->forward[i];
        }
        free(q);
        for(int i = sl->level ; i > 0 ; i --){
            if(sl->header->forward[i] == sl->tail){
                sl->level --;
            }
        }
        return 1;
    }
    else{
        return 0;
    }
}

int search(pSkipList sl,int key){
    pNode p, q;
    p = sl->header;
    int k = sl->level;
    for(int i = k ; i >= 0 ; i --){
        while((q = p->forward[i]) && q->key <= key){
            if(q->key == key)
                return q->value;
            p = q;
        }
    }
    return NULL;
}

int main() {
    srand((int)time(0));
    pSkipList sl = createSkipList();
    for(int i = 1;i <= 16;i++)
    {
        int a = insert(sl, i, i*3);
    }

    printf("find 10, ret=%d\n\n", search(sl, 10));

    printf("delete node 6\n");
    delete(sl, 6);
    printSL(sl);

    printf("find 6, ret=%d\n", search(sl, 6));
    return 0;
}
