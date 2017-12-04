#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <stdbool.h>

#define m 4


//�����Ϣ�Ľṹ��
typedef struct {
    int booknum;
    char bookname[20];
    char writer[20];
    int now; //����ִ���
    int total; //����ܿ��
    int flag; //���Ƿ����ı�־
} Book;

typedef Book KeyType;


/**  B-Treeģ�� **/

//B���ṹ��
typedef struct BTNode {
    int keynum; //����йؼ��ֵĸ���
    struct BTNode *parent; //����˫�׽��
    KeyType key[m + 1]; //����еĹؼ���
    struct BTNode *ptr[m + 1]; //���ӽ������
} BTNode, *BTree;

typedef struct {
    BTNode *pt; // ָ���ҵ�����ָ��
    int i; //�ڽ���еĹؼ������
    int tag; // ���Լ�¼����Ľṹ��
} Result;


//�����е�ָ������Ϊ��
void setNull(BTree p) {
    int i = 0;
    while (i <= p->keynum) {
        p->ptr[i] = NULL;
        i++;
    }
}

//�ڽ���в��ҹؼ���
int searchNode(BTree p, KeyType *k) {
    int i = 1;
    while (i <= p->keynum) {
        if (k->booknum < p->key[i].booknum)
            return i - 1;
        i++;
    }
    return i - 1;
}


//��B-Tree�в��ҹؼ���
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


//��ֵ����
void cpy(KeyType *p, KeyType *q) {
    p->total = q->total;
    p->now = q->now;
    strcpy(p->writer, q->writer);
    p->booknum = q->booknum;
    strcpy(p->bookname, q->bookname);
    p->flag = q->flag;
}

//�ڽ���в����µĹؼ���k��ָ��pt
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

