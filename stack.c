//
// Created by 23276 on 2019/12/10.
//

#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

Stack CreateStack( int MaxSize )
{
    Stack S = (Stack)malloc(sizeof(struct SNode));
    S->Data = (StackElementType *)malloc(MaxSize * sizeof(StackElementType));
    S->Top = -1;
    S->MaxSize = MaxSize;
    return S;
}

int IsFull( Stack S )
{
    if(S->Top == S->MaxSize-1) {
        return 1;
    } else{
        return 0;
    }
}

int Push( Stack S, StackElementType X )
{
    if ( IsFull(S) ) {
        printf("堆栈满");
        return 0;
    }
    else {
        S->Data[++(S->Top)] = X;
        return 1;
    }
}

int IsEmpty( Stack S )
{
    if(S->Top == -1) {
        return 1;
    } else{
        return 0;
    }
}

StackElementType Pop( Stack S )
{
    if ( IsEmpty(S) ) {
        printf("堆栈空");
        return '$'; /* ERROR是ElementType的特殊值，标志错误 */
    }
    else
        return ( S->Data[(S->Top)--] );
}