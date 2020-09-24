//----------------------------------------------------------
//Note:
//1.屬性id 0->age, 1->income, 2->student, 3->credit_rating
//2.產生子節點演算法概述
//  a.產生root
//  b.將節點及表格帶入函式
//  c.若皆為相同class則return
//  d.若只剩1行, 則取多數當class, return
//  e.分別計算大表格的info及各屬性的infoDa, 找出最大的gain, 並標注節點為由此屬性分支
//  f.切割最多3個表格, 及各別產生子節點, 回到b. 
//----------------------------------------------------------
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<math.h>

#define maxbranch 3 //樹的分支度

typedef struct node_t node;
struct node_t{
    char* data;
    node* left;
    node* middle;
    node* right;
};
void init_node(node* n);
void get_two_dimension(int** data, char *filename);
int get_row(char *filename);
int get_col(char *filename);
double get_attr_info(int attr, int **data, int row, int col);
void generate_child(node *root, int **data, int row, int col);
void preorder(node *root);
void postorder(node *root);
void levelorder(node *root);

void init_node(node* n){ //初始化新節點
    n->left = NULL;
    n->middle = NULL;
    n->right = NULL;
}
void get_two_dimension(int** data, char *filename)
{   
    FILE* stream = fopen(filename, "r");
    char line[1024];
    int i = 0;
    //fgets(line, 1024, stream);
    while (fgets(line, 1024, stream)){//逐行讀取
        int j = 0;
        char *tok = strtok(line, ","); 
        char* tmp = strdup(line);
        int count;
        tok = strtok(NULL, ","); 
        for (;tok && *tok; count++, j++, tok = strtok(NULL, ",\n")){
            data[i][j] = atoi(tok);//轉換成浮點數 
        }//字符串拆分操作 
        i++;
        free(tmp);
    }   
    fclose(stream);
}   
int get_row(char *filename){//return number of row
    char line[1024];
    int i = 0;
    FILE *stream = fopen(filename, "r");
    //fgets(line, 1024, stream);
    while(fgets(line, 1024, stream)){
        i++;
    }
    fclose(stream);
    return i;
}
int get_col(char* filename){
    char line[1024];
    int i = 0;
    FILE *stream = fopen(filename, "r");
    fgets(line, 1024, stream);
    char* token = strtok(line, ",");
    token = strtok(NULL, ",");
    while(token){
        token = strtok(NULL, ",");
        i++;
    }
    fclose(stream);
    return i;
}
double get_attr_info(int attr, int **data, int row, int col){//計算attribute of info
    int kindCount = 0; //紀錄此屬性有幾種分類
    int dataKind[maxbranch] = {0}; //紀錄此屬性的分類
    for(int i = 0; i < row; i++){
        int same = 0;
        for(int j = 0; j < kindCount; j++){
            if(data[i][attr] == dataKind[j])//紀錄遇到且不重複的屬性
                same = 1;
        }
        if(same == 0){
            dataKind[kindCount] = data[i][attr];
            kindCount++;
        }
    }
    int kindIndividualCount[maxbranch] = {0}; //個別種類的獨立計數器
    int kindToClass[maxbranch] = {0}; //個別種類對應到yes class 的數量
    for(int i = 0; i < row; i++){
        switch (data[i][attr]){
        case 0:
            kindIndividualCount[0]++; 
            if(data[i][col-1] == 1)
                kindToClass[0]++;
            break;
        case 1:
            kindIndividualCount[1]++;
            if(data[i][col-1] == 1)
                kindToClass[1]++;
            break;
        case 2:
            kindIndividualCount[2]++;
            if(data[i][col-1] == 1)
                kindToClass[2]++;
            break;
        default:
            printf("Error");
        }
    }
    double infoDa = 0;
    for(int i = 0; i < kindCount; i++){//計算各entropy並相加為infoDa
        double pa, pyes, pno;
        pa = (double)kindIndividualCount[i] / row;
        pyes = (double)kindToClass[i] / kindIndividualCount[i];
        pno = 1 - pyes;
        if(pyes == 0)
            infoDa += (pa * (pno * log2(pno)));
        else if(pno == 0)
            infoDa += (pa * (pyes * log2(pyes)));
        else
            infoDa += (pa * (pyes * log2(pyes) + pno * log2(pno)));
    }
    infoDa = 0 - infoDa;
    return infoDa;
}
void generate_child(node* root, int **tableContainRID, int row, int col){ //遞迴產生子節點
    int changed = 0;
    int class = 0;
    int **data = (int**)&tableContainRID[1]; //宣告一個新指標指向第二列的開頭, 因為第一列為紀錄屬性id
    row = row - 1; //因第一列不計
    for(int i = 0; i < row-1; i++){ //若此表格的class皆相同, 標記為葉子
        class = data[i][col-1];
        if(class != data[i+1][col-1]){
           changed = 1;
           break;
        }
    }
    if (changed == 0){
        root->data = class ? "yes" : "no";
        return;
    }
    if(col == 1){ //若此表格只剩最後一行, 則標記為葉子且資料為多數class
        int numOfYes = 0, numOfNo = 0;
        for(int i = 0; i < row; i++){
            if(data[row][0] == 1) 
                numOfYes++;
            else 
                numOfNo++;
        } 
        root->data = numOfYes >= numOfNo ? "yes" : "no";
        return;
    }

    double biggestGain = 0, infoD, pyes, pno;//計算大表格的info
    int select = 0, numOfCy = 0;
    for(int i = 0; i < row; i++){
        if(data[i][col-1] == 1) 
            numOfCy++;
    }
    pyes = (double)numOfCy/row;
    pno = 1 - pyes;
    infoD = 0- (pyes * log2(pyes) + pno * log2(pno));
    //printf("pyes: %f pno: %f\n", pyes, pno);
    //printf("infoD:%f\n", infoD);
    for(int i = 0; i < col-1; i++){
        double infoDa = get_attr_info(i, data, row, col);//分別計算各屬性的infoDa
        if(biggestGain <= infoD - infoDa){
            select = i;
            biggestGain = infoD - infoDa;
        }
        printf("gain:%f\n", infoD - infoDa);
    }
    printf("select:%d\n", select);
    switch(tableContainRID[0][select]){//得出最大的gain後, 得將此節點標記為由此屬性來分支
        case 0:
            root->data = "branch by age";
            break;
        case 1:
            root->data = "branch by income";
            break;
        case 2:
            root->data = "branch by student";
            break;
        case 3:
            root->data = "branch by credit_rating";
        default:
            break;
    }
    int **kindIndex = (int**)malloc(maxbranch * sizeof(int*));//分別紀錄屬性中各類在表格中的指標
    for(int i = 0; i < maxbranch; i++){
        kindIndex[i] = (int*)malloc(row * sizeof(int));
    }
    int indexCount[maxbranch] = {0};//計算此屬性各種類的數量
    for(int i = 0; i < row; i++){
        switch(data[i][select]){
            case 0:
                kindIndex[0][indexCount[0]] = i;
                indexCount[0]++;
            break;
            case 1:
                kindIndex[1][indexCount[1]] = i;
                indexCount[1]++;
            break;
            case 2:
                kindIndex[2][indexCount[2]] = i;
                indexCount[2]++;
            break;
            default:
                printf("Error\n");
        }    
    }
    int ***newTable;//建立三維陣列分別存取切割後的表格
    if(indexCount[2] == 0){//若此屬性的種類只有兩種, 只要建立兩個表格
        newTable = (int***)malloc(2 * sizeof(int**));
        for(int i = 0; i < 2; i++){
            newTable[i] = (int**)malloc((indexCount[i]+1) * sizeof(int*));
            for(int j = 0; j < (indexCount[i]+1); j++){
                newTable[i][j] = (int*)malloc((col - 1) * sizeof(int));
            }
        }
    }
    else{
        newTable = (int***)malloc(3 * sizeof(int**));
        for(int i = 0; i < 3; i++){
            newTable[i] = (int**)malloc((indexCount[i]+1) * sizeof(int*));
            for(int j = 0; j < (indexCount[i]+1); j++){
                newTable[i][j] = (int*)malloc((col - 1) * sizeof(int));
            }
        }
    }
    int table = 0, tablei = 0, tablej = 0;//切割表格
    int indexi = 0, indexj = 0;
    for(table = 0; table < 2; table++){
        for(int j = 0, indexj = 0; indexj < col; indexj++, j++){
            if(select == indexj)
                indexj++;
            newTable[table][0][j] = tableContainRID[0][indexj];
        }
        for(tablei = 1, indexi = 0; tablei < indexCount[table] + 1 && indexi < indexCount[table]; indexi++, tablei++){
            for(tablej = 0, indexj = 0; tablej < col - 1 && indexj < col; indexj++, tablej++){
                if(indexj == select)
                    indexj++;
                newTable[table][tablei][tablej] = data[kindIndex[table][indexi]][indexj];
            }
        }
    }
    for(table = 0; table < 2; table++){//output前兩個表格
        for(tablei = 0 ; tablei < indexCount[table]+1; tablei++){
            for(tablej = 0; tablej < col - 1 ; tablej++){
                printf("%d, ", newTable[table][tablei][tablej]);
            }
            printf("\n");
        }
        printf("\n");
    }
    if(indexCount[2] != 0){//若有第三種類則繼續切割
        for(int j = 0, indexj = 0; indexj < col; indexj++, j++){
            if(select == indexj)
                indexj++;
            newTable[2][0][j] = tableContainRID[0][indexj];
        }
        for(tablei = 1, indexi = 0; tablei < indexCount[table] + 1 && indexi < indexCount[table]; indexi++, tablei++){
            for(tablej = 0, indexj = 0; tablej < col - 1 && indexj < col; indexj++, tablej++){
                if(indexj == select)
                    indexj++;
                newTable[table][tablei][tablej] = data[kindIndex[table][indexi]][indexj];
            }
        }
        for(tablei = 0 ; tablei < indexCount[table]+1; tablei++){
            for(tablej = 0; tablej < col - 1 ; tablej++){
                printf("%d, ", newTable[table][tablei][tablej]);
            }
            printf("\n");
        }
        printf("\n");
    } 
    node* child1 = (node*)malloc(sizeof(node));//將切割完的表格及新節點帶入此函式
    init_node(child1);
    root->left = child1;
    generate_child(child1, newTable[0], indexCount[0] + 1, col - 1);//因為要紀錄屬性id, indexCount加1
    node* child2 = (node*)malloc(sizeof(node));
    init_node(child2);
    root->middle = child2;
    generate_child(child2, newTable[1], indexCount[1] + 1, col - 1);
    if(indexCount[2] != 0){
        node* child3 = (node*)malloc(sizeof(node));
        init_node(child3);
        root->right = child3;
        generate_child(child3, newTable[2], indexCount[2] + 1, col - 1);
    }
}
void preorder(node* root){
    node* p = root;
    if(p != NULL){
        printf("%s->", p->data);
        preorder(p->left);
        preorder(p->middle);
        preorder(p->right);
    }
}
void postorder(node* root){
    node* p = root;
    if(p != NULL){
        postorder(p->left);
        postorder(p->middle);
        postorder(p->right);
        printf("%s->", p->data);
    }
}
void levelorder(node* root){
    int front = 0, rear = 0;
    node **queue = (node**)malloc(8 * sizeof(node*));
    queue[++rear] = root;

    while(front != rear){
        node *p = queue[++front];
        printf("%s->", p->data);
        if(p->left)
            queue[++rear] = p->left;
        if(p->middle)
            queue[++rear] = p->middle;
        if(p->right)
            queue[++rear] = p->right;
    }
}
int main(){
    char filename[] = "data.csv";
    int row, col;
    int **data;
    row = get_row(filename);
    col = get_col(filename);
    data = (int **)malloc(row * sizeof(int *));
    for(int i = 0; i < row; i++)
        data[i] = (int *)malloc(col * sizeof(int));
    get_two_dimension(data, filename);
    printf("原資料:\n");
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            printf("%d, ", data[i][j]);
        }
        printf("\n");
    }

    node* root = (node*)malloc(sizeof(node));
    init_node(root); 
    generate_child(root, data, row, col);
    printf("print by preorder:\n");
    preorder(root);
    printf("\n");
    printf("print by postorder:\n");
    postorder(root);
    printf("\n");
    printf("print by levelorder:\n");
    levelorder(root);
    printf("\n");
}