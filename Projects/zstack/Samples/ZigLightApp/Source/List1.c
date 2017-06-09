
#include "List1.h"
#include "mt_uart.h"
#include "ZDProfile.h"
#include "ZDApp.h"
extern Node *nodelist;
extern Node *deletnodelist;
extern uint8 newdevice_joinin;//新設備加入標誌 =0標示加入出錯 =1加入成功
uint8 currentMac[8];
uint8 NewMacUpate=0;//by liu
extern uint8 ZDAppTaskID;
/* 1.初始化线性表，即置单链表的表头指针为空 */
void initList(Node **pNode)
{
//  uint8 i;
  *pNode = NULL;
//  pNode = (Node *)osal_mem_alloc(sizeof(Node)); //申请新节点
  (*pNode)->next = NULL;
  (*pNode)->prior = NULL;
//  for(i=0;i<8;i++)
//    pNode->extaddr[i] = 0;
//  pNode->heartbeatCnt = 0;
//  pNode->heartbeatCntOld = 0;
//  pNode->detectHeartbeatCnt = 0;
  //pruint8f("initList函数执行，初始化成功\n");
}
 
/* 2.创建线性表，此函数输入负数终止读取数据*/
//Node *creatList(Node *pHead , uint8 * extaddr)
//{
//  Node *p1,*p2;
//  p2 = pHead;
//  while(p2->next != NULL)
//    p2 = p2->next;
//  p1=(Node *)osal_mem_alloc(sizeof(Node)); //申请新节点
//  if(p1 == NULL)
//  {
//    //pruint8f("内存分配失败\n");
//    return 0;
//  }
//  p2->next = p1;
//  osal_memcmp(p1->extaddr,extaddr,8);
//  
//  p1->next = NULL;         //新节点的指针置为空
//
//  return pHead;           //返回链表的头指针
//}
 
