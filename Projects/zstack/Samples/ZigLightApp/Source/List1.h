
#ifndef LIST1_H
#define LIST1_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "OSAL.h"
#include "AF.h"
#define MAX_MISS_HEARTBEAT_CNT 3
typedef uint8 elemType ;
 
/************************************************************************/
/*             以下是关于线性表链接存储（单链表）操作的18种算法        */
 
/* 1.初始化线性表，即置单链表的表头指针为空 */
/* 2.创建线性表，此函数输入负数终止读取数据*/
/* 3.打印链表，链表的遍历*/
/* 4.清除线性表L中的所有元素，即释放单链表L中所有的结点，使之成为一个空表 */
/* 5.返回单链表的长度 */
/* 6.检查单链表是否为空，若为空则返回１，否则返回０ */
/* 7.返回单链表中第pos个结点中的元素，若pos超出范围，则停止程序运行 */
/* 8.从单链表中查找具有给定值x的第一个元素，若查找成功则返回该结点data域的存储地址，否则返回NULL */
/* 9.把单链表中第pos个结点的值修改为x的值，若修改成功返回１，否则返回０ */
/* 10.向单链表的表头插入一个元素 */
/* 11.向单链表的末尾添加一个元素 */
/* 12.向单链表中第pos个结点位置插入元素为x的结点，若插入成功返回１，否则返回０ */
/* 13.向有序单链表中插入元素x结点，使得插入后仍然有序 */
/* 14.从单链表中删除表头结点，并把该结点的值返回，若删除失败则停止程序运行 */
/* 15.从单链表中删除表尾结点并返回它的值，若删除失败则停止程序运行 */
/* 16.从单链表中删除第pos个结点并返回它的值，若删除失败则停止程序运行 */
/* 17.从单链表中删除值为x的第一个结点，若删除成功则返回1,否则返回0 */
/* 18.交换2个元素的位置 */
/* 19.将线性表进行快速排序 */
 
 
/************************************************************************/
typedef struct Node{    /* 定义单链表结点类型 */
    elemType element;//0：无需更新设备mac ； 1：新设备mac需要更新到gizwit
//    uint8 shortaddr[2];
    uint8 extaddr[8];
    uint16  shortaddr;
    uint8 heartbeatCnt;//心跳包计数值
    uint8 heartbeatCntOld;//心跳包上一次计数值
    uint8 detectHeartbeatCnt;//检测心跳包没有增长时 计数累计
    struct Node *next;
    struct Node *prior;
}Node;


void initList(Node **pNode);
//Node *creatList(Node *pHead , uint8 * extaddr);
Node* searchfromList(Node *pHead , uint8 * extaddr);
Node* searchshortaddfromList(Node *pHead , uint16  shortaddr);
//void clearList(Node *pHead);
//uint8 sizeList(Node *pHead);
//uint8 isEmptyList(Node *pHead);
//elemType getElement(Node *pHead, uint8 pos);
//elemType *getElemAddr(Node *pHead, elemType x);
//uint8 modifyElem(Node *pNode,uint8 pos,elemType x);
//uint8 insertHeadList(Node **pNode,elemType insertElem);
uint8 insertLastList(Node **pHead , uint8 * extaddr , uint16 shortAddr);
uint8 detectHandle(Node **pNode);//三秒扫描一次 

uint8 mac_table_fresh( afIncomingMSGPacket_t *pkt );
//uint8 deleteOneList(Node *pNode,elemType insertElem);

#ifdef __cplusplus
}
#endif

#endif /* LIST1_H */

