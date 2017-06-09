

#include "List.h"


void InitList(LinkList *list)  
{  
  *list = (LinkList)osal_mem_alloc(sizeof(Node));  
  (*list)->next = NULL;  
  (*list)->data = 0;  
}  
void ListInsert(LinkList *list, uint8 i , uint8 e)//第i个元素后面插入e  
{     
  LinkList p = *list;  
  
  if( i == 0)  
  {  
    pNode q = (LinkList)osal_mem_alloc(sizeof(Node));  
    q->next = NULL;  
    q->data = e;  
    p->next = q;  
  }  
  else  
  {     
    pNode p = (*list)->next;  
    uint8 j = 1;  
    
    while( p && i > j)  
    {  
      p = p->next;  
      ++j;  
    }  
    
    pNode q = (LinkList)osal_mem_alloc(sizeof(Node));  
    q->next = p->next;  
    q->data = e;  
    p->next = q;  
  }  
  
}  

void ListTraverse(LinkList list)  
{  
    pNode p = list;  
    if(p != NULL)  
    {  
      p = p->next;  
    }  
    else  
    {  
      return;
      }
        while(p)  
    {  
//        pruint8f("%d\t", p->data);  
        p = p->next;  
    }  
}  
void Recursion(LinkList list)//递归的方法逆序输出链表  
{   
    if(NULL==list)  
    {   
        return;  
    }   
    if(list->next!=NULL)  
    {  
     Recursion(list->next);  
     }  
//    pruint8f("%d\t",list->data);  
}  
bool ListEmpty(LinkList list)  
{  
    pNode p = list;  
  
    if(NULL == list->next)  
        return true;  
    else  
        return false;  
}  
void GetElem(LinkList list, uint8 *e, uint8 i)  
{  
    pNode p = list;  
      
    if( i < 0 || i > ListLength(list))  
        return ;  
    p = p->next;  
    uint8 j = 1;  
    while( j < i)  
        {  
            p = p->next;  
            j++;  
        }  
    e = &p->data;  
}  
void ListDelete(LinkList *list, uint8 i, uint8 *e)  
{  
    pNode p = *list;  
    if( i < 0 || i > ListLength(*list))  
        return ;  
    pNode q = p;  
    p = q->next;  
    uint8 j = 1;  
    while( j < i )  
        {   q = p;  
            p = p->next;  
            j++;  
        }  
    p->data = *e;  
    q->next = p->next;  
    osal_mem_free(p);  
      
  
      
}  
uint8 ListLength(LinkList list)  
{  
    pNode p = list;  
    uint8 i = 0;  
    p = p->next;  
    while(p)  
        {  
            p = p->next;  
            i++;  
        }  
    return i;  
}  
void ClearList (LinkList *list)  
{  
    pNode p = *list;  
    if( p != NULL)  
        p = p->next;  
    pNode q;  
    while( p )  
    {  
        q = p;  
        p = p->next;  
        osal_mem_free(q);  
    }  
      
}  
void DestoryList(LinkList *list)  
{  
    pNode p = *list;  
    if( p != NULL)  
        p = p->next;  
    pNode q;  
    while( p )  
    {  
        q = p;  
        p = p->next;  
        osal_mem_free(q);  
    }  
    osal_mem_free(*list);  
} 