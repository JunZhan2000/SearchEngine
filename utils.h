//
// Created by 23276 on 2019/12/9.
//

#ifndef SEARCHENGINE_UTILS_H
#define SEARCHENGINE_UTILS_H

#include "hash.h"
#include "bool_search.h"

/*
 * 向倒排表中插入节点
 * table: 被插入的倒排表
 * newNode: 需要插入的节点
 * order: 插入的位置
 * */
void InsertNode(InvertedTable table, InvertedTable newNode, int order);

/*
 * 复制一个倒排表节点
 * newNode: 新节点指针
 * oldNode: 旧节点指针
 * */
InvertedTable CopyNode(InvertedTable oldNode);

/*
 * 在链表尾部添加新节点
 * answerPtr: 被插入的表
 * t: 被复制后插入的节点
 * */
void AddNode(InvertedTable *answerPtr, InvertedTable t);

/*
 * 在链表尾部添加新节点
 * answerPtr: 被插入的表
 * docId: 新节点的docID
 * */
void AddNodeByID(InvertedTable *answerPtr, int docID);

/*
 * 判断是否为操作符
 * ch: 被判断的字符
 * */
int IsOperator(char ch);

/*
 * 输出一个倒排记录表
 *
 * table: 被输出的表
 * */
void PrintTable(InvertedTable table);

/*
 * 在表达式表末尾插入运算对象
 * */
void InsertItem(ExpItem exp, InvertedTable table);

/*
 * 在表达式表末尾插入运算符
 * */
void InsertOperator(ExpItem exp, char operator);

/*
 * 深拷贝一个词项位置链表
 * */
TermPositions CopyTermPs(TermPositions positions);

/*
 * 在词项位置链表末尾插入p节点
 * */
void AddNodeToTermP(TermPositions *termPositions, int p);

#endif //SEARCHENGINE_UTILS_H
