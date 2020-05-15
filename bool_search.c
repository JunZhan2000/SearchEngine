//
// Created by 23276 on 2019/12/9.
//

#include <stdlib.h>
#include <string.h>
#include "bool_search.h"
#include "utils.h"
#include "stack.h"

extern int fileIDs[10];
extern int IDLength;
extern HashTable HT;

/*
 * 执行一次搜索
 * HT: 倒排索引的指针
 * */
void BoolSearch() {
    char command[100];  //command用于存储命令，buffer用于临时存储词条

    printf("please enter the command (enter q to return):\n");
    while(1){
        gets(command);
        if(strcmp(command, "q") == 0){
            break;
        }
        InvertedTable answer = ComPuteMidExp(command);  //计算得最终的倒排表
        PrintTable(answer);  //输出结果
    }
}


/*
 * 两个倒排表取交集，即并操作
 * t1, t2: 两个倒排表
 *
 * return: 两表的交集
 * */
InvertedTable Intersect(InvertedTable t1, InvertedTable t2) {
    InvertedTable answer = NULL;

    while (t1 != NULL && t2 != NULL) {
        if (t1->docID == t2->docID) {  //有相同的文档ID
            AddNode(&answer, t1);
            t1 = t1->next;
            t2 = t2->next;
        } else if (t1->docID < t2->docID) {
            //由于倒排表中docID是顺序排序的，所有可以直接顺延
            t1 = t1->next;
        } else {
            t2 = t2->next;
        }
    }

    return answer;
}

/*
 * 两个倒排表取并集合，即或操作
 * t1, t2: 两个倒排表
 *
 * return: 两表的并集
 * */
InvertedTable Union(InvertedTable t1, InvertedTable t2) {
    InvertedTable answer = NULL;

    while (t1 != NULL && t2 != NULL) {
        if (t1->docID == t2->docID) {  //有相同的文档ID
            AddNode(&answer, t1);
            t1 = t1->next;
            t2 = t2->next;
        } else if (t1->docID < t2->docID) {
            while (t1 != NULL && t1->docID < t2->docID) {
                AddNode(&answer, t1);
                t1 = t1->next;
            }
        } else {
            while (t2 != NULL && t2->docID < t1->docID) {
                AddNode(&answer, t2);
                t2 = t2->next;
            }
        }
    }

    if (t1 != NULL) {
        while (t1 != NULL) {
            AddNode(&answer, t1);
            t1 = t1->next;
        }
    } else {
        while (t2 != NULL) {
            AddNode(&answer, t2);
            t2 = t2->next;
        }
    }

    return answer;
}


/*
 * 取倒排表的补集，即非操作
 * t: 倒排表
 * fileIDs: 所有文档ID的集合，即全集
 *
 * return: t表的补集
 * */
InvertedTable Complement(InvertedTable t) {
    InvertedTable answer = NULL, move = t;
    int i;

    //向answer中添加在全集中但不在t中的节点，直到t被遍历完（全集一定大于等于t）
    for (i = 0; i < IDLength && move != NULL; i++) {
        if (fileIDs[i] < move->docID) {
            AddNodeByID(&answer, fileIDs[i]);
        } else {  //遇到相同的ID或者倒排表中的docID比全集中的小，跳过此节点
            move = move->next;
        }
    }

    //再加上剩余的所有节点
    for (; i < IDLength; i++) {
        AddNodeByID(&answer, fileIDs[i]);
    }

    return answer;
}

/*
 * 解析中缀表达式并计算结果
 * midExp: 中缀表达式
 *
 * return: 计算得出的倒排索引表
 * */
InvertedTable ComPuteMidExp(char *midExp) {
    char buffer[100], *sub, ch;  //buffer用于临时存储词条，move用于遍历表达式
    int chIndex = 0, expIndex = 0, qufanFlag = 0, cizuFlag = 0;
    InvertedTable expItem;
    Position p;
    Stack S = CreateStack(100);  //创建一个栈

    //创建表达式
    Exp exp = (Exp) malloc(sizeof(struct ExpItem));  //用于存储除去括号和!的表达式
    exp->type = 0;
    exp->next = NULL;

    //China & ! (public | ! (country | you))

    while (midExp[expIndex] != '\0') {  //依次读取命令的每一个字符并做出判断
        ch = midExp[expIndex];
        if (!(IsOperator(ch) || ch == '(' || ch == ')' || ch == ' ' || ch == '"') && IsEmpty(S)) {
            //该字符不是操作符且不在括号内
            buffer[chIndex++] = ch;  //存入缓冲区
        } else {
            if (chIndex != 0 && IsEmpty(S)) {
                buffer[chIndex] = 0;  //在词项结尾添加0
                p = Find(HT, buffer);  //获取词项的倒排索引表的位置
//              printf("item: %s   position: %d\n", buffer, p);
                expItem = HT->Cells[p].table;
                if(qufanFlag == 1){
                    expItem = Complement(expItem);
                    qufanFlag = 0;
                }
                InsertItem(exp, expItem);  //向表达式添加操作对象
                chIndex = 0;
            }

            if (IsOperator(ch) && IsEmpty(S)) {  //若该字符是操作符号且不在括号内
                if(ch == '!'){
                    qufanFlag = 1;
                } else {
                    InsertOperator(exp, ch);  //向表达式添加操作符
                }
            } else if (midExp[expIndex] == '(') {
                Push(S, expIndex);  //左括号的位置入栈
            } else if (midExp[expIndex] == ')') {
                int left = Pop(S) + 1;  //left为左括号右边的第一个字符的位置，endIndex此时为右括号的位置
                if(IsEmpty(S)){
                    char temExp[100];
                    strncpy(temExp, midExp + left, expIndex - left);
                    temExp[expIndex - left] = '\0';
                    expItem = ComPuteMidExp(temExp);
                    if(qufanFlag == 1){
                        expItem = Complement(expItem);
                        qufanFlag = 0;
                    }
                    InsertItem(exp, expItem);  //向表达式添加操作对象
                }
            } else if (midExp[expIndex] == '\"'){
                if(cizuFlag == 0){
                    Push(S, expIndex);  //左引号的位置入栈
                    cizuFlag = 1;
                } else {
                    int left = Pop(S) + 1;  //left为左引号右边的第一个字符的位置，endIndex此时为右括号的位置
                    if (IsEmpty(S)){
                        char temExp[100];
                        strncpy(temExp, midExp + left, expIndex - left);
                        temExp[expIndex - left] = '\0';
                        expItem = ComputeCiZuExp(temExp);
                        if(qufanFlag == 1){
                            expItem = Complement(expItem);
                            qufanFlag = 0;
                        }
                        InsertItem(exp, expItem);  //向表达式添加操作对象
                    }
                    cizuFlag = 0;
                }

            }
        }
        expIndex++;
    }

    if (chIndex != 0) {
        //如果命令尾部有词项
        buffer[chIndex] = 0;  //在词项结尾添加0
        p = Find(HT, buffer);  //获取词项的倒排索引表的位置
        expItem = HT->Cells[p].table;
        if(qufanFlag == 1){
            expItem = Complement(expItem);
            qufanFlag = 0;
        }
        InsertItem(exp, expItem);  //向表达式添加操作对象
    }

    return ComPuteSimpleExp(exp);
}

