/*
 * 测试用例：
 * China: 1 2 6 8
 * public: 2 5
 * country: 0 2 4 5 6 8
 * you: 0 1 2 3 4 8
 *
 * China | public & country: 2 5 6 8
 * China & (public | (country | you)): 1 2 6 8
 * China & ! (public | ! (country | you))
 *
 * "father she had been raped": 5
 * "Accelerating deforestation and rampant": 3
 * China & (public | "father she had been raped") | "Accelerating deforestation and rampant": 3
 * */


#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "utils.h"
#include "free_text_search.h"
#include "bool_search.h"
#include "heapSort.h"

//affection jealous jealous

int fileIDs[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
int IDLength = 10;
HashTable HT;

int main(void){

    HT = BuildIndex();  //读取文件，获得倒排索引

    while (1){  //提供查询服务
        char command[10], ch;
        printf("Enter bool for Boolean query (support phrase query)\n"
               "for example:   !China & (public | \"father she had been raped\") | \"Accelerating deforestation and rampant\"\n\n"
               "Enter query for free text query\n"
               "Enter quit to exit\n");
        scanf("%s", command);
        ch = getchar();  //读取掉回车
        if(strcmp(command, "bool") == 0){
            BoolSearch();  //进行布尔查询
        } else if(strcmp(command, "query") == 0){
            FreeTextSearch();  //进行自由文本查询
        } else if(strcmp(command, "quit") == 0){
            break;
        }
    }

    return 0;
}



