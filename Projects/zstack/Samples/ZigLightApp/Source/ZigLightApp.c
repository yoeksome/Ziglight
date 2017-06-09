/**************************************************************************************************
  Filename:       ZigLightApp.c
  Revised:        $Date: 2009-03-18 15:56:27 -0700 (Wed, 18 Mar 2009) $
  Revision:       $Revision: 19453 $

  Description:    Sample Application (no Profile).


  Copyright 2007 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED 揂S IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/*********************************************************************
  This application isn't intended to do anything useful, it is
  intended to be a simple example of an application's structure.

  This application sends it's messages either as broadcast or
  broadcast filtered group messages.  The other (more normal)
  message addressing is unicast.  Most of the other sample
  applications are written to support the unicast message model.

  Key control:
    SW1:  Sends a flash command to all devices in Group 1.
    SW2:  Adds/Removes (toggles) this device in and out
          of Group 1.  This will enable and disable the
          reception of the flash command.
*********************************************************************/
/**
*   熟悉zigbee工程    2017/5/23 by liu
*   增加协调器与设备通信功能  2017/5/25 by liu
*   加入链表管理mac功能 2017/5/27 by liu
*   修改串口通信部分却掉串口调试功能 2017/6/5 by liu
*   加入机智云串口通信协议 2017/6/5 by liu
*   修改APSME_LookupExtAddr函数失败后请求mac地址功能  2017/6/5 by liu
*   修改了串口通信 2017/6/7 by liu
*   增加链表储存短地址功能   2017/6/7 by liu
*   增加短地址查询功能   2017/6/7 by liu
*   增加按键发送重新配置机智云功能 2017/6/8 by liu
*   处于配置态时1s闪烁 255次 2017/6/8 by liu
*   不处于配置态且没有连上路由器时2s闪烁 8次 2017/6/8 by liu
*   不处于配置态&&连上路由器&&没有连上机智云时3s闪烁 10次 2017/6/8 by liu
*   不处于配置态&&连上路由器&&连上机智云时长亮 2017/6/8 by liu
**/
/*********************************************************************
 * INCLUDES
 */
#include "OSAL.h"
#include "ZGlobals.h"
#include "AF.h"
#include "aps_groups.h"
#include "ZDApp.h"

#include "ZigLightApp.h"
#include "ZigLightAppHw.h"

#include "OnBoard.h"

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"

#include  "MT_UART.h" //此处用于串口
#include  "List1.h"
#include  "gizwits_protocol.h"
#include  "ZDObject.h"
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// This list should be filled with Application specific Cluster IDs.
const cId_t ZigLightApp_ClusterList[ZigLightApp_MAX_CLUSTERS] =
{
  ZigLightApp_PERIODIC_CLUSTERID,
  ZigLightApp_FLASH_CLUSTERID
};

const SimpleDescriptionFormat_t ZigLightApp_SimpleDesc =
{
  ZigLightApp_ENDPOINT,              //  int Endpoint;
  ZigLightApp_PROFID,                //  uint16 AppProfId[2];
  ZigLightApp_DEVICEID,              //  uint16 AppDeviceId[2];
  ZigLightApp_DEVICE_VERSION,        //  int   AppDevVer:4;
  ZigLightApp_FLAGS,                 //  int   AppFlags:4;
  ZigLightApp_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)ZigLightApp_ClusterList,  //  uint8 *pAppInClusterList;
  ZigLightApp_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)ZigLightApp_ClusterList   //  uint8 *pAppInClusterList;
};

// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in ZigLightApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
endPointDesc_t ZigLightApp_epDesc;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
uint8 ZigLightApp_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // ZigLightApp_Init() is called.
devStates_t ZigLightApp_NwkState;

uint8 ZigLightApp_TransID;  // This is the unique message ID (counter)

afAddrType_t ZigLightApp_Periodic_DstAddr;
afAddrType_t ZigLightApp_Flash_DstAddr;

