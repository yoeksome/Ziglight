

#ifndef LIST_H
#define LIST_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "OSAL.h"
  
  
 
typedef struct Node  
{     
    uint8 data;
    
    struct Node *next;  
}Node, *pNode,*LinkList; 

void InitList(LinkList *list);//��ʼ�������������յĵ�����  
void ClearList(LinkList *list);//��յ�����  
  
void ListInsert(LinkList *list,uint8 i, uint8 e);//�������i��λ�ú���������e  
void DestoryList(LinkList *list);//��������  
bool ListEmpty(LinkList list);//�жϵ������Ƿ�Ϊ�գ�Ϊ�շ�����  
void GetElem(LinkList, uint8 *e, uint8 i);//���������е�i��λ�õ�ֵ���ظ�����e  
void ListDelete(LinkList *list, uint8 i, uint8 *e);//ɾ����������i��λ��Ԫ��  
void ListTraverse(LinkList list);//�������Ա�  
uint8 ListLength(LinkList list);//�������Ա�ĳ���  
void Recursion(LinkList list);//�ݹ�������������� 

#ifdef __cplusplus
}
#endif

#endif /* LIST_H */