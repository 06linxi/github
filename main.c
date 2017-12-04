#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <stdbool.h>

#define m 4


//书的信息的结构体
typedef struct {
    int booknum;
    char bookname[20];
    char writer[20];
    int now; //书的现存量
    int total; //书的总库存
    int flag; //书是否借出的标志
} Book;

typedef Book KeyType;


/**  B-Tree模块 **/

//B树结构体
typedef struct BTNode {
    int keynum; //结点中关键字的个数
    struct BTNode *parent; //结点的双亲结点
    KeyType key[m + 1]; //结点中的关键字
    struct BTNode *ptr[m + 1]; //孩子结点数组
} BTNode, *BTree;

typedef struct {
    BTNode *pt; // 指向找到结点的指针
    int i; //在结点中的关键字序号
    int tag; // 用以记录结果的结构体
} Result;


//设结点中的指针向量为空
void setNull(BTree p) {
    int i = 0;
    while (i <= p->keynum) {
        p->ptr[i] = NULL;
        i++;
    }
}

//在结点中查找关键字
int searchNode(BTree p, KeyType *k) {
    int i = 1;
    while (i <= p->keynum) {
        if (k->booknum < p->key[i].booknum)
            return i - 1;
        i++;
    }
    return i - 1;
}


//在B-Tree中查找关键字
Result searchBTree(BTree t, KeyType *k) {
    BTree p = t, q = NULL;
    int found = 0;
    int i = 0;
    Result result;
    while (p && !found) {
        i = searchNode(p, k);
        if (i > 0 && p->key[i].booknum == k->booknum)
            found = 1;
        else {
            q = p;
            p = p->ptr[i];
        }
    }
    if (found) {
        result.pt = p;
        result.i = i;
        result.tag = 1;
    } else {
        result.pt = q;
        result.i = i;
        result.tag = 0;
    }
    return result;
}


//赋值函数
void cpy(KeyType *p, KeyType *q) {
    p->total = q->total;
    p->now = q->now;
    strcpy(p->writer, q->writer);
    p->booknum = q->booknum;
    strcpy(p->bookname, q->bookname);
    p->flag = q->flag;
}

//在结点中插入新的关键字k和指针pt
void InsertInNode(BTree q, int i, KeyType *k, BTree pt) {
    int j;
    for (j = q->keynum; j > i; j--)
        cpy(&(q->key[j + 1]), &(q->key[j]));
    cpy(&(q->key[j + 1]), k);
    for (j = q->keynum; j > i; j--)
        q->ptr[j + 1] = q->ptr[j];
    q->ptr[j + 1] = pt;
    if (pt) {
        pt->parent = q;
    }
    q->keynum++;
}

//将x和ap分别插入到q->key[i+1]和p->ptr[i+1]中
void Insert(BTNode *q, int i, KeyType x, BTNode *ap) {
    int j;
    //空出一个位置
    for (j = q->keynum; j > i; j--) {
        q->key[j + 1] = q->key[j];
        q->ptr[j + 1] = q->ptr[j];
    }
    q->key[i + 1] = x;
    q->ptr[i + 1] = ap;
    if (ap != NULL)
        ap->parent = q;
    q->keynum++;
}


//分裂结点p
void split(BTree p, BTree q) {
    int s = m % 2 == 0 ? m / 2 - 1 : m / 2, i, j = 0, t;
    p->keynum = s;
    q = (BTree) malloc(sizeof(BTNode));
    setNull(q);
    for (i = s + 2; i < m; i++) {
        q->ptr[j] = p->ptr[i - 1];
        cpy(&(q->key[++j]), &(p->key[i]));
    }
    q->ptr[j] = p->ptr[i - 1];
    q->keynum = j;
    for (t = s + 1; t <= m; t++) {
        if (p->ptr[t] != NULL)
            p->ptr[t]->parent = q;
    }
}


//生成含信息(T,x,xap)的新的根结点*t,原t和ap为子树指针
void newRoot(BTNode *t, BTNode *p, KeyType x, BTNode *ap) {
    t = (BTNode *) malloc(sizeof(BTNode));
    t->keynum = 1;
    t->ptr[0] = p;
    t->ptr[1] = ap;
    t->key[1] = x;
    if (p != NULL)
        p->parent = t;
    if (ap != NULL)
        ap->parent = t;
    t->parent = NULL;
}


