/**
************************************************************
* @file         gizwits_protocol.h
* @brief        对应gizwits_protocol.c的头文件 (包含产品相关定义)
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

#ifndef _GIZWITS_PROTOCOL_H
#define _GIZWITS_PROTOCOL_H

//#include <stdint.h>
//#include <stdbool.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include "OSAL.h"
#include "gizwits_product.h"


/** WiFi模组配置参数*/
typedef enum
{
  WIFI_RESET_MODE = 0x00,                           ///< WIFI模组复位
  WIFI_SOFTAP_MODE,                                 ///< WIFI模组softAP模式
  WIFI_AIRLINK_MODE,                                ///< WIFI模组AirLink模式
  WIFI_PRODUCTION_TEST,                             ///< MCU请求WiFi模组进入产测模式
  WIFI_NINABLE_MODE                                 ///< MCU请求模组进入可绑定模�?
}WIFI_MODE_TYPE_T;                                  
                                          
#define min(a, b) (a)<(b)?(a):(b)                   ///< 获取最小�?                                                    
#define SEND_MAX_TIME       2000                    ///< 2000ms重发
#define SEND_MAX_NUM        2                       ///< 重发次数
                                                    
#define protocol_VERSION    "00000004"              ///< protocol版本�?
#define P0_VERSION          "00000002"              ///< P0协议版本�?
#define DEV_IS_GATEWAY   0                          ///< 设备是否为中控类,0表示�?1表示�?
#define NINABLETIME  0                              ///< 绑定时间
                            

/** 协议生成事件类型*/
typedef enum
{
  STATELESS_TYPE = 0x00,                            ///< 无状态类�?
  ACTION_CONTROL_TYPE,                              ///< 协议4.10 WiFi模组控制设备事件
  WIFI_STATUS_TYPE,                                 ///< 协议4.5 WiFi模组向设备MCU通知WiFi模组工作状态的变化事件
  ACTION_W2D_TRANSPARENT_TYPE,                      ///< 协议WiFi到设备MCU透传事件
  GET_NTP_TYPE,                                     ///< 协议4.13 MCU请求获取网络时间事件
  PROTOCOL_EVENT_TYPE_MAX                           ///< 枚举成员数量计算 (用户误删)
} PROTOCOL_EVENT_TYPE_T;
                                                                                       
                                                    
/** wifi信号强度数值结构体 */                       
typedef_t struct {                           
    uint8 rssi;                                   ///< WIFI信号强度
}moduleStatusInfo_t;                                

/** 协议网络时间结构�?*/
typedef_t struct
{
    uint16 year;
    uint8 month;
    uint8 day;
    uint8 hour;
    uint8 minute;
    uint8 second;
    uint32 ntp;
}protocolTime_t;

