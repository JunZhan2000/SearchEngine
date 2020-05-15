//
// Created by 23276 on 2019/12/12.
//

#ifndef SEARCHENGINE_HEAPSORT_H
#define SEARCHENGINE_HEAPSORT_H

typedef struct DocScore{
    float score;  //得分
    int docID;    //文档ID
} DocScore;
typedef DocScore SortElementType;

//交换值
void swap(SortElementType *a, SortElementType *b);

//求左儿子的位置
int leftChild(int p);

//下滤
void PercDown(SortElementType nodes[], int start, int end);

//堆排序
void HeapSort(float nums[], int len, int k);

#endif //SEARCHENGINE_HEAPSORT_H