void insertBTree(BTNode *t, KeyType k, BTNode *q, int i) {
    BTNode *ap;
    int finished, needNewRoot, s;
    KeyType x;
    if (q == NULL)
        newRoot(t, NULL, k, NULL);
    else {
        x = k;
        ap = NULL;
        finished = needNewRoot = 0;
        while (needNewRoot == 0 && finished == 0) {
            //将x和ap分别插入到q->key[i+1]和q->ptr[i+1]
            Insert(q, i, x, ap);
            if (q->keynum <= m)
                finished = 1;
            else {
                s = (m + 1) / 2;
                split(q, ap);
                x = q->key[s];
                if (q->parent) {
                    q = q->parent;
                } else
                    needNewRoot = 1;
            }
        }
        if (needNewRoot == 1)
            newRoot(t, q, x, ap);
    }
}

//查找被删除关键字p->key[i]（在非叶子结点中）的替代叶子结点
void successor(BTNode *p, int i) {
    BTNode *q;
    for (q = p->ptr[i]; q->ptr[0] != NULL; q = q->ptr[0]);
    p->key[i] = q->key[1]; // 赋值关键字
}

//查找被删除关键字p->key[i]
void Remove(BTNode *p, int i) {
    BTNode *q;
    for (q = p->ptr[i]; q->ptr[0] != NULL; q = q->ptr[0]);
}

//把一个关键字移动到右兄弟中
void moveRight(BTNode *p, int i) {
    int c;
    BTNode *t = p->ptr[i];
    //将右兄弟中所有关键字移动一位
    for (c = t->keynum; c > 0; c--) {
        t->key[c + 1] = t->key[c];
        t->ptr[c + 1] = t->ptr[c];
    }
    //从双亲结点移动关键字到右兄弟中
    t->ptr[1] = t->ptr[0];
    t->keynum++;
    t->key[1] = p->key[i];
    t = p->ptr[i - 1]; //将左兄弟中最后一个关键字移动到双亲结点中
    p->key[i] = t->key[t->keynum];
    p->ptr[i]->ptr[0] = t->ptr[t->keynum];
    t->keynum--;
}


//把一个关键字移动到左兄弟
void moveLeft(BTNode *p, int i) {
    int c;
    BTNode *t;
    t = p->ptr[i - 1]; //把双亲结点中的关键字移动到左兄弟中
    t->keynum++;
    t->key[t->keynum] = p->key[i];
    t->ptr[t->keynum] = p->ptr[i]->ptr[0];
    t = p->ptr[i]; // 把右兄弟中所有关键字移动一位
    p->key[i] = t->key[1];
    p->ptr[0] = t->ptr[1];
    t->keynum--;
    //将右兄弟中所有关键字移动一位
    for (c = 1; c <= t->keynum; c++) {
        t->key[c] = t->key[c + 1];
        t->ptr[c] = t->ptr[c + 1];
    }
}


void combine(BTNode *p, int i) {
    int c;
    BTNode *q = p->ptr[i]; //指向右结点，它被置空和删除
    BTNode *l = p->ptr[i - 1];
    l->keynum++;
    l->key[l->keynum] = p->key[i];
    l->ptr[l->keynum] = q->ptr[0];
    //插入右结点中的所有关键字
    for (c = 1; c <= q->keynum; c++) {
        l->keynum++;
        l->key[l->keynum] = q->key[c];
        l->ptr[l->keynum] = q->ptr[c];
    }

    //删除父节点所有关键字
    for (c = i; c < p->keynum; c++) {
        p->key[c] = p->key[c + 1];
        p->ptr[c] = p->ptr[c + 1];
    }
    p->keynum--;
    free(q); //释放空右结点的空间
}

//关键字被删除后，调整B-树，找到一个关键字将其插入到p->ptr[i]中
void restore(BTNode *p, int i) {
    //为最左边关键字的情况
    if (i == 0) if (p->ptr[1]->keynum > 0)
        moveLeft(p, 1);
    else
        combine(p, 1);
    else if (i == p->keynum) if (p->ptr[i - 1]->keynum > 0)
        moveRight(p, i);
    else
        combine(p, i);
    else if (p->ptr[i - 1]->keynum > 0)//为其他情况
        moveRight(p, i);
    else if (p->ptr[i + 1]->keynum > 0)
        moveLeft(p, i + 1);
    else
        combine(p, i);
}


//在结点p中找关键字为K的位置i，成功时返回1，否则返回0
int SearchNode(KeyType k, BTNode *p, int i) {
    //k小于*p结点的最小关键字时返回0
    if (k.booknum < p->key[1].booknum) {
        i = 1;
        return 0;
    } else {
        //在*p结点中查找
        i = p->keynum;
        while (k.booknum < p->key[i].booknum && i > 1)
            i--;
        return (k.booknum == p->key[i].booknum);
    }
}

