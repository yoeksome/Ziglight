
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
/*             �����ǹ������Ա����Ӵ洢��������������18���㷨        */
 
/* 1.��ʼ�����Ա����õ�����ı�ͷָ��Ϊ�� */
/* 2.�������Ա��˺������븺����ֹ��ȡ����*/
/* 3.��ӡ��������ı���*/
/* 4.������Ա�L�е�����Ԫ�أ����ͷŵ�����L�����еĽ�㣬ʹ֮��Ϊһ���ձ� */
/* 5.���ص�����ĳ��� */
/* 6.��鵥�����Ƿ�Ϊ�գ���Ϊ���򷵻أ������򷵻أ� */
/* 7.���ص������е�pos������е�Ԫ�أ���pos������Χ����ֹͣ�������� */
/* 8.�ӵ������в��Ҿ��и���ֵx�ĵ�һ��Ԫ�أ������ҳɹ��򷵻ظý��data��Ĵ洢��ַ�����򷵻�NULL */
/* 9.�ѵ������е�pos������ֵ�޸�Ϊx��ֵ�����޸ĳɹ����أ������򷵻أ� */
/* 10.������ı�ͷ����һ��Ԫ�� */
/* 11.�������ĩβ���һ��Ԫ�� */
/* 12.�������е�pos�����λ�ò���Ԫ��Ϊx�Ľ�㣬������ɹ����أ������򷵻أ� */
/* 13.�����������в���Ԫ��x��㣬ʹ�ò������Ȼ���� */
/* 14.�ӵ�������ɾ����ͷ��㣬���Ѹý���ֵ���أ���ɾ��ʧ����ֹͣ�������� */
/* 15.�ӵ�������ɾ����β��㲢��������ֵ����ɾ��ʧ����ֹͣ�������� */
/* 16.�ӵ�������ɾ����pos����㲢��������ֵ����ɾ��ʧ����ֹͣ�������� */
/* 17.�ӵ�������ɾ��ֵΪx�ĵ�һ����㣬��ɾ���ɹ��򷵻�1,���򷵻�0 */
/* 18.����2��Ԫ�ص�λ�� */
/* 19.�����Ա���п������� */
 
 
/************************************************************************/
typedef struct Node{    /* ���嵥���������� */
    elemType element;//0����������豸mac �� 1�����豸mac��Ҫ���µ�gizwit
//    uint8 shortaddr[2];
    uint8 extaddr[8];
    uint16  shortaddr;
    uint8 heartbeatCnt;//����������ֵ
    uint8 heartbeatCntOld;//��������һ�μ���ֵ
    uint8 detectHeartbeatCnt;//���������û������ʱ �����ۼ�
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
uint8 detectHandle(Node **pNode);//����ɨ��һ�� 

uint8 mac_table_fresh( afIncomingMSGPacket_t *pkt );
//uint8 deleteOneList(Node *pNode,elemType insertElem);

#ifdef __cplusplus
}
#endif

#endif /* LIST1_H */

