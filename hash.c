//
// Created by 23276 on 2019/12/4.
//

/*
1. 哈希表数据结构的实现
2. 统计一个文本中单词的词频
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "utils.h"

extern int fileIDs[10];
extern int IDLength;

//初始化一个哈希表并返回
HashTable InitHashTable(int TableSize) {
    HashTable HT = (HashTable) malloc(sizeof(struct HashTable));

    HT->TableSize = TableSize;
    HT->Cells = (Cell) malloc(TableSize * (sizeof(struct TblNode)));     //给表开辟相应的空间

    for (int i = 0; i < TableSize; i++) {
        HT->Cells[i].info = Empty;
        HT->Cells[i].table = NULL;
    }

    return HT;
}


//输出哈希表
void ShowTable(HashTable HT) {
    for (int i = 0; i < HT->TableSize; i++) {
        if (HT->Cells[i].info != Legitimate) {
            printf("%d: Not Inserted\n", i);
        } else {
            printf("%d: word:%s\t\tdocFrequency:%d\n", i, HT->Cells[i].wordTerm, HT->Cells[i].docFrequency);
            PrintTable(HT->Cells[i].table);
        }
        printf("\n");
    }
}


//映射字符串的哈希函数
Position StrHash(char *str, int TableSize) {
    Position p = 0;

    while (*str != 0) {
        //获取对应的128进制的数字
        p = (p << 5) + *str;
        str++;
    }
    return p % TableSize;
}


//查找元素E在HT中的位置，查到则返回对应的位置，否则返回应该插入的位置
Position Find(HashTable HT, ElementType E) {
    Position p, newP;
    int i = 1;      //探测次数

    newP = p = StrHash(E, HT->TableSize);     //根据哈希函数映射出一个存放的位置

    while (HT->Cells[newP].info != Empty && strcmp(E, HT->Cells[newP].wordTerm) != 0) {
        //该位置上不是E，继续寻找下一个位置，使用平方探测法
        if (i++ % 2 == 0) {
            //偶数次
            newP = p - ((i + 1) / 2) * ((i + 1) / 2);       //向左探测更新地址
            while (newP < 0) {
                newP += HT->TableSize;                  //调整至合法地址
            }
        } else {
            newP = p + ((i + 1) / 2) * ((i + 1) / 2);       //向右探测更新地址
            if (newP > 0) {
                newP = newP % HT->TableSize;            //调整至合法地址
            }
        }
    }

    return newP;
}

/*
 * 将词项插入到哈希表中或者增大对应的记录
 *
 * HT: 被插入的哈希表
 * E: 插入的词项
 * docID: 文档ID
 * */
void InsertOrCount(HashTable HT, ElementType E, int docID, int termPosition) {
    Position p = Find(HT, E);       //先查找E是否存在于表HT中

    if (HT->Cells[p].info != Legitimate) {
        //若该位置未被占据，插入
        HT->Cells[p].info = Legitimate;
        HT->Cells[p].wordTerm = E;
        HT->Cells[p].docFrequency = 1;

        HT->Cells[p].table = (InvertedTable) malloc(sizeof(struct InvertedTable));
        HT->Cells[p].table->docID = docID;      //记录docID
        HT->Cells[p].table->frequency = 1;  //记录该文档中出现的频率
        HT->Cells[p].table->positions = (TermPositions) malloc(sizeof(struct TermPosition));
        HT->Cells[p].table->positions->p = termPosition;
        HT->Cells[p].table->positions->next = NULL;
        HT->Cells[p].table->next = NULL;    //标记next为NULL
    } else {
        //词典中已有该词项，判断对应倒排表中是否有本文档的记录
        InvertedTable move = HT->Cells[p].table;  //用于遍历链表
        int order = 0;  //用于记录节点位置

        while (move != NULL && docID > move->docID) {
            //若未找到对应的文档ID且倒排表遍历未结束，则继续循环
            move = move->next;
            order++;
        }
        if (move != NULL && move->docID == docID) {
            //对应的倒排表中已有本文档的记录
            move->frequency++;  //对应倒排表节点的词频加一
            //插入位置信息，因为位置是越来越大的，所有直接插到结尾即可。且由于已有本文档的记录，所有不用管理链表为空时的情形
            TermPositions positionMove = move->positions;
            while (positionMove->next != NULL) {
                positionMove = positionMove->next;
            }
            positionMove->next = (TermPosition) malloc(sizeof(struct TermPosition));
            positionMove->next->p = termPosition;
            positionMove->next->next = NULL;
        } else {
            //对应的倒排表中没有本文档的记录
            InvertedTable newNode = (InvertedTable) malloc(sizeof(struct InvertedTable));  //新建倒排表节点
            newNode->docID = docID;  //记录文档ID
            newNode->frequency = 1;  //初始化词频
            newNode->positions = NULL;
            AddNodeToTermP(&newNode->positions, termPosition);

            InsertNode(HT->Cells[p].table, newNode, order);

            HT->Cells[p].docFrequency++;  //词项的文档频率加1
        }
    }
}