afAddrType_t Point_To_Point_DstAddr;//网蜂点对点通信定义
/****************by liu ***********************/
afAddrType_t Self_DstAddr;//by liu
zAddrType_t Self_zDstAddr;//by liu
uint8 self_index=1;//by liu
uint8 sbuf[6];//by liu
uint8 sbuf_len;//by liu
uint8 on_off;//by liu
uint8 white,red,green,blue;//by liu
uint8 gizwits_buf[20];
enum mstate{
  POWER_ON=0X10,
  ESTABLISH_CONNECTION_REQ,//router & end send
  ESTABLISH_CONNECTION_RSQ,//coordinate send
  HEARTBEAT,//router & end send
  DEVICE_STATUS_CHANGE,//router & end send
  DEVICE_COMMAND//coordinate send
  
  };//by liu
uint8 main_state = ESTABLISH_CONNECTION_REQ;//by liu
Node *nodelist;
Node *deletnodelist=NULL;
uint8 newdevice_joinin=0;//新設備加入標誌 =0無效 =1加入成功 = 2內存分配出錯 = 3函数APSME_LookupExtAddr无法找到mac
#define SERIAL_INCOMING_MSG                  0xE1
//typedef struct
//{
//  osal_event_hdr_t  hdr;
//  uint8             *msg;
//} mtOSALSerialData_t;
extern uint8 currentMac[8];
extern uint8 MacToAppFlag ;//by liu
extern uint8 AutoUpateMac ;//by liu
dataPoint_t currentDataPoint;// by liu
extern uint8 blink_finished ;//by liu =1 完成闪烁 =0 没有完成闪烁
/****************by liu end***********************/
aps_Group_t ZigLightApp_Group;

uint8 ZigLightAppPeriodicCounter = 0;
uint8 ZigLightAppFlashCounter = 0;
dataPoint_t currentDataPoint;
/*********************************************************************
 * LOCAL FUNCTIONS
 */