//查找并删除关键字k
int recDelete(KeyType k, BTNode *p) {
    int i;
    int found;
    if (p == NULL)
        return 0;
    else {
        //查找关键字k
        if ((found = SearchNode(k, p, i)) == 1) {
            //若为非叶子结点
            if (p->ptr[i - 1] != NULL) {
                successor(p, i); // 由其后继代替它
                recDelete(p->key[i], p->ptr[i]); //p->key[i]在叶子结点中
            } else
                Remove(p, i); //从*p结点中位置i处删除关键字
        } else
            found = recDelete(k, p->ptr[i]); //沿孩子结点递归查找并删除关键k

        if (p->ptr[i] != NULL) if (p->ptr[i]->keynum < 0)  //删除后关键字个数小于MIN
            restore(p, i);
        return found;
    }
}


//从B-树root中删除关键字k，若在一个结点中删除指定的关键字，不再有其他关键字，则删除该结点
void DeleteBTree(KeyType k, BTNode *root) {
    BTNode *p; //用于释放一个空的root
    if (recDelete(k, root) == 0)
        printf("关键字不在B-树中\n");
    else if (root->keynum == 0) {
        p = root;
        root = root->ptr[0];
        free(p);
    }
}

//在q结点第i个位置插入关键字
void InsertBTree(BTree t, KeyType *k, BTree q, int i) {
    BTree ap = NULL, root;
    bool fininsh = 0;
    int s;
    KeyType *x;
    x = (KeyType *) malloc(sizeof(KeyType));
    cpy(x, k);
    while (q && !fininsh) {
        InsertInNode(q, i, x, ap);
        if (q->keynum < m)
            fininsh = 1;
        else {
            s = m % 2 == 0 ? m / 2 : m / 2 + 1;
            split(q, ap);
            cpy(x, &(q->key[s]));
            q = q->parent;
            if (q)
                i = searchNode(q, x);
        }
    }
    if (!fininsh) {
        root = (BTree) malloc(sizeof(BTNode));
        setNull(root);
        cpy(&(root->key[1]), x);
        root->ptr[0] = t;
        root->ptr[1] = ap;
        root->keynum = 1;
        root->parent = NULL;
        if (t)
            t->parent = root;
        if (ap)
            ap->parent = root;
        t = root;
    }
}


//在B树中插入一个关键字
void InsertNode(BTree t, KeyType *key) {
    Result result;
    result = searchBTree(t, key);
    if (!result.tag) {
        InsertBTree(t, key, result.pt, result.i);
    }
}


//////图书操作函数
//图书采编入库
void Inputku(BTree *r) {
    Book book;
    Result result;
    printf("按照书号  书名  现存量  库存量 作者的顺序输入书的信息，当输入书号<0时结束：\n");
    while (1) {
        /*printf("请输入书号:");*/
        scanf("%d", &book.booknum);
        if (book.booknum < 0)
            break;
        scanf("%s", &book.bookname[20]);
        scanf("%d", &book.now);
        scanf("%d", &book.total);
        scanf("%s", &book.writer[20]);
        book.flag = book.now; //表示该书没有被借出
        result = searchBTree(*r, &book);
        if (result.tag == 0)
            InsertNode(*r, &book);
        else {
            result.pt->key[result.i].now += book.now;
            result.pt->key[result.i].flag += book.flag;
        }
    }
    printf("图书采编入库成功!\n");
}

//借阅图书
void lend(BTree *r) {
    KeyType b;
    int j;
    Result result;
    printf("输入所借书的书号和借书的本数：");
    scanf("%d", &b.booknum);
    scanf("%d", &j);
    result = searchBTree(*r, &b);
    if (b.booknum < 0) {
        printf("输入错误！\n");
        return;
    }

    if (result.tag == 0) {
        printf("抱歉，没有此书!\n");
    } else {
        if (result.pt->key[result.i].now >= j) {
            result.pt->key[result.i].now -= j;
            printf("借书成功！\n您借的书的信息：\n书号:%d 借出:%d本\n请确认信息！\n", b.booknum, j);

        } else
            printf("该书现存不够！\n");
        if (result.pt->key[result.i].flag == 0)
            printf("此书已清除库存，请采编入库!\n");

    }
}

