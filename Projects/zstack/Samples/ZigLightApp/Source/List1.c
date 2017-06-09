
#include "List1.h"
#include "mt_uart.h"
#include "ZDProfile.h"
#include "ZDApp.h"
extern Node *nodelist;
extern Node *deletnodelist;
extern uint8 newdevice_joinin;//���O�������I =0��ʾ������e =1����ɹ�
uint8 currentMac[8];
uint8 NewMacUpate=0;//by liu
extern uint8 ZDAppTaskID;
/* 1.��ʼ�����Ա����õ�����ı�ͷָ��Ϊ�� */
void initList(Node **pNode)
{
//  uint8 i;
  *pNode = NULL;
//  pNode = (Node *)osal_mem_alloc(sizeof(Node)); //�����½ڵ�
  (*pNode)->next = NULL;
  (*pNode)->prior = NULL;
//  for(i=0;i<8;i++)
//    pNode->extaddr[i] = 0;
//  pNode->heartbeatCnt = 0;
//  pNode->heartbeatCntOld = 0;
//  pNode->detectHeartbeatCnt = 0;
  //pruint8f("initList����ִ�У���ʼ���ɹ�\n");
}
 
/* 2.�������Ա��˺������븺����ֹ��ȡ����*/
//Node *creatList(Node *pHead , uint8 * extaddr)
//{
//  Node *p1,*p2;
//  p2 = pHead;
//  while(p2->next != NULL)
//    p2 = p2->next;
//  p1=(Node *)osal_mem_alloc(sizeof(Node)); //�����½ڵ�
//  if(p1 == NULL)
//  {
//    //pruint8f("�ڴ����ʧ��\n");
//    return 0;
//  }
//  p2->next = p1;
//  osal_memcmp(p1->extaddr,extaddr,8);
//  
//  p1->next = NULL;         //�½ڵ��ָ����Ϊ��
//
//  return pHead;           //���������ͷָ��
//}
 
/* 3.��ӡ��������ı���*/
Node* searchfromList(Node *pHead , uint8 * extaddr)
{
  Node *p1;
  p1 = pHead;
  if(NULL == p1)   //����Ϊ��
  {
    //pruint8f("Pruint8List����ִ�У�����Ϊ��\n");
    return NULL;
  }
  else
  {
    while(NULL != p1)
    {
      if(osal_memcmp(p1->extaddr,extaddr,8) == true){
        p1->heartbeatCnt++;
        return p1;
      }
      p1 = p1->next;
    }
    return NULL;
  }
}
 
Node* searchshortaddfromList(Node *pHead , uint16  shortaddr)
{
  Node *p1;
  p1 = pHead;
  if(NULL == p1)   //����Ϊ��
  {
    //pruint8f("Pruint8List����ִ�У�����Ϊ��\n");
    return NULL;
  }
  else
  {
    while(NULL != p1)
    {
      if(p1->shortaddr == shortaddr){
        p1->heartbeatCnt++;
        return p1;
      }
      p1 = p1->next;
    }
    return NULL;
  }
}
/* 4.������Ա�L�е�����Ԫ�أ����ͷŵ�����L�����еĽ�㣬ʹ֮��Ϊһ���ձ� */
//void clearList(Node *pHead)
//{
//  Node *pNext;            //����һ����pHead���ڽڵ�
//  
//  if(pHead == NULL)
//  {
//    //pruint8f("clearList����ִ�У�����Ϊ��\n");
//    return;
//  }
//  while(pHead->next != NULL)
//  {
//    pNext = pHead->next;//������һ����ָ��
//    osal_mem_free(pHead);
//    pHead = pNext;      //��ͷ����
//  }
//  //pruint8f("clearList����ִ�У������Ѿ����\n");
//}
 
/* 5.���ص�����ĳ��� */
//uint8 sizeList(Node *pHead)
//{
//  uint8 size = 0;
//  
//  while(pHead != NULL)
//  {
//    size++;         //��������size��С�������ʵ�ʳ���С1
//    pHead = pHead->next;
//  }
//  return size;    //�����ʵ�ʳ���
//}
 