void ZigLightApp_HandleKeys( uint8 shift, uint8 keys );
void ZigLightApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
void ZigLightApp_SendPeriodicMessage( void );
void ZigLightApp_SendFlashMessage( uint16 flashTime );
void ZigLightApp_SendPointToPointMessage( afAddrType_t * dstaddr , endPointDesc_t * epdesc , uint16 len , uint8 * buf  );
uint8 SendData_load(uint8 *sbuf);//by liu
void rfdata_handle( afIncomingMSGPacket_t *pkt );//by liu
void serial_data_handle(uint8 *ptr);//by liu
void ZigLightApp_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg );//by liu
/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      ZigLightApp_Init
 *
 * @brief   Initialization function for the Generic App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void ZigLightApp_Init( uint8 task_id )
{
//  uint8 i;
  ZigLightApp_TaskID = task_id;
  ZigLightApp_NwkState = DEV_INIT;
  ZigLightApp_TransID = 0;
  
  MT_UartInit();//串口初始化
  MT_UartRegisterTaskID(task_id);//登记任务号
  HalUARTWrite(0,"giziwit set mode\n",17); //（串口0，'字符'，字符个数。）
  osal_memset((uint8*)&currentDataPoint, 0, sizeof(dataPoint_t));
  gizwitsInit();
//  gizwitsSetMode(WIFI_AIRLINK_MODE);//airlink
  // Device hardware initialization can be added here or in main() (Zmain.c).
  // If the hardware is application specific - add it here.
  // If the hardware is other parts of the device add it in main().

 #if defined ( BUILD_ALL_DEVICES )
  // The "Demo" target is setup to have BUILD_ALL_DEVICES and HOLD_AUTO_START
  // We are looking at a jumper (defined in ZigLightAppHw.c) to be jumpered
  // together - if they are - we will start up a coordinator. Otherwise,
  // the device will start as a router.
  if ( readCoordinatorJumper() )
    zgDeviceLogicalType = ZG_DEVICETYPE_COORDINATOR;
  else
    zgDeviceLogicalType = ZG_DEVICETYPE_ROUTER;
#endif // BUILD_ALL_DEVICES

#if defined ( HOLD_AUTO_START )
  // HOLD_AUTO_START is a compile option that will surpress ZDApp
  //  from starting the device and wait for the application to
  //  start the device.
  ZDOInitDevice(0);
#endif

  // Setup for the periodic message's destination address
  // Broadcast to everyone
  ZigLightApp_Periodic_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
  ZigLightApp_Periodic_DstAddr.endPoint = ZigLightApp_ENDPOINT;
  ZigLightApp_Periodic_DstAddr.addr.shortAddr = 0xFFFF;
  
  // Setup for the flash command's destination address - Group 1
  ZigLightApp_Flash_DstAddr.addrMode = (afAddrMode_t)afAddrGroup;
  ZigLightApp_Flash_DstAddr.endPoint = ZigLightApp_ENDPOINT;
  ZigLightApp_Flash_DstAddr.addr.shortAddr = ZigLightApp_FLASH_GROUP;
  
  // 网蜂点对点通讯定义
  Point_To_Point_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;//点播
  Point_To_Point_DstAddr.endPoint = ZigLightApp_ENDPOINT;
  Point_To_Point_DstAddr.addr.shortAddr = 0x0000; //发给协调器
//    for(i=0;i<8;i++)
//      Point_To_Point_DstAddr.addr.extAddr[i] = 0x00; //发给协调器
//    //test by liu 
//    
//    Self_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;//点播
//    Self_DstAddr.endPoint = ZigLightApp_ENDPOINT;
//    Self_DstAddr.addr.shortAddr = 0x0000; //发给协调器
//    
//    Self_zDstAddr.addrMode = (afAddrMode_t)Addr16Bit;//点播
//    Self_zDstAddr.addr.shortAddr = 0x0000; //发给协调器
//    //end test
  /****************************by liu *****************************/
  initList(&nodelist);
//  insertLastList(&nodelist,"12345678");
//  insertLastList(&nodelist,"22045671");
  
  // Register callback evetns from the ZDApp
  ZDO_RegisterForZDOMsg( ZigLightApp_TaskID, IEEE_addr_rsp );
  /***************************by liu end ***************************/
  // Fill out the endpoint description.
  ZigLightApp_epDesc.endPoint = ZigLightApp_ENDPOINT;
  ZigLightApp_epDesc.task_id = &ZigLightApp_TaskID;
  ZigLightApp_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&ZigLightApp_SimpleDesc;
  ZigLightApp_epDesc.latencyReq = noLatencyReqs;

  // Register the endpoint description with the AF
  afRegister( &ZigLightApp_epDesc );

  // Register for all key events - This app will handle all key events
  RegisterForKeys( ZigLightApp_TaskID );

  // By default, all devices start out in Group 1
  ZigLightApp_Group.ID = 0x0001;
  osal_memcpy( ZigLightApp_Group.name, "Group 1", 7  );
  aps_AddGroup( ZigLightApp_ENDPOINT, &ZigLightApp_Group );

#if defined ( LCD_SUPPORTED )
  HalLcdWriteString( "ZigLightApp", HAL_LCD_LINE_1 );
#endif
}
/***********************************add by liu*****************************************/
/*****
ZigLightApp_NwkState
SendData_load 参数 ptr 缓存数组
返回：数组有效字节数
****/
uint8 SendData_load(uint8 *sbuf)
{
  uint8 i;
//  uint8 coordextaddr[8];
  switch(main_state){
  case ESTABLISH_CONNECTION_REQ:
    {
      if(ZigLightApp_NwkState != DEV_ZB_COORD){
        sbuf[0] = 0x00;
        if(ZigLightApp_NwkState == DEV_ROUTER)
          sbuf[1] = 0x01;
        else if(ZigLightApp_NwkState == DEV_END_DEVICE)
          sbuf[1] = 0x02;
        #ifdef GUOHUA_LIGHT
        sbuf[1] |= GUOHUA_LIGHT;
        #endif
        #ifdef GUOHUA_PLUG
        sbuf[1] |= GUOHUA_PLUG;
        #endif
        #ifdef GUOHUA_POWERSTRIP
        sbuf[1] |= GUOHUA_POWERSTRIP;
        #endif
      }
      else{
        sbuf[0] = 0xff;
        sbuf[1] = 0xff;
      }
      sbuf_len = 2;
//      NLME_GetCoordExtAddr( coordextaddr );
//      for(i=0;i<8;i++)
//        Point_To_Point_DstAddr.addr.extAddr[i] = coordextaddr[i]; //发给协调器
      break;
    }
  case ESTABLISH_CONNECTION_RSQ:
    {
      if(ZigLightApp_NwkState == DEV_ZB_COORD){
        sbuf[0] = 0x01;
        sbuf[1] = 1;//newdevice_joinin; // 根据链表空间大小处理 
        newdevice_joinin = 0;
      }
      else{
        sbuf[0] = 0xff;
        sbuf[1] = 0xff;
      }
      sbuf_len = 2;
      break;
    }
  case HEARTBEAT:
    {
      if(ZigLightApp_NwkState != DEV_ZB_COORD){
        sbuf[0] = 0x02;
      }
      else
        sbuf[0] = 0xff;
      sbuf_len = 1;
      break;
    }
  case DEVICE_STATUS_CHANGE:
    {
      if(ZigLightApp_NwkState != DEV_ZB_COORD){
        sbuf[0] = 0x04;
        sbuf[1] = on_off;
        sbuf[2] = white;
        sbuf[3] = red;
        sbuf[4] = green;
        sbuf[5] = blue;
      }
      else{
        sbuf[0] = 0xff;
        sbuf[1] = 0xff;
        sbuf[2] = 0xff;
        sbuf[3] = 0xff;
        sbuf[4] = 0xff;
        sbuf[5] = 0xff;
      }
      sbuf_len = 6;
      break;
    }
  case DEVICE_COMMAND://未完成 ！！！！！！！！！！！！！！！
    {
      sbuf[0] = 0x05;
      for(i=1;i<6;i++)
        sbuf[i] = gizwits_buf[i];
      sbuf_len = 6;
      break;
    }
  default:
    sbuf_len = 0;
  }
  return sbuf_len;
}
void serial_data_handle(uint8 *ptr)
{
  uint8 i;
  uint8 res;
  uint8 *serialMac;
  uint16 shortaddr;
  serialMac = &ptr[0];
  if(ZigLightApp_NwkState == DEV_ZB_COORD){
    if(ptr[8] != 0x05){//cmdtype
      HalUARTWrite(0,"error cmd type\r",15);
      return;
    }
    main_state = DEVICE_COMMAND;
    res = APSME_LookupNwkAddr(serialMac , &shortaddr);
    if(res == false){
      // 添加未找到设备回应信息
      HalUARTWrite(0,"can't find MAC device\r",22);
      return;
    }
    Point_To_Point_DstAddr.addr.shortAddr = shortaddr; //发给设备
    for(i=0;i<6;i++)
      gizwits_buf[i] = ptr[i+8];
    sbuf_len = SendData_load(sbuf);
    if(sbuf_len)
      ZigLightApp_SendPointToPointMessage(&Point_To_Point_DstAddr,&ZigLightApp_epDesc,sbuf_len,sbuf);
  }
}
void ZigLightApp_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg )
{
  uint8 i;
  uint16 temp;
  uint8 asc_16[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  Node *currentnode;
  switch ( inMsg->clusterID )
  {
    case IEEE_addr_rsp:
      {
        // Send find device callback to application
        ZDO_NwkIEEEAddrResp_t *pNwkAddrRsp = ZDO_ParseAddrRsp( inMsg );
        currentnode = searchfromList(nodelist,pNwkAddrRsp->extAddr);
        if(currentnode == NULL)
          newdevice_joinin = insertLastList(&nodelist,pNwkAddrRsp->extAddr,pNwkAddrRsp->nwkAddr);
        else
          currentnode->heartbeatCnt++;
        if(ZigLightApp_NwkState == DEV_ZB_COORD){
          /****将短地址分解，ASC码打印*****/
          temp=pNwkAddrRsp->nwkAddr; //读出数据包的16位短地址
          HalUARTWrite(0,&asc_16[temp/4096],1);
          HalUARTWrite(0,&asc_16[temp%4096/256],1);
          HalUARTWrite(0,&asc_16[temp%256/16],1);
          HalUARTWrite(0,&asc_16[temp%16],1);  
          HalUARTWrite(0,"\r\n",2);
          main_state = ESTABLISH_CONNECTION_RSQ;//返回建立標示rsq
          sbuf_len = SendData_load(sbuf);
          if(sbuf_len){
            Point_To_Point_DstAddr.addr.shortAddr = inMsg->srcAddr.addr.shortAddr;
            ZigLightApp_SendPointToPointMessage(&Point_To_Point_DstAddr,&ZigLightApp_epDesc,sbuf_len,sbuf);//返回建立標示rsq
          }
        }
//        ZDO_RemoveRegisteredCB(ZigLightApp_TaskID,IEEE_addr_rsp);
        if(0)//uart test
        {
          HalUARTWrite(0,"\nA:",3);
          for(i=0;i<8;i++)
          {
            HalUARTWrite(0,&asc_16[pNwkAddrRsp->extAddr[i]%256/16],1);
            HalUARTWrite(0,&asc_16[pNwkAddrRsp->extAddr[i]%16],1);  
          }
          HalUARTWrite(0,"\n",1);
        }
      }
      break;
  }
}
/***********************************end by liu*****************************************/
/*********************************************************************
 * @fn      ZigLightApp_ProcessEvent
 *
 * @brief   Generic Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
uint16 ZigLightApp_ProcessEvent( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
//  uint8 *msg_ptr;//by liu
//  uint8 i;//by liu
  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( ZigLightApp_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        case ZDO_CB_MSG:
          ZigLightApp_ProcessZDOMsgs( (zdoIncomingMsg_t *)MSGpkt );
          break;
        // Received when a key is pressed
        case KEY_CHANGE:
          ZigLightApp_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        // Received when a messages is received (OTA) for this endpoint
        case AF_INCOMING_MSG_CMD:
          ZigLightApp_MessageMSGCB( MSGpkt );
          break;

        // Received whenever the device changes state in the network
        case ZDO_STATE_CHANGE:
          ZigLightApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
//          if ( //(ZigLightApp_NwkState == DEV_ZB_COORD)|| //协调器不给自己点播
//               (ZigLightApp_NwkState == DEV_ROUTER)
//              || (ZigLightApp_NwkState == DEV_END_DEVICE) )
//          {
            // Start sending the periodic message in a regular interval.
            osal_start_timerEx( ZigLightApp_TaskID,
                              ZigLightApp_SEND_PERIODIC_MSG_EVT,
                              ZigLightApp_SEND_PERIODIC_MSG_TIMEOUT );
//          }
//          else
//          {
//            // Device is no longer in the network
//          }
          break;
        case SPI_INCOMING_ZTOOL_PORT:
          HalUARTWrite(0,"ZTOOL\r",6);
          break;
        case SERIAL_INCOMING_MSG:
          HalUARTWrite(0,"SERIAL\r",7);
          serial_data_handle(((mtOSALSerialData_t *)MSGpkt)->msg);

          break;
        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next - if one is available
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( ZigLightApp_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  // Send a message out - This event is generated by a timer
  //  (setup in ZigLightApp_Init()).
  if ( events & ZigLightApp_SEND_PERIODIC_MSG_EVT )
  {
    if(ZigLightApp_NwkState == DEV_ZB_COORD){
      if(MacToAppFlag == 0 && AutoUpateMac == 0){//在读取mac时禁止链表操作
        detectHandle(&nodelist);
      }
    }
    else if(ZigLightApp_NwkState == DEV_ROUTER || ZigLightApp_NwkState == DEV_END_DEVICE ){
      // Send the periodic message
//      if(main_state != ESTABLISH_CONNECTION_REQ)
//        return (events ^ ZigLightApp_SEND_PERIODIC_MSG_EVT);
      sbuf_len = SendData_load(sbuf);
      if(sbuf_len)
        ZigLightApp_SendPointToPointMessage(&Point_To_Point_DstAddr,&ZigLightApp_epDesc,sbuf_len,sbuf);
    }
    if(ZigLightApp_NwkState != DEV_ZB_COORD){
      if(main_state == HEARTBEAT){
        // Setup to send message again in normal period (+ a little jitter)
        osal_start_timerEx( ZigLightApp_TaskID, ZigLightApp_SEND_PERIODIC_MSG_EVT,
            (ZigLightApp_SEND_HEARTBEAT_MSG_TIMEOUT + (osal_rand() & 0x0FFF)) );
      }
      else{
        // Setup to send message again in normal period (+ a little jitter)
        osal_start_timerEx( ZigLightApp_TaskID, ZigLightApp_SEND_PERIODIC_MSG_EVT,
            (ZigLightApp_SEND_INIT_REQUEST_MSG_TIMEOUT + (osal_rand() & 0x0FFF)) );
      }
    }
    else{
        // Setup to send message again in normal period (+ a little jitter)
        osal_start_timerEx( ZigLightApp_TaskID, ZigLightApp_SEND_PERIODIC_MSG_EVT,
            (ZigLightApp_SEND_HEARTBEAT_MSG_TIMEOUT + (osal_rand() & 0x0FFF)) );
    }
    // return unprocessed events
    return (events ^ ZigLightApp_SEND_PERIODIC_MSG_EVT);
  }

  // Discard unknown events
  return 0;
}

/*********************************************************************
 * Event Generation Functions
 */
/*********************************************************************
 * @fn      ZigLightApp_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
void ZigLightApp_HandleKeys( uint8 shift, uint8 keys )
{
  (void)shift;  // Intentionally unreferenced parameter
  if ( keys & HAL_KEY_GIZWIT )
  {
    gizwitsSetMode(WIFI_AIRLINK_MODE);//airlink
//    HalLedBlink( HAL_LED_1, 20, 50, 1000 );
  }
  //by liu
//  if ( keys & HAL_KEY_SW_1 )
//  {
//    /* This key sends the Flash Command is sent to Group 1.
//     * This device will not receive the Flash Command from this
//     * device (even if it belongs to group 1).
//     */
//    ZigLightApp_SendFlashMessage( ZigLightApp_FLASH_DURATION );
//  }
//
//  if ( keys & HAL_KEY_SW_2 )
//  {
//    /* The Flashr Command is sent to Group 1.
//     * This key toggles this device in and out of group 1.
//     * If this device doesn't belong to group 1, this application
//     * will not receive the Flash command sent to group 1.
//     */
//    aps_Group_t *grp;
//    grp = aps_FindGroup( ZigLightApp_ENDPOINT, ZigLightApp_FLASH_GROUP );
//    if ( grp )
//    {
//      // Remove from the group
//      aps_RemoveGroup( ZigLightApp_ENDPOINT, ZigLightApp_FLASH_GROUP );
//    }
//    else
//    {
//      // Add to the flash group
//      aps_AddGroup( ZigLightApp_ENDPOINT, &ZigLightApp_Group );
//    }
//  }
}
/**********************************
* add by liu 
*************************/
void rfdata_handle( afIncomingMSGPacket_t *pkt )
{
  uint8 asc_16[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  uint8 i;
  uint16 temp;
  switch(pkt->cmd.Data[0]){
  case 0://接收到建立標示req
    {
      if(ZigLightApp_NwkState == DEV_ZB_COORD){
        HalUARTWrite(0,"\nR",2);
        HalUARTWrite(0,&asc_16[pkt->cmd.Data[1]],1);  
        HalUARTWrite(0,"S",1);
          /****将短地址分解，ASC码打印*****/
        temp=pkt->srcAddr.addr.shortAddr; //读出数据包的16位短地址
        HalUARTWrite(0,&asc_16[temp/4096],1);
        HalUARTWrite(0,&asc_16[temp%4096/256],1);
        HalUARTWrite(0,&asc_16[temp%256/16],1);
        HalUARTWrite(0,&asc_16[temp%16],1);  
        HalUARTWrite(0,"\n",1);
        main_state = ESTABLISH_CONNECTION_RSQ;//返回建立標示rsq
        sbuf_len = SendData_load(sbuf);
        if(sbuf_len){
          Point_To_Point_DstAddr.addr.shortAddr = pkt->srcAddr.addr.shortAddr;
          ZigLightApp_SendPointToPointMessage(&Point_To_Point_DstAddr,&ZigLightApp_epDesc,sbuf_len,sbuf);//返回建立標示rsq
        }
      }
      break;
    }
  case 1://接收到建立標示rsq
    {
      if(ZigLightApp_NwkState != DEV_ZB_COORD){
        if(pkt->cmd.Data[1] == 1){//成功加入協調器 開始發送心跳包
          main_state = HEARTBEAT;
        }
      }
      break;
    }
  case 2://接收到心跳包
    {
      //nothing to do 
      break;
    }
  case 4://接收到設備狀態改變信號
    {
      if(ZigLightApp_NwkState == DEV_ZB_COORD){
      // update to gizwit
        HalUARTWrite(0,"stat:",5);
        for(i=0;i<8;i++){
          HalUARTWrite(0,&asc_16[(currentMac[i])>>4],1);  
          HalUARTWrite(0,&asc_16[(currentMac[i])&0x0f],1); 
        }
        for(i=0;i<pkt->cmd.DataLength;i++){
          HalUARTWrite(0,&asc_16[(pkt->cmd.Data[i])>>4],1);  
          HalUARTWrite(0,&asc_16[(pkt->cmd.Data[i])&0x0f],1); 
        }
        HalUARTWrite(0,"\r\n",2);
      }
      break;
    }
  case 5:
    {
      if(ZigLightApp_NwkState != DEV_ZB_COORD){
      // 增加pwm控制程序
      //pwm_handle(on_off,w,r,g,b);

        on_off  = pkt->cmd.Data[1];
        white   = pkt->cmd.Data[2];
        red     = pkt->cmd.Data[3];
        green   = pkt->cmd.Data[4];
        blue    = pkt->cmd.Data[5];
        main_state = DEVICE_STATUS_CHANGE;
        sbuf_len = SendData_load(sbuf);
        if(sbuf_len)
          ZigLightApp_SendPointToPointMessage(&Point_To_Point_DstAddr,&ZigLightApp_epDesc,sbuf_len,sbuf);
        main_state = HEARTBEAT;
        //重新计时3秒
        osal_start_timerEx( ZigLightApp_TaskID, ZigLightApp_SEND_PERIODIC_MSG_EVT,
            (ZigLightApp_SEND_PERIODIC_MSG_TIMEOUT ) );
      }
      break;
    }
  default:
    {
      HalUARTWrite(0,"rf error\r\n",10);
    }
  }
//  if( (pkt->cmd.Data[0])==4 )
//  {
//    HalUARTWrite(0,"dstatus",7);
//    HalUARTWrite(0,&asc_16[pkt->cmd.Data[0]],1);  
//    HalUARTWrite(0,"\r\n",2);
//  }
}
/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      ZigLightApp_MessageMSGCB
 *
 * @brief   Data message processor callback.  This function processes
 *          any incoming data - probably from other devices.  So, based
 *          on cluster ID, perform the intended action.
 *
 * @param   none
 *
 * @return  none
 */
void ZigLightApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
//  uint16 flashTime;//temp;  
//  uint8 *ptr;//i;//res;
//  uint8 tp[10];
//  uint8 index;
  //网蜂16进制转ASCII码表
//    uint8 asc_16[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
                     
  switch ( pkt->clusterId )
  {
    case ZigLightApp_POINT_TO_POINT_CLUSTERID:  
      if(MacToAppFlag == 0 && AutoUpateMac == 0){//在读取mac时禁止zigbee操作
        mac_table_fresh( pkt );
        if(newdevice_joinin != 3)
          rfdata_handle( pkt );
      }
//      if(blink_finished == 0){
//        HalLedBlink( HAL_LED_1, 4, 50, 3000 );
//        blink_finished = 1;
//      }
//      temp=pkt->srcAddr.addr.shortAddr; //读出数据包的16位短地址
//      for(i=0;i<pkt->cmd.DataLength;i++)
//      {
//        HalUARTWrite(0,&asc_16[pkt->cmd.Data[i]%256/16],1);
//        HalUARTWrite(0,&asc_16[pkt->cmd.Data[i]%16],1);  
//      }
//      HalUARTWrite(0,"\r",1);
//      HalUARTWrite(0,"\n",1);               // 回车换行
//      if (   (ZigLightApp_NwkState == DEV_ZB_COORD) ){
//        if( ((pkt->cmd.Data[1])&0x03)==1 ) //路由器        
//          HalUARTWrite(0,"ROUTER ShortAddr:0x",19); //提示接收到数据
//        if( ((pkt->cmd.Data[1])&0x03)==2 ) //终端
//          HalUARTWrite(0,"ENDDEVICE ShortAddr:0x",22); //提示接收到数据
//          /****将短地址分解，ASC码打印*****/
//        HalUARTWrite(0,&asc_16[temp/4096],1);
//        HalUARTWrite(0,&asc_16[temp%4096/256],1);
//        HalUARTWrite(0,&asc_16[temp%256/16],1);
//        HalUARTWrite(0,&asc_16[temp%16],1);  
//        HalUARTWrite(0,"\r",1);
//        HalUARTWrite(0,"\n",1);               // 回车换行
//      }
      break;

//    case ZigLightApp_FLASH_CLUSTERID:
//      flashTime = BUILD_UINT16(pkt->cmd.Data[1], pkt->cmd.Data[2] );
//      HalLedBlink( HAL_LED_4, 4, 50, (flashTime / 4) );
//      break;
  }
}

