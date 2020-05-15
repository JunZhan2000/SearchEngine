//
// Created by 23276 on 2019/12/11.
//

#include "hash.h"
#include "utils.h"
#include "heapSort.h"
#include "free_text_search.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

extern HashTable HT;
extern int IDLength;

/*
 * 进行一次自由文本查询
 * */
void FreeTextSearch() {
    char query[100];  //command用于存储命令，buffer用于临时存储词条
    int k = 3;
    while(1){
        printf("please enter the query (enter q to return):\n");
        gets(query);
        if(strcmp(query, "q") == 0){
            break;
        }
        QueryVector queryVector = GetQueryVector(query);  //计算查询向量
        OutputTopKDocs(queryVector, k);  //输出相关性最高的前五个文档ID
    }
}

/*
 * 基于向量空间模型，输出关联性最高的前k个文档ID
 * */
void OutputTopKDocs(QueryVector queryVector, int k) {
    int *docIDs = (int *) malloc(k * sizeof(int));
    float *lengths, *Scores = (float *) malloc(IDLength * sizeof(float));
    InvertedTable move;
    QueryNode queryMove = queryVector->first;
    Position p;

    for(int i = 0; i < IDLength; i++){  //初始化
        Scores[i] = 0;
    }

    lengths = ComputeVectorLength();  //获取所有文档向量的长度

    //遍历查询向量，累加得查询向量与各个文档向量的内积
    for(int i = 0; i < queryVector->length; i++){
        p = Find(HT, queryMove->item);
        float idf = log10f((float)IDLength / (float)HT->Cells[p].docFrequency);  //逆倒排文档频率，每个词项的该值只有一个
        float queryWf = (float)queryMove->frequency * idf;  //该词项在查询向量中的权重
        //遍历该词项的倒排记录表
        move = HT->Cells[p].table;
        while(move != NULL){
            float docWf = (float)move->frequency * idf;  //一个词项在一个文档中的权重
            Scores[move->docID] += queryWf * docWf;
            move = move->next;
        }
        queryMove = queryMove->next;
    }

    //每个内积除以对应的文档向量的长度
    for(int i = 0; i < IDLength; i++){  //初始化
        Scores[i] = Scores[i] / lengths[i];
    }

    for(int i = 0; i < IDLength; i++){
        printf("doc%d's scores:  %f\n", i, Scores[i]);
    }
    HeapSort(Scores, IDLength, k);  //输出分数最高的k篇文档

    free(lengths);
    free(Scores);
    free(docIDs);
}

/*
 * 计算查询文本的查询向量
 * query: 用户输入的自由文本
 *
 * return 自由文本对应的查询向量
 * */
QueryVector GetQueryVector(char *query) {
    char buffer[21], ch;     //缓冲区，最多读取长度为20的单词
    int queryIndex = 0, bufferIndex = 0;
    QueryVector queryVector = InitQueryVector();

    while (query[queryIndex] != '\0') {
        ch = query[queryIndex];
        if ((ch < 48 || (ch >= 58 && ch <= 64) || (ch >= 91 && ch <= 96 && ch != 95) || ch > 122) && bufferIndex != 0) {
            //如果读到的字符是分隔符，且已经开始读取词条
            if (bufferIndex >= 3) {
                // 词条长度大于等于3，则停止读取并将信息插入至查询向量中，否则丢弃该词条
                buffer[bufferIndex] = '\0';
                InsertOrCountToQuery(queryVector, buffer);
            }
            bufferIndex = 0;
        } else {
            //读到的字符不是分隔符，则存入缓冲区中
            buffer[bufferIndex++] = ch;
        }
        queryIndex++;
    }

    if (bufferIndex >= 3) {
        // 若字符串尾部有长度大于等于3的词条，则将其信息插入至查询向量中
        buffer[bufferIndex] = '\0';
        InsertOrCountToQuery(queryVector, buffer);
    }

    return queryVector;
}

/*
 * 初始化一个查询向量
 * */
QueryVector InitQueryVector() {
    QueryVector queryVector = (QueryVector) malloc(sizeof(struct QueryVector));
    queryVector->first = NULL;
    queryVector->length = 0;

    return queryVector;
}

/*
 * 将一个词项及其信息插入查询向量，若已存在，则增大对应词频
 * */
void InsertOrCountToQuery(QueryVector queryVector, char *item) {
    QueryNode move = queryVector->first;
    int i;

    for (i = 0; i < queryVector->length; i++) {
        if (strcmp(move->item, item) == 0) {
            //查询向量中已经有了该词项，则增大词频
            move->frequency++;
            break;
        }
        move = move->next;
    }

    if (i == queryVector->length) {  //查询向量中没有该词项，则直接插入到头部
        //新建
        QueryNode newNode = (QueryNode) malloc(sizeof(struct QueryNode));
        newNode->item = (char *) malloc((strlen(item) + 1) * sizeof(char));
        strcpy(newNode->item, item);
        newNode->frequency = 1;

        //插入
        newNode->next = queryVector->first;
        queryVector->first = newNode;
        queryVector->length++;
    }
}



/*
 * 计算所有文档向量的长度
 * HT: 倒排索引
 *
 * return: 所有文档向量的长度，数组
 * */
float *ComputeVectorLength() {
    float *lengths = (float *) malloc(IDLength * sizeof(int));
    InvertedTable move;

    for(int i = 0; i < IDLength; i++){  //初始化
        lengths[i] = 0;
    }

    //遍历词典，累加得文档向量的长度
    for(int i = 0; i < HT->TableSize; i++){
        if(HT->Cells[i].info == Legitimate){  //若该位置处有词项
            float idf = log10f((float)IDLength / (float)HT->Cells[i].docFrequency);  //逆倒排文档频率
            move = HT->Cells[i].table;
            while(move != NULL){
                float wf = (float)move->frequency * idf;  //一个词项在一个文档中的权重
                lengths[move->docID] += wf * wf;
                move = move->next;
            }
        }
    }

    for(int i = 0; i < IDLength; i++){  //开方
        lengths[i] = sqrtf(lengths[i]);
    }

    return lengths;
}