//归还图书函数
void returnBook(BTree *r) {
    Book book;
    int k;
    Result result;
    printf("请输入归还的书的书号和归还的本数：");
    scanf("%d", &book.booknum);
    scanf("%d", &k);
    if (book.booknum < 0) {
        printf("输入错误！");
        return;
    }

    result = searchBTree(*r, &book);
    if (result.tag == 0)
        printf("没有此图书！\n");
    else {
        if (k <= result.pt->key[result.i].total - result.pt->key[result.i].now) {
            result.pt->key[result.i].now = result.pt->key[result.i].now + k;
            printf("还书成功！\n\n");
        } else if (k > result.pt->key[result.i].total - result.pt->key[result.i].now) {
            printf("\n还书大于总存量，输入错误！请重试！\n");
        }
    }
}


//凹入法显示图书
void output(BTree r, int n) {
    int i = 1, j;
    if (r == NULL)
        return;
    while (i <= r->keynum) {
        output(r->ptr[i - 1], n + 1);
        for (j = 0; j < n; j++)
            printf("  ");
        if (n > 0) {
            printf("----");
            printf("书号：%d,书名:%s,作者:%s,现存：%d,总库存：%d", r->key[i].booknum, r->key[i].bookname, r->key[i].writer,
                   r->key[i].now, r->key[i].total);
            if (r->key[i].flag == 0)
                printf("------>此书已清除库存!\n");
            else
                printf("\n");
        }
        i++;
    }
    output(r->ptr[i - 1], n + 1);
}


//清除库存函数
void clearKuCun(BTree *r) {
    KeyType b;
    Result result;
    printf("请输入你要清楚库存的书的信息书号:");
    scanf("%d", &b.booknum);
    result = searchBTree(*r, &b);
    if (b.booknum < 0) {
        printf("输入错误！");
        return;
    }

    if (result.tag == 0)
        printf("没有此图书！\n");
    else {
        result.pt->key[result.i].total = 0;
        result.pt->key[result.i].now = 0;
        result.pt->key[result.i].flag = 0;
        printf("清除成功！\n");
    }
}


////////////////主函数
int main() {
    BTree root = NULL;
    int t, n;

    L1:
    printf("**************************************\n");
    printf("**********欢迎使用本系统*****************\n");
    printf("**************************************\n\n");
    printf("|        1、图书管理员登陆              |\n\n");
    printf("|        2、普通借阅者登陆              |\n\n");
    printf("|        3、退出                      |\n\n");
    printf("*********请选择登陆状态*****************\n");
    printf("\n");
    printf("请输入编号：");
    scanf("%d", &n);
    if (n == 1) {
        while (1) {
            printf("**************欢迎使用图书管理系统*******\n");
            printf("**************管理员使用状态************\n");
            printf("|                                    |\n");
            printf("|  1、图书入库                         |\n");
            printf("|                                    |\n");
            printf("|  2、清除库存                         |\n");
            printf("|                                    |\n");
            printf("|  3、借阅图书                         |\n");
            printf("|                                    |\n");
            printf("|  4、归还图书                         |\n");
            printf("|                                    |\n");
            printf("|  5、查询图书信息                      |\n");
            printf("|                                    |\n");
            printf("|  0、返回                            |\n");
            printf("**************请选择序号************\n");
            printf("请选择你要进行的操作：");
            scanf("%d", &t);
            if (t == 0) {
                system("CLS");
                goto L1;
            }
            switch (t) {
                case 0:
                    goto L1;
                case 1:
                    Inputku(&root);
                    break;
                case 2:
                    clearKuCun(&root);
                    break;
                case 3:
                    lend(&root);
                    break;
                case 4:
                    returnBook(&root);
                    break;
                case 5:
                    output(root, 0);
                    break;
                default:
                    printf("输入错误！");
                    goto L3;
            }
        }
    } else if (n == 2) {
        while (1) {
            printf("*********欢迎使用图书管理系统********\n");
            printf("*******用户使用状态********\n");
            printf("|                                    |\n");
            printf("|  1、显示所有图书                            |\n");
            printf("|                                    |\n");
            printf("|  0、退出                           |\n");
            printf("|                                    |\n");
            printf("***************请选择序号***************\n");
            printf("\n");
            L3:
            printf("请选择你要进行的操作：");
            scanf("%d", &t);
            switch (t) {
                case 0:
                    goto L1;
                    break;
                case 1:
                    output(root, 0);
                    break;
                default:
                    printf("输入错误！");
                    goto L3;
                    break;
            }
        }
    } else if (n == 3)
        printf("               成功退出！            \n");
    else {
        printf("输入错误，请重新输入！\n");
        goto L1;
    }
}
