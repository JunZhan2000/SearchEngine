//
// Created by 23276 on 2019/12/9.
//

#ifndef SEARCHENGINE_BOOL_SEARCH_H
#define SEARCHENGINE_BOOL_SEARCH_H

#include "hash.h"

//表达式对象
typedef struct ExpItem{
    int type;  //为链表头部时为0，为运算值时为1，为运算符时为2
    InvertedTable table;
    char operator;
    struct ExpItem *next;
} *ExpItem;
typedef struct ExpItem *Exp;

/*
 * 执行一次搜索
 * HT: 倒排索引的指针
 * */
void BoolSearch();

/*
 * 两个倒排表取交集，即与操作
 * t1, t2: 两个倒排表
 *
 * return: 两表的交集
 * */
InvertedTable Intersect(InvertedTable t1, InvertedTable t2);

/*
 * 两个倒排表取并集合，即或操作
 * t1, t2: 两个倒排表
 *
 * return: 两表的并集
 * */
InvertedTable Union(InvertedTable t1, InvertedTable t2);

/*
 * 取倒排表的补集，即非操作
 * t: 倒排表
 * fileIDs: 所有文档ID的集合，即全集
 *
 * return: t表的补集
 * */
InvertedTable Complement(InvertedTable t);

/*
 * 解析中缀表达式并计算结果
 * midExp: 中缀表达式
 *
 * return: 计算得出的倒排索引表
 * */
InvertedTable ComPuteMidExp(char *midExp);

/*
 * 计算只有&和|的简单表达式
 * */
InvertedTable ComPuteSimpleExp(Exp exp);

/*
 * 获取一个不定长词组对应的的倒排表
 * */
InvertedTable ComputeCiZuExp(char *exp);

/*
 * 近邻搜索
 * t1, t2: 两个词项的倒排记录表
 *
 * return: 合并后的倒排记录表
 * */
InvertedTable PositionalIntersect(InvertedTable t1, InvertedTable t2);

#endif //SEARCHENGINE_BOOL_SEARCH_H