/*********************************************************************
 * @fn      ZigLightApp_SendPeriodicMessage
 *
 * @brief   Send the periodic message.
 *
 * @param   none
 *
 * @return  none
 */
//by liu
//void ZigLightApp_SendPeriodicMessage( void )
//{
//  uint8 data[10]={'0','1','2','3','4','5','6','7','8','9'};//自定义数据
//  if ( AF_DataRequest( &ZigLightApp_Periodic_DstAddr, &ZigLightApp_epDesc,
//                       ZigLightApp_PERIODIC_CLUSTERID,
//                       10,//字节数
//                       data,//指针头
//                       &ZigLightApp_TransID,
//                       AF_DISCV_ROUTE,
//                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
//  {
//  }
//  else
//  {
//    // Error occurred in request to send.
//  }
//}

/*********************************************************************
 * @fn      ZigLightApp_SendFlashMessage
 *
 * @brief   Send the flash message to group 1.
 *
 * @param   flashTime - in milliseconds
 *
 * @return  none
 */
//by liu
//void ZigLightApp_SendFlashMessage( uint16 flashTime )
//{
//  uint8 buffer[3];
//  buffer[0] = (uint8)(ZigLightAppFlashCounter++);
//  buffer[1] = LO_UINT16( flashTime );
//  buffer[2] = HI_UINT16( flashTime );
//
//  if ( AF_DataRequest( &ZigLightApp_Flash_DstAddr, &ZigLightApp_epDesc,
//                       ZigLightApp_FLASH_CLUSTERID,
//                       3,
//                       buffer,
//                       &ZigLightApp_TransID,
//                       AF_DISCV_ROUTE,
//                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
//  {
//  }
//  else
//  {
//    // Error occurred in request to send.
//  }
//}

/*********************************************************************
ZigLightApp_SendPointToPointMessage(&Point_To_Point_DstAddr,&ZigLightApp_epDesc,1,&device);
*********************************************************************/
void ZigLightApp_SendPointToPointMessage( afAddrType_t * dstaddr , endPointDesc_t * epdesc , uint16 len , uint8 * buf  )
{
  
  
  if ( AF_DataRequest( dstaddr,  //发送设备类型编号
                       epdesc,
                       ZigLightApp_POINT_TO_POINT_CLUSTERID,
                       len,
                       buf,
                       &ZigLightApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
    ZigLightApp_TransID++;
  }
  else
  {
    // Error occurred in request to send.
  }

}