/* 3.打印链表，链表的遍历*/
Node* searchfromList(Node *pHead , uint8 * extaddr)
{
  Node *p1;
  p1 = pHead;
  if(NULL == p1)   //链表为空
  {
    //pruint8f("Pruint8List函数执行，链表为空\n");
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
  if(NULL == p1)   //链表为空
  {
    //pruint8f("Pruint8List函数执行，链表为空\n");
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
/* 4.清除线性表L中的所有元素，即释放单链表L中所有的结点，使之成为一个空表 */
//void clearList(Node *pHead)
//{
//  Node *pNext;            //定义一个与pHead相邻节点
//  
//  if(pHead == NULL)
//  {
//    //pruint8f("clearList函数执行，链表为空\n");
//    return;
//  }
//  while(pHead->next != NULL)
//  {
//    pNext = pHead->next;//保存下一结点的指针
//    osal_mem_free(pHead);
//    pHead = pNext;      //表头下移
//  }
//  //pruint8f("clearList函数执行，链表已经清除\n");
//}
 
/* 5.返回单链表的长度 */
//uint8 sizeList(Node *pHead)
//{
//  uint8 size = 0;
//  
//  while(pHead != NULL)
//  {
//    size++;         //遍历链表size大小比链表的实际长度小1
//    pHead = pHead->next;
//  }
//  return size;    //链表的实际长度
//}
 
/* 6.检查单链表是否为空，若为空则返回１，否则返回０ */
//uint8 isEmptyList(Node *pHead)
//{
//  if(pHead == NULL)
//  {
//    //pruint8f("isEmptyList函数执行，链表为空\n");
//    return 1;
//  }
//  //pruint8f("isEmptyList函数执行，链表非空\n");
//  
//  return 0;
//}
 
/* 7.返回单链表中第pos个结点中的元素，若pos超出范围，则停止程序运行 */
//elemType getElement(Node *pHead, uint8 pos)
//{
//  uint8 i=0;
//  
//  if(pos < 1)
//  {
//    //        pruint8f("getElement函数执行，pos值非法\n");
//    return 0;
//  }
//  if(pHead == NULL)
//  {
//    //        pruint8f("getElement函数执行，链表为空\n");
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
//    pHead = pHead->next; //移到下一结点
//  }
//  if(i < pos)                  //链表长度不足则退出
//  {
//    //        pruint8f("getElement函数执行，pos值超出链表长度\n");
//    return 0;
//  }
//  
//  return pHead->element;
//}

/* 8.从单链表中查找具有给定值x的第一个元素，若查找成功则返回该结点data域的存储地址，否则返回NULL */
//elemType *getElemAddr(Node *pHead, elemType x)
//{
//    if(NULL == pHead)
//    {
////        pruint8f("getElemAddr函数执行，链表为空\n");
//        return NULL;
//    }
//    if(x < 0)
//    {
////        pruint8f("getElemAddr函数执行，给定值X不合法\n");
//        return NULL;
//    }
//    while((pHead->element != x) && (NULL != pHead->next)) //判断是否到链表末尾，以及是否存在所要找的元素
//    {
//        pHead = pHead->next;
//    }
//    if((pHead->element != x) && (pHead != NULL))
//    {
////        pruint8f("getElemAddr函数执行，在链表中未找到x值\n");
//        return NULL;
//    }
//    if(pHead->element == x)
//    {
////        pruint8f("getElemAddr函数执行，元素 %d 的地址为 0x%x\n",x,&(pHead->element));
//    }
// 
//    return &(pHead->element);//返回元素的地址
//}
 
/* 9.把单链表中第pos个结点的值修改为x的值，若修改成功返回１，否则返回０ */
//uint8 modifyElem(Node *pNode,uint8 pos,elemType x)
//{
//    Node *pHead;
//    pHead = pNode;
//    uint8 i = 0;
// 
//    if(NULL == pHead)
//    {
////        pruint8f("modifyElem函数执行，链表为空\n");
//    }
//    if(pos < 1)
//    {
////        pruint8f("modifyElem函数执行，pos值非法\n");
//        return 0;
//    }
//    while(pHead !=NULL)
//    {
//        ++i;
//        if(i == pos)
//        {
//            break;
//        }
//        pHead = pHead->next; //移到下一结点
//    }
//    if(i < pos)                  //链表长度不足则退出
//    {
////        pruint8f("modifyElem函数执行，pos值超出链表长度\n");
//        return 0;
//    }
//    pNode = pHead;
//    pNode->element = x;
////    pruint8f("modifyElem函数执行\n");
//     
//    return 1;
//}
 
/* 10.向单链表的表头插入一个元素 */
//uint8 insertHeadList(Node **pNode,elemType insertElem)
//{
//  Node *pInsert;
//  pInsert = (Node *)osal_mem_alloc(sizeof(Node));
//  osal_memset(pInsert,0,sizeof(Node));
//  pInsert->element = insertElem;
//  pInsert->next = *pNode;
//  *pNode = pInsert;
//  //pruint8f("insertHeadList函数执行，向表头插入元素成功\n");
//  
//  return 1;
//}

/* 11.向单链表的末尾添加一个元素 */
//uint8 insertLastList(Node *pNode,elemType insertElem)
//{
//  Node *pInsert;
//  Node *pHead;
//  Node *pTmp; //定义一个临时链表用来存放第一个节点
//  
//  pHead = pNode;
//  pTmp = pHead;
//  pInsert = (Node *)osal_mem_alloc(sizeof(Node)); //申请一个新节点
//  osal_memset(pInsert,0,sizeof(Node));
//  pInsert->element = insertElem;
//  
//  while(pHead->next != NULL)
//  {
//    pHead = pHead->next;
//  }
//  pHead->next = pInsert;   //将链表末尾节点的下一结点指向新添加的节点
////  pNode = pTmp;
//  //pruint8f("insertLastList函数执行，向表尾插入元素成功\n");
//  
//  return 1;
//}

/************
* insertLastList 
* 參數： pHead鏈錶頭； extaddr設備mac
* 返回：0不能分配空間添加設備信息，1成功添加設備
***************/
uint8 insertLastList(Node **pHead , uint8 * extaddr , uint16 shortAddr)
{
  uint8 asc_16[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  uint8 i;
  Node *p1,*p2,*tp;
  p2 = *pHead;
  tp = p2;
  if(*pHead == NULL){
    (*pHead)=(Node *)osal_mem_alloc(sizeof(Node)); //申请新节点
    if((*pHead) == NULL)
    {
      //pruint8f("内存分配失败\n");
      HalUARTWrite(0,"\r\nlist memalloc error",21);
      return 2;         //返回error
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
    return 1;           //返回ok
  }
  while(p2->next != NULL)
    p2 = p2->next;
  p1=(Node *)osal_mem_alloc(sizeof(Node)); //申请新节点
  if(p1 == NULL)
  {
    //pruint8f("内存分配失败\n");
    HalUARTWrite(0,"\r\nlist memalloc error",21);
    return 2;         //返回error
  }
  p1->element = 1;
  p1->shortaddr = shortAddr;
  p1->heartbeatCnt = 0;
  p1->heartbeatCntOld = 0;
  p1->detectHeartbeatCnt = 0;
  p2->next = p1;
  p1->prior = p2;
  osal_memcpy(p1->extaddr,extaddr,8);
  
  p1->next = NULL;         //新节点的指针置为空
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
  return 1;           //返回ok
}
#pragma optimize=none
uint8 detectHandle(Node **pNode)//三秒扫描一次 如果计数超过MAX_MISS_HEARTBEAT_CNT的设备将被移除list并串口发送出设备mac信息
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
      //gizwit_status = 0;//发送给机智云状态数据
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
  if( (pkt->cmd.Data[0])==0 || (pkt->cmd.Data[0])==2 ){// 发现请求建立数据包和心跳包时更新mac list table
    if(true == APSME_LookupExtAddr(pkt->srcAddr.addr.shortAddr,currentMac)){
      currentnode = searchfromList(nodelist,currentMac);
      if(currentnode == NULL){
        newdevice_joinin = insertLastList(&nodelist,currentMac,pkt->srcAddr.addr.shortAddr);
//        newdevice_joinin = 1;
      }
      else
        currentnode->heartbeatCnt++;
      return 0;//找到设备mac
    }
    else if(currentnode=searchshortaddfromList(nodelist,pkt->srcAddr.addr.shortAddr)){
      currentnode->heartbeatCnt++;
    }
    else{
      ZDP_IEEEAddrReq(pkt->srcAddr.addr.shortAddr , ZDP_ADDR_REQTYPE_SINGLE, 0 ,0 );
      newdevice_joinin = 3;//无法找到mac
//      HalUARTWrite(0,"LookupExtAddr err\r\n",19);
    }
  }
  return 1;//找不到设备mac
}
//uint8 deleteOneList(Node *pNode,elemType insertElem)
//{
//  Node *pInsert;
//  Node *pHead;
//  Node *pTmp; //定义一个临时链表用来存放第一个节点
//  
//  pHead = pNode;
//  pTmp = pHead;
//  pInsert = (Node *)osal_mem_alloc(sizeof(Node)); //申请一个新节点
//  osal_memset(pInsert,0,sizeof(Node));
//  pInsert->element = insertElem;
//  
//  while(pHead->next != NULL)
//  {
//    pHead = pHead->next;
//  }
//  pHead->next = pInsert;   //将链表末尾节点的下一结点指向新添加的节点
//  pNode = pTmp;
//  //pruint8f("insertLastList函数执行，向表尾插入元素成功\n");
//  
//  return 1;
//}
