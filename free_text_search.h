//
// Created by 23276 on 2019/12/11.
//

#ifndef SEARCHENGINE_FREE_TEXT_SEARCH_H
#define SEARCHENGINE_FREE_TEXT_SEARCH_H

//查询向量的节点
typedef struct QueryNode{
    char * item;  //词项
    int frequency;  //在自由文本中出现的频率，即tf值
    //idf值直接用倒排索引里的数据
    struct QueryNode * next;
} * QueryNode;

//查询向量
typedef struct QueryVector{
    QueryNode first;
    int length;
} * QueryVector;

/*
 * 进行一次自由文本查询
 * */
void FreeTextSearch();

/*
 * 基于向量空间模型，输出关联性最高的前k个文档ID
 * */
void OutputTopKDocs(QueryVector queryVector, int k);

/*
 * 计算查询文本的查询向量
 * 这里直接用了倒排索引的结构来实现(为了节省时间)，最合理的方式应该是新建一个数据结构)
 * query: 用户输入的自由文本
 *
 * return: 查询向量
 * */
QueryVector GetQueryVector(char *query);

/*
 * 初始化一个查询向量
 * */
QueryVector InitQueryVector();

/*
 * 将一个词项及其信息插入查询向量，若已存在，则增大对应词频
 * */
void InsertOrCountToQuery(QueryVector queryVector, char *item);

/*
 * 获取一个词项在查询向量中的词频，即tf值
 * */
int getTfInQuery(QueryVector queryVector, char *item);

/*
 * 计算所有文档向量的长度
 * HT: 倒排索引
 *
 * return: 所有文档向量的长度，数组
 * */
float * ComputeVectorLength();

#endif //SEARCHENGINE_FREE_TEXT_SEARCH_H