//��x��ap�ֱ���뵽q->key[i+1]��p->ptr[i+1]��
void Insert(BTNode *q, int i, KeyType x, BTNode *ap) {
    int j;
    //�ճ�һ��λ��
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


//���ѽ��p
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


//���ɺ���Ϣ(T,x,xap)���µĸ����*t,ԭt��apΪ����ָ��
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
            //��x��ap�ֱ���뵽q->key[i+1]��q->ptr[i+1]
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

//���ұ�ɾ���ؼ���p->key[i]���ڷ�Ҷ�ӽ���У������Ҷ�ӽ��
void successor(BTNode *p, int i) {
    BTNode *q;
    for (q = p->ptr[i]; q->ptr[0] != NULL; q = q->ptr[0]);
    p->key[i] = q->key[1]; // ��ֵ�ؼ���
}

//���ұ�ɾ���ؼ���p->key[i]
void Remove(BTNode *p, int i) {
    BTNode *q;
    for (q = p->ptr[i]; q->ptr[0] != NULL; q = q->ptr[0]);
}

//��һ���ؼ����ƶ������ֵ���
void moveRight(BTNode *p, int i) {
    int c;
    BTNode *t = p->ptr[i];
    //�����ֵ������йؼ����ƶ�һλ
    for (c = t->keynum; c > 0; c--) {
        t->key[c + 1] = t->key[c];
        t->ptr[c + 1] = t->ptr[c];
    }
    //��˫�׽���ƶ��ؼ��ֵ����ֵ���
    t->ptr[1] = t->ptr[0];
    t->keynum++;
    t->key[1] = p->key[i];
    t = p->ptr[i - 1]; //�����ֵ������һ���ؼ����ƶ���˫�׽����
    p->key[i] = t->key[t->keynum];
    p->ptr[i]->ptr[0] = t->ptr[t->keynum];
    t->keynum--;
}


//��һ���ؼ����ƶ������ֵ�
void moveLeft(BTNode *p, int i) {
    int c;
    BTNode *t;
    t = p->ptr[i - 1]; //��˫�׽���еĹؼ����ƶ������ֵ���
    t->keynum++;
    t->key[t->keynum] = p->key[i];
    t->ptr[t->keynum] = p->ptr[i]->ptr[0];
    t = p->ptr[i]; // �����ֵ������йؼ����ƶ�һλ
    p->key[i] = t->key[1];
    p->ptr[0] = t->ptr[1];
    t->keynum--;
    //�����ֵ������йؼ����ƶ�һλ
    for (c = 1; c <= t->keynum; c++) {
        t->key[c] = t->key[c + 1];
        t->ptr[c] = t->ptr[c + 1];
    }
}


void combine(BTNode *p, int i) {
    int c;
    BTNode *q = p->ptr[i]; //ָ���ҽ�㣬�����ÿպ�ɾ��
    BTNode *l = p->ptr[i - 1];
    l->keynum++;
    l->key[l->keynum] = p->key[i];
    l->ptr[l->keynum] = q->ptr[0];
    //�����ҽ���е����йؼ���
    for (c = 1; c <= q->keynum; c++) {
        l->keynum++;
        l->key[l->keynum] = q->key[c];
        l->ptr[l->keynum] = q->ptr[c];
    }

    //ɾ�����ڵ����йؼ���
    for (c = i; c < p->keynum; c++) {
        p->key[c] = p->key[c + 1];
        p->ptr[c] = p->ptr[c + 1];
    }
    p->keynum--;
    free(q); //�ͷſ��ҽ��Ŀռ�
}

//�ؼ��ֱ�ɾ���󣬵���B-�����ҵ�һ���ؼ��ֽ�����뵽p->ptr[i]��
void restore(BTNode *p, int i) {
    //Ϊ����߹ؼ��ֵ����
    if (i == 0) if (p->ptr[1]->keynum > 0)
        moveLeft(p, 1);
    else
        combine(p, 1);
    else if (i == p->keynum) if (p->ptr[i - 1]->keynum > 0)
        moveRight(p, i);
    else
        combine(p, i);
    else if (p->ptr[i - 1]->keynum > 0)//Ϊ�������
        moveRight(p, i);
    else if (p->ptr[i + 1]->keynum > 0)
        moveLeft(p, i + 1);
    else
        combine(p, i);
}


//�ڽ��p���ҹؼ���ΪK��λ��i���ɹ�ʱ����1�����򷵻�0
int SearchNode(KeyType k, BTNode *p, int i) {
    //kС��*p������С�ؼ���ʱ����0
    if (k.booknum < p->key[1].booknum) {
        i = 1;
        return 0;
    } else {
        //��*p����в���
        i = p->keynum;
        while (k.booknum < p->key[i].booknum && i > 1)
            i--;
        return (k.booknum == p->key[i].booknum);
    }
}

//���Ҳ�ɾ���ؼ���k
int recDelete(KeyType k, BTNode *p) {
    int i;
    int found;
    if (p == NULL)
        return 0;
    else {
        //���ҹؼ���k
        if ((found = SearchNode(k, p, i)) == 1) {
            //��Ϊ��Ҷ�ӽ��
            if (p->ptr[i - 1] != NULL) {
                successor(p, i); // �����̴�����
                recDelete(p->key[i], p->ptr[i]); //p->key[i]��Ҷ�ӽ����
            } else
                Remove(p, i); //��*p�����λ��i��ɾ���ؼ���
        } else
            found = recDelete(k, p->ptr[i]); //�غ��ӽ��ݹ���Ҳ�ɾ���ؼ�k

        if (p->ptr[i] != NULL) if (p->ptr[i]->keynum < 0)  //ɾ����ؼ��ָ���С��MIN
            restore(p, i);
        return found;
    }
}


//��B-��root��ɾ���ؼ���k������һ�������ɾ��ָ���Ĺؼ��֣������������ؼ��֣���ɾ���ý��
void DeleteBTree(KeyType k, BTNode *root) {
    BTNode *p; //�����ͷ�һ���յ�root
    if (recDelete(k, root) == 0)
        printf("�ؼ��ֲ���B-����\n");
    else if (root->keynum == 0) {
        p = root;
        root = root->ptr[0];
        free(p);
    }
}

//��q����i��λ�ò���ؼ���
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


//��B���в���һ���ؼ���
void InsertNode(BTree t, KeyType *key) {
    Result result;
    result = searchBTree(t, key);
    if (!result.tag) {
        InsertBTree(t, key, result.pt, result.i);
    }
}


//////ͼ���������
//ͼ��ɱ����
void Inputku(BTree *r) {
    Book book;
    Result result;
    printf("�������  ����  �ִ���  ����� ���ߵ�˳�����������Ϣ�����������<0ʱ������\n");
    while (1) {
        /*printf("���������:");*/
        scanf("%d", &book.booknum);
        if (book.booknum < 0)
            break;
        scanf("%s", &book.bookname[20]);
        scanf("%d", &book.now);
        scanf("%d", &book.total);
        scanf("%s", &book.writer[20]);
        book.flag = book.now; //��ʾ����û�б����
        result = searchBTree(*r, &book);
        if (result.tag == 0)
            InsertNode(*r, &book);
        else {
            result.pt->key[result.i].now += book.now;
            result.pt->key[result.i].flag += book.flag;
        }
    }
    printf("ͼ��ɱ����ɹ�!\n");
}

//����ͼ��
void lend(BTree *r) {
    KeyType b;
    int j;
    Result result;
    printf("�������������źͽ���ı�����");
    scanf("%d", &b.booknum);
    scanf("%d", &j);
    result = searchBTree(*r, &b);
    if (b.booknum < 0) {
        printf("�������\n");
        return;
    }

    if (result.tag == 0) {
        printf("��Ǹ��û�д���!\n");
    } else {
        if (result.pt->key[result.i].now >= j) {
            result.pt->key[result.i].now -= j;
            printf("����ɹ���\n����������Ϣ��\n���:%d ���:%d��\n��ȷ����Ϣ��\n", b.booknum, j);

        } else
            printf("�����ִ治����\n");
        if (result.pt->key[result.i].flag == 0)
            printf("�����������棬��ɱ����!\n");

    }
}

//�黹ͼ�麯��
void returnBook(BTree *r) {
    Book book;
    int k;
    Result result;
    printf("������黹�������ź͹黹�ı�����");
    scanf("%d", &book.booknum);
    scanf("%d", &k);
    if (book.booknum < 0) {
        printf("�������");
        return;
    }

    result = searchBTree(*r, &book);
    if (result.tag == 0)
        printf("û�д�ͼ�飡\n");
    else {
        if (k <= result.pt->key[result.i].total - result.pt->key[result.i].now) {
            result.pt->key[result.i].now = result.pt->key[result.i].now + k;
            printf("����ɹ���\n\n");
        } else if (k > result.pt->key[result.i].total - result.pt->key[result.i].now) {
            printf("\n��������ܴ�����������������ԣ�\n");
        }
    }
}


//���뷨��ʾͼ��
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
            printf("��ţ�%d,����:%s,����:%s,�ִ棺%d,�ܿ�棺%d", r->key[i].booknum, r->key[i].bookname, r->key[i].writer,
                   r->key[i].now, r->key[i].total);
            if (r->key[i].flag == 0)
                printf("------>������������!\n");
            else
                printf("\n");
        }
        i++;
    }
    output(r->ptr[i - 1], n + 1);
}


