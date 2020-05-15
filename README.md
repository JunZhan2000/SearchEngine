# SearchEngine

一个C语言实现的英文文本搜索引擎

An English text search engine implemented in C language



#### 文档材料

static目录下的十个txt文件，从0.txt到9.txt，内容来自BBC的英文报道。



#### 程序流程

1. **建立倒排索引**：使用哈希表存储词项及其倒排表、文档频率、词频等信息
2. 提供检索服务
   * 布尔查询
   * 词组查询：建立含位置信息的倒排索引
   * 自由文本查询：基于向量空间模型对检索结果排序



#### 建立倒排索引--哈希表

1. 依次读取文档的词条，若一个词条已存在于倒排索引中，则只更新其信息，若不存在，则插入并初始化信息

2. 重复1直至所有文档都被遍历，倒排索引建立完成

   ```C
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
           //拼接处文件路径的字符串，如"../static/0.txt"
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
                   //如果读出的字符串长度大于等于3，我们将其视为单词
                   wordCounts++;               //读取的单词数加1
                   InsertOrCount(HT, word, fileIDs[i], wordOrder++);    //若该单词第一次出现，插入表中；若已存在于表中，词频加一。wordOrder用于记录词条出现的位置
               } else {
                   //否则，弃之，释放空间
                   free(word);
               }
           }
           fclose(fp);
       }
       return HT;
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
   
   ```

**倒排索引数据结构**

* 词项位置链表

  ```C
  typedef struct TermPosition {
      int p;
      struct TermPosition *next;
  } *TermPosition;
  typedef TermPosition TermPositions;
  ```

* 倒排记录表：单链表

  ```C
  //倒排记录表
  typedef struct InvertedTable{
      int docID;  //文档ID
      int frequency;  //在该文档中出现的频率
      struct InvertedTable * next;  //指向下一个节点
  } * InvertedTable;
  ```

* 倒排索引：哈希表

  ```C
  typedef unsigned int Position;
  typedef char *ElementType;
  
  //用于标记一个节点的状态
  typedef enum Info {
      Legitimate, Empty, Deleted
  } Info;
  
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
  
  ```



#### 布尔查询

**与操作**：合并倒排表

```C
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
```

**或操作**：两倒排表取并集，类同上

**非操作**：倒排表取补集，类同上



**解析布尔查询指令并计算得结果**:

用到的几个关键函数（用栈处理括号与双引号（词组））：

```C
//数据结构：用于存储表达式，其中倒排记录表为运算对象
typedef struct ExpItem{
    int type;  //为链表头部时为0，为运算值时为1，为运算符时为2
    InvertedTable table;
    char operator;
    struct ExpItem *next;
} *ExpItem;
typedef struct ExpItem *Exp;

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

```

**测试用例**

> ```
> 左侧为输入字符串，右侧为检索出的文档ID
> 
> China: 1 2 6 8
> public: 2 5
> country: 0 2 4 5 6 8
> you: 0 1 2 3 4 8
> China | public & country: 2 5 6 8
> China & (public | (country | you)): 1 2 6 8
> ```



#### 词组查询

**原理：**在倒排索引中存储词项在文档中的位置，查询时基于位置信息合并倒排表。

```C
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
```

**测试用例**

> ```
> 左侧为输入字符串，右侧为检索出的文档ID
> 
> "father she had been raped": 5
> "Accelerating deforestation and rampant": 3
> China & (public | "father she had been raped") | "Accelerating deforestation and rampant": 3
> 
> 
> ```



#### 基于向量空间模型的自由文本查询

**原理**：

* 设**tf**为一个词项在一篇文档中的词频，
* **df**为一个词项在文档集中的文档频率，**idf**为逆文档频率，表达式为log(N / df)，其中N为所有文档的份数
* 一个词项在一篇文档中的权重**w**可表示为：tf * idf
* 一篇文档与词典中的所有词项之间的权重组成一个向量，称为**文档向量**
* 两个文档之间的**相似度**可由两者的文档向量的**夹角余弦**表示，表达式为 (v1 * v2) / (|v1| * |v2|)
* 将一个自由文本查询视作文档，即可得到其**查询向量**，则一个查询与一篇文档的相关程度可由查询向量和文档向量的相似度表示
* 计算查询向量与所有文档向量的相似度，输出相似度最高的K个文档ID（可由最大堆实现）

##### 代码实现

```C
//数据结构
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


//算法实现
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


```

**测试用例**

> ```
> 从文档7.txt中复制一段话
> 
> 输入：further action will be taken against
> 
> 输出：
> doc0's scores:  0.028482
> doc1's scores:  0.003112
> doc2's scores:  0.035071
> doc3's scores:  0.008099
> doc4's scores:  0.010380
> doc5's scores:  0.000000
> doc6's scores:  0.005722
> doc7's scores:  0.124365
> doc8's scores:  0.016283
> doc9's scores:  0.014555
> Top K:
> docId: 7  scores: 0.124365
> docId: 2  scores: 0.035071
> docId: 0  scores: 0.028482
> 
> 
> ```



#### 一些可以优化程序的方式

1. 对词条进行预处理
2. 对布尔查询操作的先后顺序进行设定（先合并小的倒排表）
3. 索引压缩