/**
************************************************************
* @file         gizwits_product.c
* @brief        Gizwits 控制协议处理,及平台相关的硬件初始�?
* @author       Gizwits
* @date         2016-09-05
* @version      V03010101
* @copyright    Gizwits
* 
* @note         机智�?只为智能硬件而生
*               Gizwits Smart Cloud  for Smart Products
*               链接|增值ֵ|开放|中立|安全|自有|自由|生�?*               www.gizwits.com
*
***********************************************************/

#include <stdio.h>
#include <string.h>
#include "gizwits_protocol.h"
#include "List1.h"
#define SERIAL_INCOMING_MSG                  0xE1//by liu
int8 gizwitsEventProcess(eventInfo_t *info, uint8 *data, uint32 len);//by liu
void gizTimerMs(void);//by liu
uint32 gizGetTimerCount(void);//by liu
void TIMER_IRQ_FUN(void);//by liu
void UART_IRQ_FUN(void);//by liu
extern uint16 HalUARTWrite(uint8 port, uint8 *buf, uint16 len);//by liu
uint8 MacToAppFlag = 0;//by liu
uint8 AutoUpateMac = 0;//by liu
extern Node *nodelist;//by liu
extern uint8 ZigLightApp_TaskID;
typedef struct
{
  osal_event_hdr_t  hdr;
  uint8             *msg;
} gizOSALData_t;
/**@} */
/**@name Gizwits 用户API接口
* @{
*/
extern dataPoint_t currentDataPoint;

/**
* @brief 事件处理接口

* 说明�?
* 1.用户可以对WiFi模组状态的变化进行自定义的处理

* 2.用户可以在该函数内添加数据点事件处理逻辑，如调用相关硬件外设的操作接�?
* @param[in] info : 事件队列
* @param[in] data : 协议数据
* @param[in] len : 协议数据长度
* @return NULL
* @ref gizwits_protocol.h
*/
int8 gizwitsEventProcess(eventInfo_t *info, uint8 *data, uint32 len)
{
  uint8 i = 0;
  dataPoint_t *dataPointPtr = (dataPoint_t *)data;
  moduleStatusInfo_t *wifiData = (moduleStatusInfo_t *)data;
  protocolTime_t *ptime = (protocolTime_t *)data;
  static Node * nodeptr=NULL;
  static uint8 sn = 0;
  uint8 pos;
  gizOSALData_t  *pMsg;
  if((NULL == info) || (NULL == data))
  {
    return -1;
  }

  for(i=0; i<info->num; i++)
  {
    switch(info->event[i])
    {



      case EVENT_PAYLOAD:
        GIZWITS_LOG("Evt: EVENT_PAYLOAD\n");
        osal_memcpy((uint8 *)&currentDataPoint.valuepayload,"AutoUpdate DeviceMac",20);
        //user handle
        switch(dataPointPtr->valuepayload[0])
        {
        case 0x01://�����豸
          MacToAppFlag = 1;
//          if(nodeptr == NULL){
//            MacToAppFlag = 1;
//            nodeptr = nodelist;
//            sn = dataPointPtr->valueaa1[1];
//            for(i=0;i<20;i++)
//              currentDataPoint.valueaa1[i]=0;
//          }
//          for(pos=0;nodeptr&&pos<=1;nodeptr=nodeptr->next,pos++)
//          {
//            osal_memcpy((uint8 *)&currentDataPoint.valueaa1[3+8*pos],(uint8 *)&nodeptr->extaddr, 8);
//          }
//          if(nodeptr == NULL){
//            MacToAppFlag = 0;
//          }
//          currentDataPoint.valueaa1[0]=0x10;
//          currentDataPoint.valueaa1[1]=pos;
//          currentDataPoint.valueaa1[2]=sn;
          break;
        case 0x02://��������
          /* Allocate memory for the data */
          pMsg = (gizOSALData_t *)osal_msg_allocate( sizeof ( gizOSALData_t ) + 14 );
          pMsg->hdr.event = SERIAL_INCOMING_MSG;
          pMsg->msg = (uint8*)(pMsg+1);
          osal_memcpy((uint8 *)pMsg->msg,(uint8 *)&dataPointPtr->valuepayload[2], 14);
          osal_msg_send( ZigLightApp_TaskID, (byte *)pMsg );
          break;
        default:
          break;
        }
        
        break;

      case WIFI_SOFTAP:
        break;
      case WIFI_AIRLINK:
        break;
      case WIFI_STATION:
        break;
      case WIFI_CON_ROUTER:
        break;
      case WIFI_DISCON_ROUTER:
        break;
      case WIFI_CON_M2M:
        break;
      case WIFI_DISCON_M2M:
        break;
      case WIFI_RSSI:
        GIZWITS_LOG("RSSI %d\n", wifiData->rssi);
        break;
      case TRANSPARENT_DATA:
        GIZWITS_LOG("TRANSPARENT_DATA \n");
        //user handle , Fetch data from [data] , size is [len]
        break;
      case WIFI_NTP:
        GIZWITS_LOG("WIFI_NTP : [%d-%d-%d %02d:%02d:%02d][%d] \n",ptime->year,ptime->month,ptime->day,ptime->hour,ptime->minute,ptime->second,ptime->ntp);
        break;
      default:
        break;
    }
  }

  return 0;
}


