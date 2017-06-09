/**
************************************************************
* @file         gizwits_product.h
* @brief        对应gizwits_product.c的头文件(包含产品软硬件版本定�?
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
#ifndef _GIZWITS_PRODUCT_H
#define _GIZWITS_PRODUCT_H

//#include <stdint.h>
#include "OSAL.h"
#include "gizwits_protocol.h"

/**
* @name 日志打印宏定�?* @{
*/
#define printf(...) 
#define GIZWITS_LOG printf                          ///<运行日志打印
//#define PROTOCOL_DEBUG                              ///<协议数据打印

#ifndef ICACHE_FLASH_ATTR
#define ICACHE_FLASH_ATTR
#endif

/**@} */
#define typedef_t typedef

/**
* MCU软件版本�?*/
#define SOFTWARE_VERSION "03010101"
/**
* MCU硬件版本�?*/
#define HARDWARE_VERSION "03010100"

#ifndef SOFTWARE_VERSION
    #error "no define SOFTWARE_VERSION"
#endif

#ifndef HARDWARE_VERSION
    #error "no define HARDWARE_VERSION"
#endif

/**@name 设备状态数据上报间隔时间宏定义
* @{
*/
#define REPORT_TIME_MAX 6000 //6S
/**@} */

void mcuRestart(void);
uint32 uartWrite(uint8 *buf, uint32 len);

/**@name Gizwits 用户API接口
* @{
*/
//int8 gizwitsEventProcess(eventInfo_t *info, uint8 *data, uint32 len);
/**@} */

#endif