/* 6.��鵥�����Ƿ�Ϊ�գ���Ϊ���򷵻أ������򷵻أ� */
//uint8 isEmptyList(Node *pHead)
//{
//  if(pHead == NULL)
//  {
//    //pruint8f("isEmptyList����ִ�У�����Ϊ��\n");
//    return 1;
//  }
//  //pruint8f("isEmptyList����ִ�У�����ǿ�\n");
//  
//  return 0;
//}
 
/* 7.���ص������е�pos������е�Ԫ�أ���pos������Χ����ֹͣ�������� */
//elemType getElement(Node *pHead, uint8 pos)
//{
//  uint8 i=0;
//  
//  if(pos < 1)
//  {
//    //        pruint8f("getElement����ִ�У�posֵ�Ƿ�\n");
//    return 0;
//  }
//  if(pHead == NULL)
//  {
//    //        pruint8f("getElement����ִ�У�����Ϊ��\n");
//    return 0;
//    //exit(1);
//  }
//  while(pHead !=NULL)
//  {
//    ++i;
//    if(i == pos)
//    {
//      break;
//    }
//    pHead = pHead->next; //�Ƶ���һ���
//  }
//  if(i < pos)                  //�����Ȳ������˳�
//  {
//    //        pruint8f("getElement����ִ�У�posֵ����������\n");
//    return 0;
//  }
//  
//  return pHead->element;
//}

/* 8.�ӵ������в��Ҿ��и���ֵx�ĵ�һ��Ԫ�أ������ҳɹ��򷵻ظý��data��Ĵ洢��ַ�����򷵻�NULL */
//elemType *getElemAddr(Node *pHead, elemType x)
//{
//    if(NULL == pHead)
//    {
////        pruint8f("getElemAddr����ִ�У�����Ϊ��\n");
//        return NULL;
//    }
//    if(x < 0)
//    {
////        pruint8f("getElemAddr����ִ�У�����ֵX���Ϸ�\n");
//        return NULL;
//    }
//    while((pHead->element != x) && (NULL != pHead->next)) //�ж��Ƿ�����ĩβ���Լ��Ƿ������Ҫ�ҵ�Ԫ��
//    {
//        pHead = pHead->next;
//    }
//    if((pHead->element != x) && (pHead != NULL))
//    {
////        pruint8f("getElemAddr����ִ�У���������δ�ҵ�xֵ\n");
//        return NULL;
//    }
//    if(pHead->element == x)
//    {
////        pruint8f("getElemAddr����ִ�У�Ԫ�� %d �ĵ�ַΪ 0x%x\n",x,&(pHead->element));
//    }
// 
//    return &(pHead->element);//����Ԫ�صĵ�ַ
//}
 
/* 9.�ѵ������е�pos������ֵ�޸�Ϊx��ֵ�����޸ĳɹ����أ������򷵻أ� */
//uint8 modifyElem(Node *pNode,uint8 pos,elemType x)
//{
//    Node *pHead;
//    pHead = pNode;
//    uint8 i = 0;
// 
//    if(NULL == pHead)
//    {
////        pruint8f("modifyElem����ִ�У�����Ϊ��\n");
//    }
//    if(pos < 1)
//    {
////        pruint8f("modifyElem����ִ�У�posֵ�Ƿ�\n");
//        return 0;
//    }
//    while(pHead !=NULL)
//    {
//        ++i;
//        if(i == pos)
//        {
//            break;
//        }
//        pHead = pHead->next; //�Ƶ���һ���
//    }
//    if(i < pos)                  //�����Ȳ������˳�
//    {
////        pruint8f("modifyElem����ִ�У�posֵ����������\n");
//        return 0;
//    }
//    pNode = pHead;
//    pNode->element = x;
////    pruint8f("modifyElem����ִ��\n");
//     
//    return 1;
//}
 
