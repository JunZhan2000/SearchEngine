//
// Created by 23276 on 2019/12/10.
// 数组实现的栈
//
#ifndef SEARCHENGINE_STACK_H
#define SEARCHENGINE_STACK_H


typedef int StackElementType;
typedef unsigned int Position;
struct SNode {
    StackElementType *Data; /* 存储元素的数组 */
    Position Top;      /* 栈顶指针 */
    int MaxSize;       /* 堆栈最大容量 */
};
typedef struct SNode *Stack;

Stack CreateStack(int MaxSize);

int IsFull(Stack S);

int Push(Stack S, StackElementType X);

int IsEmpty(Stack S);

StackElementType Pop(Stack S);

#endif //SEARCHENGINE_STACK_H
