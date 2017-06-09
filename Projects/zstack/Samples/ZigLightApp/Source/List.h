

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

void InitList(LinkList *list);//初始化操作，建立空的单链表  
void ClearList(LinkList *list);//清空单链表。  
  
void ListInsert(LinkList *list,uint8 i, uint8 e);//单链表第i个位置后面插入变量e  
void DestoryList(LinkList *list);//销毁链表  
bool ListEmpty(LinkList list);//判断单链表是否为空，为空返回真  
void GetElem(LinkList, uint8 *e, uint8 i);//将单链表中第i个位置的值返回给变量e  
void ListDelete(LinkList *list, uint8 i, uint8 *e);//删除单链表表第i个位置元素  
void ListTraverse(LinkList list);//遍历线性表  
uint8 ListLength(LinkList list);//返回线性表的长度  
void Recursion(LinkList list);//递归逆序输出单链表 

#ifdef __cplusplus
}
#endif

#endif /* LIST_H */