/* 10.������ı�ͷ����һ��Ԫ�� */
//uint8 insertHeadList(Node **pNode,elemType insertElem)
//{
//  Node *pInsert;
//  pInsert = (Node *)osal_mem_alloc(sizeof(Node));
//  osal_memset(pInsert,0,sizeof(Node));
//  pInsert->element = insertElem;
//  pInsert->next = *pNode;
//  *pNode = pInsert;
//  //pruint8f("insertHeadList����ִ�У����ͷ����Ԫ�سɹ�\n");
//  
//  return 1;
//}

/* 11.�������ĩβ���һ��Ԫ�� */
//uint8 insertLastList(Node *pNode,elemType insertElem)
//{
//  Node *pInsert;
//  Node *pHead;
//  Node *pTmp; //����һ����ʱ����������ŵ�һ���ڵ�
//  
//  pHead = pNode;
//  pTmp = pHead;
//  pInsert = (Node *)osal_mem_alloc(sizeof(Node)); //����һ���½ڵ�
//  osal_memset(pInsert,0,sizeof(Node));
//  pInsert->element = insertElem;
//  
//  while(pHead->next != NULL)
//  {
//    pHead = pHead->next;
//  }
//  pHead->next = pInsert;   //������ĩβ�ڵ����һ���ָ������ӵĽڵ�
////  pNode = pTmp;
//  //pruint8f("insertLastList����ִ�У����β����Ԫ�سɹ�\n");
//  
//  return 1;
//}