/** 协议命令�?*/                                   
typedef_t enum                               
{                                                   
    CMD_GET_DEVICE_INTO             = 0x01,         ///< 命令字，对应协议�?.1 WiFi模组请求设备信息 �?WiFi模组发�?
    ACK_GET_DEVICE_INFO             = 0x02,         ///< 命令字，对应协议�?.1 WiFi模组请求设备信息 �?设备MCU回复
            
    CMD_ISSUED_P0                   = 0x03,         ///< 命令字，对应协议�?.8 WiFi模组读取设备的当前状态�?.10 WiFi模组控制设备 �?WiFi模组发�?
    ACK_ISSUED_P0                   = 0x04,         ///< 命令字，对应协议�?.8 WiFi模组读取设备的当前状态�?.10 WiFi模组控制设备 �?设备MCU回复
            
    CMD_REPORT_P0                   = 0x05,         ///< 命令字，对应协议�?.9 设备MCU向WiFi模组主动上报当前状�?�?设备MCU发�?
    ACK_REPORT_P0                   = 0x06,         ///< 命令字，对应协议�?.9 设备MCU向WiFi模组主动上报当前状�?�?WiFi模组回复
            
    CMD_HEARTBEAT                   = 0x07,         ///< 命令字，对应协议�?.2 WiFi模组与设备MCU的心�?�?WiFi模组发�?
    ACK_HEARTBEAT                   = 0x08,         ///< 命令字，对应协议�?.2 WiFi模组与设备MCU的心�?�?设备MCU回复
            
    CMD_WIFI_CONFIG                 = 0x09,         ///< 命令字，对应协议�?.3 设备MCU通知WiFi模组进入配置模式 �?设备MCU发�?
    ACK_WIFI_CONFIG                 = 0x0A,         ///< 命令字，对应协议�?.3 设备MCU通知WiFi模组进入配置模式 �?WiFi模组回复
            
    CMD_SET_DEFAULT                 = 0x0B,         ///< 命令字，对应协议�?.4 设备MCU重置WiFi模组 �?设备MCU发�?
    ACK_SET_DEFAULT                 = 0x0C,         ///< 命令字，对应协议�?.4 设备MCU重置WiFi模组 �?WiFi模组回复
            
    CMD_WIFISTATUS                  = 0x0D,         ///< 命令字，对应协议�?.5 WiFi模组向设备MCU通知WiFi模组工作状态的变化 �?WiFi模组发�?
    ACK_WIFISTATUS                  = 0x0E,         ///< 命令字，对应协议�?.5 WiFi模组向设备MCU通知WiFi模组工作状态的变化 �?设备MCU回复
        
    CMD_MCU_REBOOT                  = 0x0F,         ///< 命令字，对应协议�?.6 WiFi模组请求重启MCU �?WiFi模组发�?
    ACK_MCU_REBOOT                  = 0x10,         ///< 命令字，对应协议�?.6 WiFi模组请求重启MCU �?设备MCU回复
            
    CMD_ERROR_PACKAGE               = 0x11,         ///< 命令字，对应协议�?.7 非法消息通知 �?WiFi模组回应MCU对应包序号的数据包非�?
    ACK_ERROR_PACKAGE               = 0x12,         ///< 命令字，对应协议�?.7 非法消息通知 �?MCU回应WiFi模组对应包序号的数据包非�?
    CMD_PRODUCTION_TEST             = 0x13,         ///< 命令字，对应协议�?.11 MCU请求WiFi模组进入产测模式 �?设备MCU发�?
    ACK_PRODUCTION_TEST             = 0x14,         ///< 命令字，对应协议�?.11 MCU请求WiFi模组进入产测模式 �?WiFi模组回复

    CMD_NINABLE_MODE                = 0x15,         ///< 命令字，对应协议�?.12 MCU通知WiFi模组进入可绑定模�?�?设备MCU发�?
    ACK_NINABLE_MODE                = 0x16,         ///< 命令字，对应协议�?.12 MCU通知WiFi模组进入可绑定模�?�?WiFi模组回复

    CMD_GET_NTP                     = 0x17,         ///< 命令字，对应协议�?.13 MCU请求获取网络时间 �?设备MCU发�?
    ACK_GET_NTP                     = 0x18,         ///< 命令字，对应协议�?.13 MCU请求获取网络时间 �?WiFi模组回复
} PROTOCOL_CMDTYPE;

/** 协议标准头结构体 */
typedef_t struct
{
    uint8                 head[2];                ///< 包头(header)固定�?xFFFF
    uint16                len;                    ///< 长度(len)是指从cmd 开始到整个数据包结束所占用的字节数
    uint8                 cmd;                    ///< 命令字（cmd）表示具体的命令含义，详见协议举�?
    uint8                 sn;                     ///< 消息序号(sn)由发送方给出,接收方响应命令时需把消息序号返回给发送方
    uint8                 flags[2];               ///< 标志位（flag），产品填写默认0
} protocolHead_t;

/** 4.1 WiFi模组请求设备信息 协议结构�?*/
typedef_t struct
{
    protocolHead_t          head;                   ///< 协议标准头结构体
    uint8                 protocolVer[8];         ///< 协议版本�?
    uint8                 p0Ver[8];               ///< p0协议版本�?
    uint8                 hardVer[8];             ///< 硬件版本�?
    uint8                 softVer[8];             ///< 软件版本�?
    uint8                 productKey[32];         ///< 产品标识�?
    uint16                ninableTime;            ///< 绑定超时(�?
    uint8                 devAttr[8];             ///< 设备属�?
    uint8                 productSecret[32];      ///< 产品密钥
    uint8                 sum;                    ///< 检验和
} protocolDeviceInfo_t;

/** 协议通用数据�?4.2�?.4�?.6�?.9�?.10) 协议结构�?*/
typedef_t struct
{
    protocolHead_t          head;                   ///< 协议标准头结构体
    uint8                 sum;                    ///< 检验和
} protocolCommon_t;

/** 4.3 设备MCU通知WiFi模组进入配置模式 协议结构�?*/
typedef_t struct
{
    protocolHead_t          head;                   ///< 协议标准头结构体
    uint8                 cfgMode;                ///< 配置参数
    uint8                 sum;                    ///< 检验和
} protocolCfgMode_t;

/** 4.13 MCU请求获取网络时间 协议结构�?*/
typedef_t struct
{
    protocolHead_t          head;                   ///< 协议标准头结构体
    uint8                 time[7];                ///< 硬件版本�?
    uint8                 ntp_time[4];            ///< 软件版本�?
    uint8                 sum;                    ///< 检验和
} protocolUTT_t;

