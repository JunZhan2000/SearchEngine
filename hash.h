//
// Created by 23276 on 2019/12/4.
//

#ifndef SEARCHENGINE_HASH_H
#define SEARCHENGINE_HASH_H

#include <stdio.h>
#include "stack.h"


typedef unsigned int Position;
typedef char *ElementType;

//用于标记一个节点的状态
typedef enum Info {
    Legitimate, Empty, Deleted
} Info;

//词项位置链表
typedef struct TermPosition {
    int p;
    struct TermPosition *next;
} *TermPosition;
typedef TermPosition TermPositions;

//倒排记录表
typedef struct InvertedTable {
    int docID;  //文档ID
    int frequency;  //在该文档中出现的频率
    TermPositions positions;  //词项位置链表
    struct InvertedTable *next;  //指向下一个节点
} *InvertedTable;

//哈希表的节点，一个词项对应的倒排索引
struct TblNode {
    ElementType wordTerm;   //词项
    int docFrequency;       //文档频率
    InvertedTable table;    //一个词项的倒排记录表
    Info info;              //状态
};
typedef struct TblNode *Cell;

//哈希表
typedef struct HashTable {
    int TableSize;             //表的大小
    Cell Cells;              //结点数组
} *HashTable;

/*
 * 建立倒排索引
 *
 * return: 建立好的倒排索引（哈希表）
 * */
HashTable BuildIndex();

HashTable InitHashTable(int TableSize);                         //初始化一个哈希表并返回
void ShowTable(HashTable HT);                                   //输出哈希表
Position StrHash(char *str, int TableSize);                     //映射字符串的哈希函数
Position Find(HashTable HT, ElementType E);                     //查找元素E在HT中的位置，查到则返回对应的位置，否则返回应该插入的位置

/*
 * 将词项插入到哈希表中或者增大对应的记录
 *
 * HT: 被插入的哈希表
 * E: 插入的词项
 * docID: 文档ID
 * */
void InsertOrCount(HashTable HT, ElementType E, int docID, int termPosition);

int ReadAWord(FILE *fp, ElementType *E);                        //从文件中读取一个单词至E，返回单词的长度
void RankHashTbl(HashTable HT);                                 //输出哈希表中词频前一百的单词



#endif //SEARCHENGINE_HASH_H