/*
 * 建立倒排索引
 *
 * return: 建立好的倒排索引（哈希表）
 * */
HashTable BuildIndex() {
    int length, wordCounts = 0, TableSize = 3000, wordOrder = 1;
    char *word = NULL;
    FILE *fp;
    char fileID[10], buffer[100];
    Position p;

    HashTable HT = InitHashTable(TableSize);    //初始化一张哈希表

    for (int i = 0; i < IDLength; i++) {
        //拼接处文件路径的字符串，如../static/0.txt
        char txt[50] = "../static/";
        itoa(fileIDs[i], fileID, 10);
        strcat(txt, fileID);
        strcat(txt, ".txt");
        wordOrder = 1;

        if ((fp = fopen(txt, "r")) == NULL) {
            //未能成功打开文件
            printf("Open failed: %s\n", txt);
            exit(0);
        }
        while (!feof(fp)) {
            length = ReadAWord(fp, &word);      //从文本中读取一个单词
            if (length >= 3) {
                //如果读出的字符串长度大于等于4，我们将其视为单词，主要是由于在表的size很大的情况下，
                wordCounts++;               //读取的单词数加1
                InsertOrCount(HT, word, fileIDs[i], wordOrder++);    //若该单词第一次出现，插入表中；若已存在于表中，词频加一
            } else {
                //否则，弃之，释放空间
                free(word);
            }
        }
        fclose(fp);
    }
    return HT;
}


//从文件中读取一个单词至E，返回单词的长度
int ReadAWord(FILE *fp, ElementType *E) {
    char buffer[21];                //缓冲区，最多读取长度为20的字符串
    int j = 0;
    char ch;
    for (int i = 0; i < 100; i++) {
        ch = fgetc(fp);      //读取一个字符
        if (ch < 48 || (ch >= 58 && ch <= 64) || (ch >= 91 && ch <= 96 && ch != 95) || ch > 122) {
            //如果读到的字符是分隔符，退出循环不再读取
            if (j != 0) {
                //缓冲区内已经读到了字符
                break;
            }
        } else {
            buffer[j++] = ch;   //读入缓冲区中
        }
    }
    buffer[j] = 0;          //在字符串结尾设置0
    *E = (ElementType) (char *) malloc((strlen(buffer) + 1) * sizeof(char));
    strcpy(*E, buffer);

    return j;
}


//输出词频前一百的单词
void RankHashTbl(HashTable HT) {
    int empty = 0;
    int numMost[100];
    for (int i = 0; i < 100; i++) {
        numMost[i] = -1;
    }

    for (int i = 0; i < HT->TableSize; i++) {
        if (HT->Cells[i].info == Empty) {
            empty++;
        }
        if (HT->Cells[i].info == Legitimate &&
            (numMost[99] == -1 || HT->Cells[i].docFrequency > HT->Cells[numMost[99]].docFrequency)) {
            //若该位置处存在单词，且其词频足以排进目前的前100，则继续确定其排位
            int rank = 99;
            while (rank >= 0 &&
                   (numMost[rank] == -1 ||
                    HT->Cells[i].docFrequency > HT->Cells[numMost[rank]].docFrequency)) {//找到这个单词应该排的位置
                rank--;
            }
            //此时word的下标应为rank+1
            for (int j = 98; j > rank; j--) {
                numMost[j + 1] = numMost[j];
            }
            numMost[rank + 1] = i; //记录这个单词的下标
        }
    }
    printf("\n\n\nTop 100:\n");
    for (int i = 0; i < 100; i++) {
        struct TblNode cell = HT->Cells[numMost[i]];
        printf("%d: word:%s\t\tdocFrequency:%d\n", i, cell.wordTerm, cell.docFrequency);
        PrintTable(cell.table);  //输出该表
    }
    printf("\nalpha: %f\n", (double) (HT->TableSize - empty) / HT->TableSize);
}