//�����溯��
void clearKuCun(BTree *r) {
    KeyType b;
    Result result;
    printf("��������Ҫ������������Ϣ���:");
    scanf("%d", &b.booknum);
    result = searchBTree(*r, &b);
    if (b.booknum < 0) {
        printf("�������");
        return;
    }

    if (result.tag == 0)
        printf("û�д�ͼ�飡\n");
    else {
        result.pt->key[result.i].total = 0;
        result.pt->key[result.i].now = 0;
        result.pt->key[result.i].flag = 0;
        printf("����ɹ���\n");
    }
}


////////////////������
int main() {
    BTree root = NULL;
    int t, n;

    L1:
    printf("**************************************\n");
    printf("**********��ӭʹ�ñ�ϵͳ*****************\n");
    printf("**************************************\n\n");
    printf("|        1��ͼ�����Ա��½              |\n\n");
    printf("|        2����ͨ�����ߵ�½              |\n\n");
    printf("|        3���˳�                      |\n\n");
    printf("*********��ѡ���½״̬*****************\n");
    printf("\n");
    printf("�������ţ�");
    scanf("%d", &n);
    if (n == 1) {
        while (1) {
            printf("**************��ӭʹ��ͼ�����ϵͳ*******\n");
            printf("**************����Աʹ��״̬************\n");
            printf("|                                    |\n");
            printf("|  1��ͼ�����                         |\n");
            printf("|                                    |\n");
            printf("|  2��������                         |\n");
            printf("|                                    |\n");
            printf("|  3������ͼ��                         |\n");
            printf("|                                    |\n");
            printf("|  4���黹ͼ��                         |\n");
            printf("|                                    |\n");
            printf("|  5����ѯͼ����Ϣ                      |\n");
            printf("|                                    |\n");
            printf("|  0������                            |\n");
            printf("**************��ѡ�����************\n");
            printf("��ѡ����Ҫ���еĲ�����");
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
                    printf("�������");
                    goto L3;
            }
        }
    } else if (n == 2) {
        while (1) {
            printf("*********��ӭʹ��ͼ�����ϵͳ********\n");
            printf("*******�û�ʹ��״̬********\n");
            printf("|                                    |\n");
            printf("|  1����ʾ����ͼ��                            |\n");
            printf("|                                    |\n");
            printf("|  0���˳�                           |\n");
            printf("|                                    |\n");
            printf("***************��ѡ�����***************\n");
            printf("\n");
            L3:
            printf("��ѡ����Ҫ���еĲ�����");
            scanf("%d", &t);
            switch (t) {
                case 0:
                    goto L1;
                    break;
                case 1:
                    output(root, 0);
                    break;
                default:
                    printf("�������");
                    goto L3;
                    break;
            }
        }
    } else if (n == 3)
        printf("               �ɹ��˳���            \n");
    else {
        printf("����������������룡\n");
        goto L1;
    }
}