/** WiFi模组工作状�?*/
typedef union
{
    uint16                value;
    struct
    {
        uint16            softap:1;               ///< 表示WiFi模组所处的SOFTAP模式状态，类型为bool
        uint16            station:1;              ///< 表示WiFi模组所处的STATION模式状态，类型为bool
        uint16            onboarding:1;           ///< 表示WiFi模组所处的配置状态，类型为bool
        uint16            binding:1;              ///< 表示WiFi模组所处的绑定状态，类型为bool
        uint16            con_route:1;            ///< 表示WiFi模组与路由器的连接状态，类型为bool
        uint16            con_m2m:1;              ///< 表示WiFi模组与云端m2m的状态，类型为bool
        uint16            reserve1:2;             ///< 数据位补�?
        uint16            rssi:3;                 ///< 表示路由的信号强度，类型为数�?
        uint16            app:1;                  ///< 表示WiFi模组与APP端的连接状态，类型为bool
        uint16            test:1;                 ///< 表示WiFi模组所处的场测模式状态，类型为bool
        uint16            reserve2:3;             ///< 数据位补�?    
    }types; 
    
} wifiStatus_t;

/** WiFi状态类�?协议结构�?*/
typedef struct
{
    protocolHead_t          head;                   ///< 协议标准头结构体
    wifiStatus_t            ststus;                 ///< WIFI状�? 
    uint8                 sum;                    ///< 检验和
} protocolWifiStatus_t;

/** 非法报文类型*/  
typedef_t enum
{
    ERROR_ACK_SUM = 0x01,                           ///< 校验错误
    ERROR_CMD     = 0x02,                           ///< 命令码错�? 
    ERROR_OTHER   = 0x03,                           ///< 其他
} errorPacketsType_t;

/** 4.7 非法消息通知 协议结构�?*/
typedef_t struct
{
    protocolHead_t          head;                   ///< 协议标准头结构体
    uint8                 error;                  ///< 错误�?
    uint8                 sum;                    ///< 检验和
} protocolErrorType_t;


/** P0报文标准�?*/
typedef_t struct
{
    protocolHead_t          head;                   ///< 协议标准头结构体
    uint8                 action;                 ///< p0 命令�?
} protocolP0Head_t;

/** 环形缓冲区数据结�?*/
typedef_t struct {
    uint16 rbCapacity;
    uint8  *rbHead;
    uint8  *rbTail;
    uint8  *rbBuff;
}rb_t;


/**@name Product Key (产品标识�?
* @{
*/
#define PRODUCT_KEY "9a2f863df7f54635b48401d832acd05a"
/**@} */

/**@name Product Secret (产品密钥)
* @{
*/
#define PRODUCT_SECRET "f06b9b5a07b74a12b2f424e60bca73e8"
/**@} */

#define MAX_PACKAGE_LEN    (sizeof(devStatus_t)+sizeof(dataPointFlags_t)+10)                ///< 数据缓冲区最大长�?
#define RB_MAX_LEN          (MAX_PACKAGE_LEN*2)     ///< 环形缓冲区最大长�?
/**@name 数据点相关定�?
* @{
*/


#define PAYLOAD_LEN                           20
/**@} */











/** 事件枚举*/
typedef enum
{
  WIFI_SOFTAP = 0x00,                               ///< WiFi SOFTAP 配置事件
  WIFI_AIRLINK,                                     ///< WiFi模块 AIRLINK 配置事件
  WIFI_STATION,                                     ///< WiFi模块 STATION 配置事件
  WIFI_OPEN_BINDING,                                ///< WiFi模块开启绑定事�? 
  WIFI_CLOSE_BINDING,                               ///< WiFi模块关闭绑定事件
  WIFI_CON_ROUTER,                                  ///< WiFi模块已连接路由事�?  
  WIFI_DISCON_ROUTER,                               ///< WiFi模块已断开连接路由事件
  WIFI_CON_M2M,                                     ///< WiFi模块已连服务器M2M事件
  WIFI_DISCON_M2M,                                  ///< WiFi模块已断开连服务器M2M事件
  WIFI_OPEN_TESTMODE,                               ///< WiFi模块开启测试模式事�? 
  WIFI_CLOSE_TESTMODE,                              ///< WiFi模块关闭测试模式事件
  WIFI_CON_APP,                                     ///< WiFi模块连接APP事件
  WIFI_DISCON_APP,                                  ///< WiFi模块断开APP事件
  WIFI_RSSI,                                        ///< WiFi模块信号事件
  WIFI_NTP,                                         ///< 网络时间事件
  TRANSPARENT_DATA,                                 ///< 透传事件
  EVENT_PAYLOAD,
  EVENT_TYPE_MAX                                    ///< 枚举成员数量计算 (用户误删)
} EVENT_TYPE_T;


/** 用户区设备状态结构体*/
typedef_t struct {
  uint8 valuepayload[20];
} dataPoint_t;

