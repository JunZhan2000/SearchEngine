//
// Created by 23276 on 2019/12/12.
//

#include "heapSort.h"
#include <stdio.h>
#include <stdlib.h>

extern int fileIDs[10];

//交换值
void swap(SortElementType *a, SortElementType *b) {
    float temScore = b->score;
    int temID = b->docID;

    b->score = a->score;
    b->docID = a->docID;
    a->score = temScore;
    a->docID = temID;
}

//求左儿子的位置
int leftChild(int p) {
    return (p * 2 + 1);
}

//下滤
void PercDown(SortElementType nodes[], int start, int end) {
    SortElementType temp = nodes[start];  //记住父节点
    int child;

    for (; leftChild(start) <= end; start = child) {
        child = leftChild(start);
        if (child + 1 != end + 1 && nodes[child + 1].score > nodes[child].score) {
            //如果右儿子存在且右儿子比左儿子更大
            child++;
        }
        if (nodes[child].score > temp.score) {
            //更大的那个儿子比原顶点大，大儿子上位
            nodes[start].score = nodes[child].score;
            nodes[start].docID = nodes[child].docID;
        } else {
            //父节点比两个儿子都大，则找到其位置，不用再下滤
            break;
        }
    }
    //原来的顶点下滤到合适的位置
    nodes[start].score = temp.score;
    nodes[start].docID = temp.docID;
}


//堆排序
void HeapSort(float nums[], int len, int k) {
    int i;

    SortElementType* nodes = (SortElementType*) malloc(len * sizeof(struct DocScore));
    for(i = 0; i < len; i++){
        nodes[i].docID = fileIDs[i];
        nodes[i].score = nums[i];
    }
    // 建立最大堆
    for (i = len / 2 - 1; i >= 0; i--)
        PercDown(nodes, i, len - 1);
    // 先将第一个元素和已排好元素前一位做交换，再重新调整，直到排序完毕
    printf("Top K:\n");
    for (i = len - 1; i > len - k - 1; i--) {
        printf("docId: %d  scores: %f\n", nodes[0].docID, nodes[0].score);
        swap(nodes, nodes+i);
        PercDown(nodes, 0, i - 1);
    }
}