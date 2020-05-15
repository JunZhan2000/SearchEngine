//
// Created by 23276 on 2019/12/9.
//

#include <stdlib.h>
#include "utils.h"
#include "hash.h"
#include "bool_search.h"

/*
 * 向倒排表中插入节点
 * table: 被插入的倒排表
 * newNode: 需要插入的节点
 * order: 插入的位置
 * */
void InsertNode(InvertedTable table, InvertedTable newNode, int order) {
    InvertedTable move = table;
    for (int i = 0; i < order; i++) {
        move = move->next;
    }
    if (move == table) {  //需要插入到首部
        newNode->next = table;
        table = newNode;
    } else if (move == NULL) {  //需要插入到尾部
        //先定位到相应位置
        move = table;
        while (move->next != NULL) {
            move = move->next;
        }
        move->next = newNode;
        newNode->next = NULL;
    } else {  //需要插入到中间位置
        newNode->next = move;
        move->next = newNode;
    }
}

/*
 * 复制一个倒排表节点
 * oldNode: 旧节点指针
 * return: 新节点指针
 * */
InvertedTable CopyNode(InvertedTable oldNode) {
    InvertedTable newNode = (InvertedTable) malloc(sizeof(struct InvertedTable));  //新建节点
    //复制属性
    newNode->docID = oldNode->docID;
    newNode->frequency = oldNode->frequency;
    newNode->positions = CopyTermPs(oldNode->positions);
    newNode->next = NULL;

    return newNode;
}

/*
 * 在链表尾部添加新节点
 * answerPtr: 被插入的表
 * t: 被复制后插入的节点
 * */
void AddNode(InvertedTable *answerPtr, InvertedTable t) {
    if (*answerPtr == NULL) {  //answer第一次插入节点
        *answerPtr = CopyNode(t);  //直接用表头获取深拷贝的节点
    } else {
        InvertedTable tail = *answerPtr;
        while (tail->next != NULL) {
            tail = tail->next;
        }
        tail->next = CopyNode(t);
    }
}

/*
 * 在链表尾部添加新节点
 * answerPtr: 被插入的表
 * docId: 新节点的docID
 * */
void AddNodeByID(InvertedTable *answerPtr, int docID) {
    InvertedTable newNode = (InvertedTable) malloc(sizeof(struct InvertedTable));
    newNode->docID = docID;
    newNode->frequency = 1;
    newNode->next = NULL;

    if (*answerPtr == NULL) {  //answer第一次插入节点
        *answerPtr = newNode;
    } else {  //不知道为什么 *move->next这样的写法会报错，这里暂且用一个新节点实现功能
        InvertedTable tail = *answerPtr;
        while (tail->next != NULL) {
            tail = tail->next;
        }
        tail->next = newNode;
    }
}

/*
 * 判断是否为操作符
 * ch: 被判断的字符
 * */
int IsOperator(char ch) {
    char ops[] = "&|!";

    for (int i = 0; i < 3; i++) {
        if (ch == ops[i])
            return 1;
    }

    return 0;
}

/*
 * 输出一个倒排记录表
 *
 * table: 被输出的表
 * */
void PrintTable(InvertedTable table) {
    InvertedTable tableMove = table;
    printf("docIDs:  ");
    while (tableMove != NULL) {
        printf("%d  ", tableMove->docID);
        tableMove = tableMove->next;
    }

    if(table == NULL){
        printf("FOUND NOTHING");
    }
    printf("\n");
}

/*
 * 在表达式表末尾插入运算对象
 * */
void InsertItem(ExpItem exp, InvertedTable table){
    ExpItem expTail = exp;
    while (expTail->next != NULL) {  //定位到表达式链尾部
        expTail = expTail->next;
    }
    expTail->next = (ExpItem) malloc(sizeof(struct ExpItem));
    expTail = expTail->next;
    expTail->type = 1;
    expTail->table = table;
    expTail->next = NULL;
}

/*
 * 在表达式表末尾插入运算符
 * */
void InsertOperator(ExpItem exp, char operator){
    ExpItem expTail = exp;
    while (expTail->next != NULL) {  //定位到表达式链尾部
        expTail = expTail->next;
    }
    expTail->next = (ExpItem) malloc(sizeof(struct ExpItem));
    expTail = expTail->next;
    expTail->type = 2;
    expTail->operator = operator;
    expTail->next = NULL;
}

/*
 * 深拷贝一个词项位置链表
 * */
TermPositions CopyTermPs(TermPositions positions){
    TermPositions newPositions = NULL, tail, move = positions;

    while(move != NULL){
        if(newPositions == NULL){
            newPositions = (TermPositions) malloc(sizeof(struct TermPosition));
            newPositions->p = move->p;
            newPositions->next = NULL;
            tail = newPositions;
        } else {
            tail->next = (TermPositions) malloc(sizeof(struct TermPosition));
            tail->next->p = move->p;
            tail->next->next = NULL;
            tail = tail->next;
        }
        move = move->next;
    }

    return newPositions;
}

/*
 * 在词项位置链表末尾插入p节点
 * */
void AddNodeToTermP(TermPositions *termPositions, int p){
    TermPosition newNode = (TermPositions) malloc(sizeof(struct TermPosition));
    newNode->p = p;
    newNode->next = NULL;

    if(*termPositions == NULL){
        *termPositions = newNode;
    } else {
        TermPosition move = *termPositions;
        while(move->next != NULL){
            move = move->next;
        }
        move->next = newNode;
    }
}