/** 用户区设备状态结构体*/
typedef_t struct {





  uint8 flagpayload:1;
} dataPointFlags_t;

#define DATAPOINT_FLAG_LEN sizeof(dataPointFlags_t)//所有数据点FLAG占用的最大字节数

/** 对应协议�?.9 设备MCU向WiFi模组主动上报当前状态”中的设备状�?dev_status(11B)" */ 
typedef_t struct {




  uint8 valuepayload[20];
} devStatus_t;                                      

/** P0 command 命令�?*/
typedef enum
{
    ACTION_CONTROL_DEVICE       = 0x11,             ///< 协议 WiFi模组控制设备 WiFi模组发�? 
    ACTION_READ_DEV_STATUS      = 0x12,             ///< 协议 WiFi模组读取设备的当前状�?WiFi模组发�? 
    ACTION_READ_DEV_STATUS_ACK  = 0x13,             ///< 协议 WiFi模组读取设备的当前状�?设备MCU回复
    ACTION_REPORT_DEV_STATUS    = 0X14,             ///< 协议 设备MCU向WiFi模组主动上报当前状�?设备MCU发�?  
    ACTION_W2D_TRANSPARENT_DATA = 0x05,             ///< WiFi到设备MCU透传
    ACTION_D2W_TRANSPARENT_DATA = 0x06,             ///< 设备MCU到WiFi透传
} actionType_t;

typedef_t struct {                           
    dataPointFlags_t  devDatapointFlag;             ///< 数据点占位Flag
    devStatus_t devStatus;                          ///< 设备全数据点状态数�?
}gizwitsElongateP0Form_t;  

typedef_t struct {                           
    uint8 action;
    uint8 data[sizeof(gizwitsElongateP0Form_t)];
}gizwitsP0Max_t;                                  
                                                    
/** 事件队列结构�?*/                               
typedef_t struct {                           
    uint8 num;                                    ///< 队列成员个数
    uint8 event[EVENT_TYPE_MAX];                  ///< 队列成员事件内容
}eventInfo_t;
 

/** 对应协议�?.9 设备MCU向WiFi模组主动上报当前状态�?中的 设备状态位"dev_status(11B)"  */ 
typedef_t struct {                           
    devStatus_t devStatus;                          ///< 作用：用来存放待上报的设备状态数�?
}gizwitsReport_t;
/** 重发机制结构�?*/
typedef_t struct {
    uint8                 num;                    ///< 重发次数
    uint8                 flag;                   ///< 1,表示有需要等待的ACK;0,表示无需要等待的ACK
    uint8                 buf[MAX_PACKAGE_LEN];   ///< 重发数据缓冲�? 
    uint16                dataLen;                ///< 重发数据长度
    uint32                sendTime;               ///< 重发的系统时�?
} protocolWaitAck_t;

/** 协议处理总结构体 */
typedef_t struct
{
    uint8 issuedFlag;                             ///< 保存对应�?p0命令类型
    uint8 protocolBuf[MAX_PACKAGE_LEN];           ///< 协议报文存储�?  
    uint8 transparentBuff[MAX_PACKAGE_LEN];       ///< 透传数据存储�?  
    uint32 transparentLen;                        ///< 透传数据长度
    
    uint32 sn;                                    ///< 消息序号
    uint32 timerMsCount;                          ///< 时间计数(Ms)
    protocolWaitAck_t waitAck;                      ///< 协议ACK数据结构
    
    eventInfo_t issuedProcessEvent;                 ///< 控制事件
    eventInfo_t wifiStatusEvent;                    ///< WIFI状�?事件
    eventInfo_t NTPEvent;                           ///< NTP 事件

    dataPointFlags_t  waitReportDatapointFlag;      ///< 存放待上报的数据点FLAG
    uint8 reportData[sizeof(gizwitsElongateP0Form_t)];    ///< 协议上报实际数据
    uint32 reportDataLen;                         ///< 协议上报实际数据长度

    dataPoint_t gizCurrentDataPoint;                ///< 当前设备状态数�?  
    dataPoint_t gizLastDataPoint;                   ///< 上次上报的设备状态数�? 
    moduleStatusInfo_t wifiStatusData;              ///< WIFI 状态信�?信号强度)
    protocolTime_t TimeNTP;                         ///< 网络时间信息
}gizwitsProtocol_t;

/**@name Gizwits 用户API接口
* @{
*/
void gizwitsInit(void);
uint32 gizwitsSetMode(uint8 mode);
void gizwitsGetNTP(void);
int32 gizwitsHandle(dataPoint_t *currentData);
int32 gizwitsPassthroughData(uint8 * data, uint32 len);
/**@} */

int32 gizPutData(uint8 *buf, uint32 len);

#endif