/************
* insertLastList 
* ������ pHead��l�^�� extaddr�O��mac
* ���أ�0���ܷ�����g����O����Ϣ��1�ɹ�����O��
***************/
uint8 insertLastList(Node **pHead , uint8 * extaddr , uint16 shortAddr)
{
  uint8 asc_16[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  uint8 i;
  Node *p1,*p2,*tp;
  p2 = *pHead;
  tp = p2;
  if(*pHead == NULL){
    (*pHead)=(Node *)osal_mem_alloc(sizeof(Node)); //�����½ڵ�
    if((*pHead) == NULL)
    {
      //pruint8f("�ڴ����ʧ��\n");
      HalUARTWrite(0,"\r\nlist memalloc error",21);
      return 2;         //����error
    }
    (*pHead)->element = 1;
    (*pHead)->shortaddr = shortAddr;
    (*pHead)->heartbeatCnt = 0;
    (*pHead)->heartbeatCntOld = 0;
    (*pHead)->detectHeartbeatCnt = 0;
    (*pHead)->next = NULL;
    (*pHead)->prior = NULL;
    osal_memcpy((*pHead)->extaddr,extaddr,8);
    if(1)//uart test
    {
      HalUARTWrite(0,"\nA:",3);
      for(i=0;i<8;i++)
      {
        HalUARTWrite(0,&asc_16[extaddr[i]%256/16],1);
        HalUARTWrite(0,&asc_16[extaddr[i]%16],1);  
      }
      HalUARTWrite(0,"\n",1);
    }
    NewMacUpate = 1;
    return 1;           //����ok
  }
  while(p2->next != NULL)
    p2 = p2->next;
  p1=(Node *)osal_mem_alloc(sizeof(Node)); //�����½ڵ�
  if(p1 == NULL)
  {
    //pruint8f("�ڴ����ʧ��\n");
    HalUARTWrite(0,"\r\nlist memalloc error",21);
    return 2;         //����error
  }
  p1->element = 1;
  p1->shortaddr = shortAddr;
  p1->heartbeatCnt = 0;
  p1->heartbeatCntOld = 0;
  p1->detectHeartbeatCnt = 0;
  p2->next = p1;
  p1->prior = p2;
  osal_memcpy(p1->extaddr,extaddr,8);
  
  p1->next = NULL;         //�½ڵ��ָ����Ϊ��
  *pHead = tp;
  if(1)//uart test
  {
    HalUARTWrite(0,"\nA:",3);
    for(i=0;i<8;i++)
    {
      HalUARTWrite(0,&asc_16[extaddr[i]%256/16],1);
      HalUARTWrite(0,&asc_16[extaddr[i]%16],1);  
    }
    HalUARTWrite(0,"\n",1);
  }
  NewMacUpate = 1;
  return 1;           //����ok
}
#pragma optimize=none
uint8 detectHandle(Node **pNode)//����ɨ��һ�� �����������MAX_MISS_HEARTBEAT_CNT���豸�����Ƴ�list�����ڷ��ͳ��豸mac��Ϣ
{
//  uint8 asc_16[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
//  uint8 i;
  Node *p1,*p2,*p3;
  p1 = *pNode;
  p2 = p1;
  while(p1 != NULL){
    if(p1->heartbeatCnt == p1->heartbeatCntOld)
      p1->detectHeartbeatCnt++;
    else{
      p1->heartbeatCntOld = p1->heartbeatCnt;
      p1->detectHeartbeatCnt = 0;
    }
    if(p1->detectHeartbeatCnt >= MAX_MISS_HEARTBEAT_CNT){
      p1->detectHeartbeatCnt = 0;
      p1->prior->next = p1->next;
      p1->next->prior = p1->prior;
      p3 = p1->next;
//      osal_mem_free(((Node*)p1));
      if(deletnodelist ==NULL){
        deletnodelist = p1;
        deletnodelist->next = NULL;
      }
      else{
        p2=deletnodelist;
        while(p2->next){p2=p2->next;}
        p2->next = p1;
        p2->next->next=NULL;
      }
      p1=p3;
      while(p3->prior != NULL)
        p3 = p3->prior;
      *pNode = p3;
//      (*pNode)->extaddr[2] = 0xff;
      //gizwit_status = 0;//���͸�������״̬����
      continue;
    }
    p1 = p1->next;
  }
//  *pNode = p2;
  return 1;
}
uint8 mac_table_fresh( afIncomingMSGPacket_t *pkt )
{
//  uint8 currentMac[8];
  Node *currentnode;
  if( (pkt->cmd.Data[0])==0 || (pkt->cmd.Data[0])==2 ){// �������������ݰ���������ʱ����mac list table
    if(true == APSME_LookupExtAddr(pkt->srcAddr.addr.shortAddr,currentMac)){
      currentnode = searchfromList(nodelist,currentMac);
      if(currentnode == NULL){
        newdevice_joinin = insertLastList(&nodelist,currentMac,pkt->srcAddr.addr.shortAddr);
//        newdevice_joinin = 1;
      }
      else
        currentnode->heartbeatCnt++;
      return 0;//�ҵ��豸mac
    }
    else if(currentnode=searchshortaddfromList(nodelist,pkt->srcAddr.addr.shortAddr)){
      currentnode->heartbeatCnt++;
    }
    else{
      ZDP_IEEEAddrReq(pkt->srcAddr.addr.shortAddr , ZDP_ADDR_REQTYPE_SINGLE, 0 ,0 );
      newdevice_joinin = 3;//�޷��ҵ�mac
//      HalUARTWrite(0,"LookupExtAddr err\r\n",19);
    }
  }
  return 1;//�Ҳ����豸mac
}
//uint8 deleteOneList(Node *pNode,elemType insertElem)
//{
//  Node *pInsert;
//  Node *pHead;
//  Node *pTmp; //����һ����ʱ����������ŵ�һ���ڵ�
//  
//  pHead = pNode;
//  pTmp = pHead;
//  pInsert = (Node *)osal_mem_alloc(sizeof(Node)); //����һ���½ڵ�
//  osal_memset(pInsert,0,sizeof(Node));
//  pInsert->element = insertElem;
//  
//  while(pHead->next != NULL)
//  {
//    pHead = pHead->next;
//  }
//  pHead->next = pInsert;   //������ĩβ�ڵ����һ���ָ������ӵĽڵ�
//  pNode = pTmp;
//  //pruint8f("insertLastList����ִ�У����β����Ԫ�سɹ�\n");
//  
//  return 1;
//}