/*
 * 计算只有&和|的简单表达式
 * */
InvertedTable ComPuteSimpleExp(Exp exp) {
    char operator;
    ExpItem expMove = exp->next;
    InvertedTable item1 = NULL, item2 = NULL;

    if(expMove->type == 1 && expMove->next == NULL){
        return expMove->table;
    }

    while (expMove != NULL) {
        if (expMove->type == 1) {
            if (item1 == NULL) {  //二元运算的第一项
                item1 = expMove->table;
            } else {  //二元运算的第二项，计算值
                item2 = expMove->table;
                switch (operator) {
                    case '&':
                        item1 = Intersect(item1, item2);
                        break;
                    case '|':
                        item1 = Union(item1, item2);
                        break;
                }
            }
        } else if (expMove->type == 2) {
            operator = expMove->operator;
        }
        expMove = expMove->next;
    }

    return item1;
}

/*
 * 获取一个不定长词组对应的的倒排表
 * */
InvertedTable ComputeCiZuExp(char *exp){
    InvertedTable item1 = NULL, item2 = NULL;
    char buffer[100], *move = exp;
    int bufferIndex = 0;

    while(*move != '\0'){
        if(*move == ' ' && bufferIndex != 0){
            buffer[bufferIndex] = '\0';
            bufferIndex = 0;
            Position p = Find(HT, buffer);
            if(HT->Cells[p].info != Legitimate){
                return NULL;
            }
            if(item1 == NULL){
                item1 = HT->Cells[p].table;
            } else {
                item2 = HT->Cells[p].table;
                item1 = PositionalIntersect(item1, item2);
            }
        } else if(*move != ' '){
            buffer[bufferIndex++] = *move;
        }
        move++;
    }
    if(bufferIndex != 0){
        buffer[bufferIndex] = '\0';
        Position p = Find(HT, buffer);
        if(HT->Cells[p].info != Legitimate){
            return NULL;
        }
        item2 = HT->Cells[p].table;
        item1 = PositionalIntersect(item1, item2);
    }

    return item1;
}

/*
 * 近邻搜索
 * t1, t2: 两个词项的倒排记录表
 *
 * return: 合并后的倒排记录表
 * */
InvertedTable PositionalIntersect(InvertedTable t1, InvertedTable t2){
    InvertedTable answer = NULL;

    while(t1 != NULL && t2 != NULL){
        if(t1->docID == t2->docID){
            TermPosition p1 = t1->positions, p2 = t2->positions, p0 = NULL;
            int pNum = 0;

            while(p1 != NULL && p2 != NULL){
                if(p2->p == p1->p + 1){
                    AddNodeToTermP(&p0, p2->p);
                    pNum++;
                    p1 = p1->next;
                    p2 = p2->next;
                } else if (p2->p <= p1->p){
                    p2 = p2->next;
                } else {
                    p1 = p1->next;
                }
            }

            if(p0 != NULL){
                InvertedTable newNode = (InvertedTable) malloc(sizeof(struct InvertedTable));
                newNode->docID = t2->docID;
                newNode->positions = p0;
                newNode->frequency = pNum;
                newNode->next = NULL;
                if (answer == NULL) {  //answer第一次插入节点
                    answer = newNode;  //直接用表头获取深拷贝的节点
                } else {
                    InvertedTable tail = answer;
                    while (tail->next != NULL) {
                        tail = tail->next;
                    }
                    tail->next = newNode;
                }
            }
            t1 = t1->next;
            t2 = t2->next;
        } else if (t1->docID < t2->docID){
            t1 = t1->next;
        } else {
            t2 = t2->next;
        }
    }

    return answer;
}