/**@} */

/**
* @brief MCU复位函数

* @param none
* @return none
*/
void mcuRestart(void)
{

}

/**@} */
/**
* @brief 系统毫秒计时维护函数,毫秒自增,溢出归零

* @param none
* @return none
*/
//static uint32 timerMsCount;
//void gizTimerMs(void)
//{
//    timerMsCount++;
//}

/**
* @brief 读取系统时间毫秒计时�?
* @param none
* @return 系统时间毫秒�?*/
uint32 gizGetTimerCount(void)
{
    return osal_GetSystemClock();
}

/**
* @brief 定时器TIM3中断处理函数

* @param none
* @return none
*/
//void TIMER_IRQ_FUN(void)
//{
//  gizTimerMs();
//}

/**
* @brief USART2串口中断函数

* 接收功能，用于接收与WiFi模组间的串口协议数据
* @param none
* @return none
*/
void UART_IRQ_FUN(void)
{
  uint8 value = 0;
  //value = USART_ReceiveData(USART2);//STM32 test demo
  gizPutData(&value, 1);
}


/**
* @brief 串口写操作，发送数据到WiFi模组
*
* @param buf      : 数据地址
* @param len       : 数据长度
*
* @return : 正确返回有效数据长度;-1，错误返�?*/
uint32 uartWrite(uint8 *buf, uint32 len)
{
    uint32 i = 0;
    uint8 appendex=0x55;
    if(NULL == buf || len == 0)
    {
        return 0;
    }
#ifdef PROTOCOL_DEBUG
    GIZWITS_LOG("MCU2WiFi[%4d:%4d]: ", gizGetTimerCount(), len);
#endif
    
    for(i=0; i<len; i++)
    {
        //USART_SendData(UART, buf[i]);//STM32 test demo
        //实现串口发送函�?将buf[i]发送到模组
      HalUARTWrite(0,&buf[i],1);
#ifdef PROTOCOL_DEBUG
        GIZWITS_LOG("%02x ", buf[i]);
#endif
        if(i >=2 && buf[i] == 0xFF)
        {
          //实现串口发送函�?�?x55发送到模组
          HalUARTWrite(0,&appendex,1);
#ifdef PROTOCOL_DEBUG
        GIZWITS_LOG("%02x ", 0x55);
#endif
        }
    }
    
#ifdef PROTOCOL_DEBUG
    GIZWITS_LOG("\n");
#endif
    
    return len;
}


