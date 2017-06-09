/**
************************************************************
* @file         gizwits_protocol.c
* @brief        Gizwits协议相关文件 (SDK API 接口函数定义)
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
#include "gizwits_protocol.h"
#include "List1.h"
#include "mt_uart.h"
#include "hal_led.h"
extern uint8 MacToAppFlag ;//by liu
extern uint8 AutoUpateMac ;//by liu
extern uint8 NewMacUpate;//by liu
extern Node *nodelist;// by liu
extern dataPoint_t currentDataPoint;// by liu
extern Node *deletnodelist;// by liu
extern uint32 gizGetTimerCount(void);
extern int8 gizwitsEventProcess(eventInfo_t *info, uint8 *data, uint32 len);
uint32 ICACHE_FLASH_ATTR calculateDynamicBitPartLen(dataPointFlags_t *aFlag);
/** 协议全局变量 **/
gizwitsProtocol_t gizwitsProtocol;

/**@name 串口接收环形缓冲区实�?* @{
*/
rb_t pRb;                                               ///< 环形缓冲区结构体变量
static uint8 rbBuf[RB_MAX_LEN];                       ///< 环形缓冲区数据缓存区

static void rbCreate(rb_t* rb)
{
    if(NULL == rb)
    {
        GIZWITS_LOG("ERROR: input rb is NULL\n");
        return;
    }

    rb->rbHead = rb->rbBuff;
    rb->rbTail = rb->rbBuff;
}

static void rbDelete(rb_t* rb)
{
    if(NULL == rb)
    {
        GIZWITS_LOG("ERROR: input rb is NULL\n");
        return;
    }

    rb->rbBuff = NULL;
    rb->rbHead = NULL;
    rb->rbTail = NULL;
    rb->rbCapacity = 0;
}
#pragma optimize=none
static int32 rbCapacity(rb_t *rb)
{
    if(NULL == rb)
    {
        GIZWITS_LOG("ERROR: input rb is NULL\n");
        return -1;
    }

    return rb->rbCapacity;
}
#pragma optimize=none
static int32 rbCanRead(rb_t *rb)
{
    if(NULL == rb)
    {
        GIZWITS_LOG("ERROR: input rb is NULL\n");
        return -1;
    }

    if (rb->rbHead == rb->rbTail)
    {
        return 0;
    }

    if (rb->rbHead < rb->rbTail)
    {
        return rb->rbTail - rb->rbHead;
    }

    return rbCapacity(rb) - (rb->rbHead - rb->rbTail);
}
#pragma optimize=none
static int32 rbCanWrite(rb_t *rb)
{
    if(NULL == rb)
    {
        GIZWITS_LOG("ERROR: input rb is NULL\n");
        return -1;
    }

    return rbCapacity(rb) - rbCanRead(rb);
}

static int32 rbRead(rb_t *rb, void *data, uint16 count)
{
    int copySz = 0;

    if(NULL == rb)
    {
        GIZWITS_LOG("ERROR: input rb is NULL\n");
        return -1;
    }

    if(NULL == data)
    {
        GIZWITS_LOG("ERROR: input data is NULL\n");
        return -1;
    }

    if (rb->rbHead < rb->rbTail)
    {
        copySz = min(count, rbCanRead(rb));
        osal_memcpy(data, rb->rbHead, copySz);
        rb->rbHead += copySz;
        return copySz;
    }
    else
    {
        if (count < rbCapacity(rb)-(rb->rbHead - rb->rbBuff))
        {
            copySz = count;
            osal_memcpy(data, rb->rbHead, copySz);
            rb->rbHead += copySz;
            return copySz;
        }
        else
        {
            copySz = rbCapacity(rb) - (rb->rbHead - rb->rbBuff);
            osal_memcpy(data, rb->rbHead, copySz);
            rb->rbHead = rb->rbBuff;
            copySz += rbRead(rb, (char*)data+copySz, count-copySz);
            return copySz;
        }
    }
}
#pragma optimize=none
static int32 rbWrite(rb_t *rb, const void *data, uint16 count)
{
    int tailAvailSz = 0;

    if(NULL == rb)
    {
        GIZWITS_LOG("ERROR: rb is empty \n");
        return -1;
    }

    if(NULL == data)
    {
        GIZWITS_LOG("ERROR: data is empty \n");
        return -1;
    }

    if (count >= rbCanWrite(rb))
    {
        GIZWITS_LOG("ERROR: no memory %d \n", rbCanWrite(rb));
        return -1;
    }

    if (rb->rbHead <= rb->rbTail)
    {
        tailAvailSz = rbCapacity(rb) - (rb->rbTail - rb->rbBuff);
        if (count <= tailAvailSz)
        {
            osal_memcpy(rb->rbTail, data, count);
            rb->rbTail += count;
            if (rb->rbTail == rb->rbBuff+rbCapacity(rb))
            {
                rb->rbTail = rb->rbBuff;
            }
            return count;
        }
        else
        {
            osal_memcpy(rb->rbTail, data, tailAvailSz);
            rb->rbTail = rb->rbBuff;

            return tailAvailSz + rbWrite(rb, (char*)data+tailAvailSz, count-tailAvailSz);
        }
    }
    else
    {
        osal_memcpy(rb->rbTail, data, count);
        rb->rbTail += count;
        return count;
    }
}
/**@} */

/**
* @brief 向环形缓冲区写入数据
* @param [in] buf        : buf地址
* @param [in] len        : 字节长度
* @return   正确 : 返回写入的数据长�?            失败 : -1
*/
int32 gizPutData(uint8 *buf, uint32 len)
{
    int32 count = 0;

    if(NULL == buf)
    {
        GIZWITS_LOG("ERROR: gizPutData buf is empty \n");
        return -1;
    }

    count = rbWrite(&pRb, buf, len);
    if(count != len)
    {
        GIZWITS_LOG("ERROR: Failed to rbWrite \n");
        return -1;
    }

    return count;
}

/**
* @brief 报文数据校验和计�?*
* @param [in] buf   : buf地址
* @param [in] len   : 字节长度
*
* @return sum : 从缓冲区�?个字节后所有的字节累加求和
*/
static uint8 gizProtocolSum(uint8 *buf, uint32 len)
{
    uint8     sum = 0;
    uint32    i = 0;

    if(buf == NULL || len <= 0)
    {
        GIZWITS_LOG("ERROR: gizProtocolSum , buf is empty or len error %d \n", len);
        return 0;
    }

    for(i=2; i<len-1; i++)
    {
        sum += buf[i];
    }

    return sum;
}

/**
* @brief 16位数据字节序转换
*
* @param [in] value : 需要转换的数据
*
* @return  tmp_value: 转换后的数据
*/
static uint16 gizProtocolExchangeBytes(uint16 value)
{
    uint16    tmp_value;
    uint8     *index_1, *index_2;

    index_1 = (uint8 *)&tmp_value;
    index_2 = (uint8 *)&value;

    *index_1 = *(index_2+1);
    *(index_1+1) = *index_2;

    return tmp_value;
}

/**
* @brief 32位数据字节序转换
*
* @param [in] value : 需要转换的数据
*
* @return  tmp_value: 转换后的数据
*/
static uint32 gizExchangeWord(uint32  value)
{
    return ((value & 0x000000FF) << 24) |
        ((value & 0x0000FF00) << 8) |
        ((value & 0x00FF0000) >> 8) |
        ((value & 0xFF000000) >> 24) ;
}

/**
* @brief 数组缓冲区网络字节序转换
*
* @param [in] buf      : buf地址
* @param [in] dataLen  : 字节长度
*
* @return 正确 : 0 
          失败 : -1
*/
static int8 gizByteOrderExchange(uint8 *buf,uint32 dataLen)
{
    uint32 i = 0;
    uint8 preData = 0;
    uint8 aftData = 0;

    if(NULL == buf)
    {
        GIZWITS_LOG("gizByteOrderExchange Error , Illegal Param\n");
        return -1;
    }

    for(i = 0;i<dataLen/2;i++)
    {
        preData = buf[i];
        aftData = buf[dataLen - i - 1];
        buf[i] = aftData;
        buf[dataLen - i - 1] = preData;
    }
    return 0;
}

/**
* @brief 转化为协议中的x值及实际通讯传输的�?*
* @param [in] ratio    : 修正系数k
* @param [in] addition : 增量m
* @param [in] preValue : 作为协议中的y�? 是App UI界面的显示�?*
* @return aft_value    : 作为协议中的x�? 是实际通讯传输的�?*/
static uint32 gizY2X(uint32 ratio, int32 addition, int32 preValue)
{
    uint32 aftValue = 0;

    //x=(y - m)/k
    aftValue = ((preValue - addition) / ratio);

    return aftValue;
}

/**
* @brief 转化为协议中的y值及App UI界面的显示�?*
* @param [in] ratio    : 修正系数k
* @param [in] addition : 增量m
* @param [in] preValue : 作为协议中的x�? 是实际通讯传输的�?*
* @return aftValue : 作为协议中的y�? 是App UI界面的显示�?*/
static int32 gizX2Y(uint32 ratio, int32 addition, uint32 preValue)
{
    int32 aftValue = 0;

    //y=k * x + m
    aftValue = (preValue * ratio + addition);

    return aftValue;
}

/**
* @brief 转化为协议中的x值及实际通讯传输的�?只用作对浮点型数据做处理
*
* @param [in] ratio    : 修正系数k
* @param [in] addition : 增量m
* @param [in] preValue : 作为协议中的y�? 是App UI界面的显示�?*
* @return  aft_value : 作为协议中的x�? 是实际通讯传输的�?*/
static uint32 gizY2XFloat(float ratio, float addition, float preValue)
{
    uint32 aftValue = 0;

    //x=(y - m)/k
    aftValue = ((preValue - addition) / ratio);

    return aftValue;
}

/**
* @brief 转化为协议中的y值及App UI界面的显示�?只用作对浮点型数据做处理
*
* @param [in] ratio    : 修正系数k
* @param [in] addition : 增量m
* @param [in] preValue : 作为协议中的x�? 是实际通讯传输的�?*
* @return : 作为协议中的y�? 是App UI界面的显示�?*/
static float gizX2YFloat(float ratio, float addition, uint32 preValue)
{
    float aftValue = 0;

    //y=k * x + m
    aftValue = (preValue * ratio + addition);

    return aftValue;
}

/**
* @brief 数据点跨字节判断
*
* @param [in] bitOffset     : 位偏�?* @param [in] bitLen        : 占用位长�?
*
* @return 未跨字节 : 0 
            跨字�?: 1
*/
static uint8 gizAcrossByteJudge(uint32 bitOffset,uint32 bitLen)
{
    if((0 == bitOffset)||(0 == bitOffset%8))
    {
        if(bitLen <= 8)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        if(8 - bitOffset%8 >= bitLen)
        {
            return 0;
        }
        else
        {
            return 1; 
        }
    }
}
/**
* @brief 协议头初始化
*
* @param [out] head         : 协议头指�?*
* @return 0�?执行成功�?�?0�?失败    
*/
static int8 gizProtocolHeadInit(protocolHead_t *head)
{
    if(NULL == head)
    {
        GIZWITS_LOG("ERROR: gizProtocolHeadInit head is empty \n");
        return -1;
    }

    osal_memset((uint8 *)head, 0, sizeof(protocolHead_t));
    head->head[0] = 0xFF;
    head->head[1] = 0xFF;

    return 0;
}

/**
* @brief 协议ACK校验处理函数
*
* @param [in] data            : 数据地址
* @param [in] len             : 数据长度
*
* @return 0�?执行成功�?�?0�?失败
*/
static int8 gizProtocolWaitAck(uint8 *data, uint32 len)
{
    if(NULL == data)
    {
        GIZWITS_LOG("ERROR: data is empty \n");
        return -1;
    }

    osal_memset((uint8 *)&gizwitsProtocol.waitAck, 0, sizeof(protocolWaitAck_t));
    osal_memcpy((uint8 *)gizwitsProtocol.waitAck.buf, data, len);
    gizwitsProtocol.waitAck.dataLen = (uint16)len;
    
    gizwitsProtocol.waitAck.flag = 1;
    gizwitsProtocol.waitAck.sendTime = gizGetTimerCount();

    return 0;
}



uint32 ICACHE_FLASH_ATTR calculateDynamicBitPartLen(dataPointFlags_t *aFlag)
{
    uint32 bitFieldBitLen = 0,bytetmpLen= 0;
    /* 只处理可写bool Enum类型数据 */

    if(0 == bitFieldBitLen)
    {
        bytetmpLen = 0;
    }
    else
    {
        if(0 == bitFieldBitLen%8)
        {
            bytetmpLen = bitFieldBitLen/8;
        }
        else
        {
            bytetmpLen = bitFieldBitLen/8 + 1;
        }
    }
    return bytetmpLen;
}
static int32 ICACHE_FLASH_ATTR gizCompressValue(uint32 bitOffset,uint32 bitLen,uint8 *bufAddr,uint32 srcData)
{
    uint8 highBit = 0;
    uint8 lowBit = 0;
    uint8 ret = 0;
    uint32 byteOffset = 0;
    if(NULL == bufAddr)
    {
        GIZWITS_LOG("gizCompressValue Error , Illegal Param\n");
        return -1;
    }       
    /* 根据bitOffset计算 byteOffset*/
    byteOffset = bitOffset/8;
    ret = gizAcrossByteJudge(bitOffset,bitLen);
    if(0 == ret)
    {
        bufAddr[byteOffset] |= (((uint8)srcData)<<(bitOffset%8));
    }
    else if(1 == ret)
    {
        /* 暂时支持最多跨两字节的压缩 */ 
        highBit = ((uint8)srcData)>>(8-bitOffset%8);
        lowBit = (uint8)srcData & (0xFF >> (8-bitOffset%8));
        bufAddr[byteOffset + 1] |=  highBit;
        bufAddr[byteOffset] |= (lowBit<<(bitOffset%8));
    }

    return 0;
}
/**
* @brief bool和enum类型数据点数据解�?*
* @param [in] bitOffset     : 位偏�?* @param [in] bitLen        : 占用位长�?* @param [in] arrayAddr     : 数组地址
* @param [in] arrayLen      : 数组长度
*
* @return destValue         : 解压后结果数�?-1表示错误返回
*/
static int32 ICACHE_FLASH_ATTR gizDecompressionValue(uint32 bitOffset,uint32 bitLen,uint8 *arrayAddr,uint32 arrayLen)
{
    int32 destValue = 0;
    return destValue;
}
/**
* @brief 根据协议生成“控制型事件�?
* @param [in] issuedData  ：控制型数据
* @param [out] info       ：事件队�?* @param [out] dataPoints ：数据点数据
* @return 0，执行成功， �?0，失�?*/
static int8 ICACHE_FLASH_ATTR gizDataPoint2Event(uint8 *issuedData, eventInfo_t *info, dataPoint_t *dataPoints)
{
    uint32 bitFieldByteLen= 0;//位段字节长度
    uint32 bitFieldOffset = 0;//位段位偏�?
    uint32 byteFieldOffset = 0;//字节段偏�?
    gizwitsElongateP0Form_t elongateP0FormTmp;
    osal_memset((uint8 *)&elongateP0FormTmp,0,sizeof(gizwitsElongateP0Form_t));
    osal_memcpy((uint8 *)&elongateP0FormTmp.devDatapointFlag,issuedData,DATAPOINT_FLAG_LEN);

    if((NULL == issuedData) || (NULL == info) ||(NULL == dataPoints))
    {
        GIZWITS_LOG("gizDataPoint2Event Error , Illegal Param\n");
        return -1;
    }
    /** 大于1字节做位序转�?**/
    if(DATAPOINT_FLAG_LEN > 1)
    {
        if(-1 == gizByteOrderExchange((uint8 *)&elongateP0FormTmp.devDatapointFlag,DATAPOINT_FLAG_LEN))
        {
            GIZWITS_LOG("gizByteOrderExchange Error\n");
            return -1;
        }
    }
    /* 计算位段占用的字节长�?*/
    bitFieldByteLen = calculateDynamicBitPartLen(&elongateP0FormTmp.devDatapointFlag);
    byteFieldOffset += bitFieldByteLen + DATAPOINT_FLAG_LEN;//数值段字节偏移




    if(0x01 == elongateP0FormTmp.devDatapointFlag.flagpayload)
    {
        info->event[info->num] = EVENT_PAYLOAD;
        info->num++;
        osal_memcpy((uint8 *)dataPoints->valuepayload,(uint8 *)&issuedData[byteFieldOffset],20);
        byteFieldOffset += PAYLOAD_LEN;
    }

    return 0;
}


/**
* @brief 对比当前数据和上次数�?*
* @param [in] cur        : 当前数据点数�?* @param [in] last       : 上次数据点数�?*
* @return : 0,数据无变�?1，数据有变化
*/
#pragma optimize=none
static int8 ICACHE_FLASH_ATTR gizCheckReport(dataPoint_t *cur, dataPoint_t *last)
{
    int8 ret = 0;
    static uint32 lastReportTime = 0;

    if((NULL == cur) || (NULL == last))
    {
        GIZWITS_LOG("gizCheckReport Error , Illegal Param\n");
        return -1;
    }
    if(TRUE != osal_memcmp((uint8 *)&last->valuepayload,(uint8 *)&cur->valuepayload,sizeof(last->valuepayload)))
    {
        GIZWITS_LOG("valuepayload Changed\n");
        gizwitsProtocol.waitReportDatapointFlag.flagpayload = 1;
        ret = 1;
    }


    return ret;
}

/**
* @brief 用户数据点数据转换为机智云上报数据点数据
*
* @param [in]  dataPoints           : 用户数据点数据地址
* @param [out] devStatusPtr         : 机智云上报数据点数据地址
*
* @return 0,正确返回;-1，错误返�?*/
static int8 ICACHE_FLASH_ATTR gizDataPoints2ReportData(dataPoint_t *dataPoints , uint8 *outData,uint32 *outDataLen)
{
    uint32 bitFieldByteLen= 0;//位段字节大小
    uint32 bitFieldOffset = 0;//位偏�?  
    uint32 byteFieldOffset = 0;//字节偏移

    uint8 allDatapointByteBuf[sizeof(gizwitsElongateP0Form_t)];//开辟最大数据点空间
    osal_memset(allDatapointByteBuf,0,sizeof(gizwitsElongateP0Form_t));

    osal_memcpy(allDatapointByteBuf,(uint8 *)&gizwitsProtocol.waitReportDatapointFlag,DATAPOINT_FLAG_LEN);
    if(DATAPOINT_FLAG_LEN > 1)
    {
        gizByteOrderExchange(allDatapointByteBuf,DATAPOINT_FLAG_LEN);
    }
    byteFieldOffset += DATAPOINT_FLAG_LEN;//先偏移出Flag大小的位�?        
    devStatus_t devStatusTmp;//临时设备数据点变�?    
    if((NULL == dataPoints) || (NULL == outData))
    {
        GIZWITS_LOG("gizDataPoints2ReportData Error , Illegal Param\n");
        return -1;
    }

    /*** 填充位段 ***/

    /* 待bit段拼装结�?计算数值段的偏�?*/
    if(0 == bitFieldOffset)
    {
        bitFieldByteLen = 0;
    }
    else
    {
        if(0 == bitFieldOffset%8)
        {
            bitFieldByteLen = bitFieldOffset/8;
        }
        else
        {
            bitFieldByteLen = bitFieldOffset/8 + 1;
        }
    }
    /** 位段字节序转�?**/
    if(bitFieldByteLen > 1)
    {
        gizByteOrderExchange((uint8 *)&allDatapointByteBuf[byteFieldOffset],bitFieldByteLen);
    }
    
    byteFieldOffset += bitFieldByteLen;//偏移出位段占用字节数

    /*** 处理数值段 ***/





    if(gizwitsProtocol.waitReportDatapointFlag.flagpayload)
    {
        osal_memcpy((uint8 *)&allDatapointByteBuf[byteFieldOffset],(uint8 *)dataPoints->valuepayload,sizeof(dataPoints->valuepayload));
        byteFieldOffset += sizeof(dataPoints->valuepayload);
    }

    osal_memset((uint8 *)&gizwitsProtocol.waitReportDatapointFlag,0,DATAPOINT_FLAG_LEN);//清空标志�?  
    *outDataLen = byteFieldOffset;
    osal_memcpy(outData,allDatapointByteBuf,*outDataLen);
    return 0;
}

/**@} */


/**
* @brief 接收协议报文并进行相应的协议处理

* Wifi模组接收来自云端或APP端下发的相关协议数据发送到MCU端，经过协议报文解析后将相关协议数据传入次函数，进行下一步的协议处理

* @param[in] inData   : 输入的协议数�?* @param[in] inLen    : 输入数据的长�?* @param[out] outData : 输出的协议数�?* @param[out] outLen  : 输出数据的长�?* @return 0�?执行成功�?�?0�?失败
*/
static int8 gizProtocolIssuedProcess(uint8 *inData, uint32 inLen, uint8 *outData, uint32 *outLen)
{
    protocolP0Head_t *protocolIssuedData = (protocolP0Head_t *)inData;
    uint8 issuedAction = 0;
    issuedAction = protocolIssuedData->action;

    if((NULL == inData)||(NULL == outData)||(NULL == outLen))
    {
        GIZWITS_LOG("gizProtocolIssuedProcess Error , Illegal Param\n");
        return -1;
    }
    
    osal_memset((uint8 *)&gizwitsProtocol.issuedProcessEvent, 0, sizeof(eventInfo_t));
    switch(issuedAction)
    {
        case ACTION_CONTROL_DEVICE:
            GIZWITS_LOG("ACTION_ELONGATE_CONTROL_DEVICE \n");
            gizDataPoint2Event(inData+sizeof(protocolP0Head_t), &gizwitsProtocol.issuedProcessEvent,&gizwitsProtocol.gizCurrentDataPoint);
            gizwitsProtocol.issuedFlag = ACTION_CONTROL_TYPE;
            outData = NULL;
            *outLen = 0;
            break;
        
        case ACTION_READ_DEV_STATUS:
            GIZWITS_LOG("ACTION_ELONGATE_READ_DEV_STATUS \n");          
            osal_memcpy((uint8 *)&gizwitsProtocol.waitReportDatapointFlag,inData+sizeof(protocolP0Head_t),DATAPOINT_FLAG_LEN);//拷贝查询FLAG               
            if(DATAPOINT_FLAG_LEN > 1)
            {
                    gizByteOrderExchange((uint8 *)&gizwitsProtocol.waitReportDatapointFlag,DATAPOINT_FLAG_LEN);
            }
                        
            if(0 == gizDataPoints2ReportData(&gizwitsProtocol.gizLastDataPoint,gizwitsProtocol.reportData,(uint32 *)&gizwitsProtocol.reportDataLen))
            {  
                osal_memcpy(outData, (uint8 *)&gizwitsProtocol.reportData, gizwitsProtocol.reportDataLen);
                *outLen = gizwitsProtocol.reportDataLen;
            }
            else
            {
                return -1;
            }
            break;
        case ACTION_W2D_TRANSPARENT_DATA:
            osal_memcpy(gizwitsProtocol.transparentBuff, inData+sizeof(protocolP0Head_t), inLen-sizeof(protocolP0Head_t)-1);
            gizwitsProtocol.transparentLen = inLen - sizeof(protocolP0Head_t) - 1;
            gizwitsProtocol.issuedProcessEvent.event[gizwitsProtocol.issuedProcessEvent.num] = TRANSPARENT_DATA;
            gizwitsProtocol.issuedProcessEvent.num++;
            gizwitsProtocol.issuedFlag = ACTION_W2D_TRANSPARENT_TYPE;
            outData = NULL;
            *outLen = 0;
            break;
        
        default:
            break;
    }

    return 0;
}
/**
* @brief 协议下发数据返回ACK
*
* @param [in] head                  : 协议头指�?* @param [in] data                  : 数据地址
* @param [in] len                   : 数据长度
*
* @return : 有效数据长度,正确返回;-1，错误返�?*/
static int32 gizProtocolIssuedDataAck(protocolHead_t *head, uint8 *data, uint32 len)
{
    int32 ret = 0;
    uint16 dataLen = 0;

    uint8 tx_buf[MAX_PACKAGE_LEN];
    uint8 *pTxBuf = tx_buf;

    if((NULL == head)||(NULL == data))
    {
        GIZWITS_LOG("gizProtocolIssuedDataAck Error , Illegal Param\n");
        return -1;
    }
    
    if(0 == len)
    {
        dataLen =  5;
        *pTxBuf ++= 0xFF;
        *pTxBuf ++= 0xFF;
        *pTxBuf ++= (uint8)(dataLen>>8);//len
        *pTxBuf ++= (uint8)(dataLen);
        *pTxBuf ++= head->cmd+1;
        *pTxBuf ++= head->sn;//sn
        *pTxBuf ++= 0x00;//flag
        *pTxBuf ++= 0x00;//flag
    }
    else
    {
        dataLen = 6 + len;
        *pTxBuf ++= 0xFF;
        *pTxBuf ++= 0xFF;
        *pTxBuf ++= (uint8)(dataLen>>8);//len
        *pTxBuf ++= (uint8)(dataLen);
        *pTxBuf ++= head->cmd+1;//0x1b cmd
        *pTxBuf ++= head->sn;//sn
        *pTxBuf ++= 0x00;//flag
        *pTxBuf ++= 0x00;//flag
        *pTxBuf ++= ACTION_READ_DEV_STATUS_ACK;//P0_Cmd
        /* 拷贝数据点数�?*/
        osal_memcpy(&tx_buf[9],data,len);
    }
    tx_buf[dataLen + 4 - 1 ] = gizProtocolSum( tx_buf , (dataLen+4));

    ret = uartWrite(tx_buf, dataLen+4);
    if(ret < 0)
    {
        //打印日志
        GIZWITS_LOG("ERROR: uart write error %d \n", ret);
    }
    return ret;
}

/**
* @brief 上报数据
*
* @param [in] action            : PO cmd
* @param [in] data              : 数据地址
* @param [in] len               : 数据长度
*
* @return : 正确返回有效数据长度;-1，错误返�?*/
#pragma optimize=none
static int32 gizReportData(uint8 action, uint8 *data, uint32 len)
{
    int32 ret = 0;
    uint8 tx_buf[MAX_PACKAGE_LEN];
    if(NULL == data)
    {
        GIZWITS_LOG("[Error] gizwitsPassthroughData Error \n");
        return (-1);
    }
    uint8 *pTxBuf = tx_buf;
    uint16 data_len = 6 + len;
    *pTxBuf ++= 0xFF;
    *pTxBuf ++= 0xFF;
    *pTxBuf ++= (uint8)(data_len>>8);//len
    *pTxBuf ++= (uint8)(data_len);
    *pTxBuf ++= CMD_REPORT_P0;//0x1b cmd
    *pTxBuf ++= gizwitsProtocol.sn++;//sn
    *pTxBuf ++= 0x00;//flag
    *pTxBuf ++= 0x00;//flag
    *pTxBuf ++= action;//P0_Cmd

    /* 拷贝数据点数�?*/
    osal_memcpy(&tx_buf[9],data,len);
    
    tx_buf[data_len + 4 - 1 ] = gizProtocolSum( tx_buf , (data_len+4));
    
      ret = uartWrite(tx_buf, data_len+4);
    if(ret < 0)
    {
        //打印日志
        GIZWITS_LOG("ERROR: uart write error %d \n", ret);
        return -1;
    }

    gizProtocolWaitAck(tx_buf, data_len+4);

    return 0;
}

/**
* @brief 上报机制
*
* @param [in] currentData       : 数据地址
*
* @return : �?*/
static void gizDevReportPolicy(dataPoint_t *currentData)
{
    /** 变化上报机制 **/
    if((1 == gizCheckReport(currentData, (dataPoint_t *)&gizwitsProtocol.gizLastDataPoint)))
    {
        GIZWITS_LOG("changed, report data\n");
        if(0 == gizDataPoints2ReportData(currentData,gizwitsProtocol.reportData,(uint32 *)&gizwitsProtocol.reportDataLen))
        {
            gizReportData(ACTION_REPORT_DEV_STATUS, gizwitsProtocol.reportData, gizwitsProtocol.reportDataLen);
        }
        osal_memcpy((uint8 *)&gizwitsProtocol.gizLastDataPoint, (uint8 *)currentData, sizeof(dataPoint_t));
    }
    /** 定时上报机制 **/
    if(0 == (gizGetTimerCount() % (600000)))
    {
        GIZWITS_LOG("Info: 600S report data\n");
        osal_memset((uint8 *)&gizwitsProtocol.waitReportDatapointFlag,0xFF,DATAPOINT_FLAG_LEN);
        if(0 == gizDataPoints2ReportData(currentData,gizwitsProtocol.reportData,(uint32 *)&gizwitsProtocol.reportDataLen))
            gizReportData(ACTION_REPORT_DEV_STATUS, gizwitsProtocol.reportData, gizwitsProtocol.reportDataLen);
        osal_memcpy((uint8 *)&gizwitsProtocol.gizLastDataPoint, (uint8 *)currentData, sizeof(dataPoint_t));
    }
}

/**
* @brief 从环形缓冲区中抓取一包数�?*
* @param [in]  rb                  : 输入数据地址
* @param [out] data                : 输出数据地址
* @param [out] len                 : 输出数据长度
*
* @return : 0,正确返回;-1，错误返�?-2，数据校验失�?*/
static int8 gizProtocolGetOnePacket(rb_t *rb, uint8 *data, uint16 *len)
{
    int32 ret = 0;
    uint8 sum = 0;
    int32 i = 0;
    uint8 tmpData;
    uint8 tmpLen = 0;
    uint16 tmpCount = 0;
    static uint8 protocolFlag = 0;
    static uint16 protocolCount = 0;
    static uint8 lastData = 0;
    static uint8 debugCount = 0;
    uint8 *protocolBuff = data;
    protocolHead_t *head = NULL;

    if((NULL == rb) || (NULL == data) ||(NULL == len))
    {
        GIZWITS_LOG("gizProtocolGetOnePacket Error , Illegal Param\n");
        return -1;
    }

    tmpLen = rbCanRead(rb);
    if(0 == tmpLen)
    {
        return -1;
    }

    for(i=0; i<tmpLen; i++)
    {
        ret = rbRead(rb, &tmpData, 1);
        if(0 != ret)
        {
            if((0xFF == lastData) && (0xFF == tmpData))
            {
                if(0 == protocolFlag)
                {
                    protocolBuff[0] = 0xFF;
                    protocolBuff[1] = 0xFF;
                    protocolCount = 2;
                    protocolFlag = 1;
                }
                else
                {
                    if((protocolCount > 4) && (protocolCount != tmpCount))
                    {
                        protocolBuff[0] = 0xFF;
                        protocolBuff[1] = 0xFF;
                        protocolCount = 2;
                    }
                }
            }
            else if((0xFF == lastData) && (0x55 == tmpData))
            {
            }
            else
            {
                if(1 == protocolFlag)
                {
                    protocolBuff[protocolCount] = tmpData;
                    protocolCount++;

                    if(protocolCount > 4)
                    {
                        head = (protocolHead_t *)protocolBuff;
                        tmpCount = gizProtocolExchangeBytes(head->len)+4;
                        if(protocolCount == tmpCount)
                        {
                            break;
                        }
                    }
                }
            }

            lastData = tmpData;
            debugCount++;
        }
    }

    if((protocolCount > 4) && (protocolCount == tmpCount))
    {
        sum = gizProtocolSum(protocolBuff, protocolCount);

        if(protocolBuff[protocolCount-1] == sum)
        {
            osal_memcpy(data, protocolBuff, tmpCount);
            *len = tmpCount;
            protocolFlag = 0;

            protocolCount = 0;
            debugCount = 0;
            lastData = 0;

            return 0;
        }
        else
        {
            return -2;
        }
    }

    return 1;
}



/**
* @brief 协议数据重发

* 校验超时且满足重发次数限制的协议数据进行重发

* @param none    
*
* @return none
*/
static void gizProtocolResendData(void)
{
    int32 ret = 0;

    if(0 == gizwitsProtocol.waitAck.flag)
    {
        return;
    }

    GIZWITS_LOG("Warning: timeout, resend data \n");
    
    ret = uartWrite(gizwitsProtocol.waitAck.buf, gizwitsProtocol.waitAck.dataLen);
    if(ret != gizwitsProtocol.waitAck.dataLen)
    {
        GIZWITS_LOG("ERROR: resend data error\n");
    }

    gizwitsProtocol.waitAck.sendTime = gizGetTimerCount();
}

/**
* @brief 清除ACK协议报文
*
* @param [in] head : 协议头地址
*
* @return 0�?执行成功�?�?0�?失败
*/
static int8 gizProtocolWaitAckCheck(protocolHead_t *head)
{
    protocolHead_t *waitAckHead = (protocolHead_t *)gizwitsProtocol.waitAck.buf;

    if(NULL == head)
    {
        GIZWITS_LOG("ERROR: data is empty \n");
        return -1;
    }

    if(waitAckHead->cmd+1 == head->cmd)
    {
        osal_memset((uint8 *)&gizwitsProtocol.waitAck, 0, sizeof(protocolWaitAck_t));
    }

    return 0;
}

/**
* @brief 发送通用协议报文数据
* 
* @param [in] head              : 协议头指�?*
* @return : 有效数据长度,正确返回;-1，错误返�?*/
static int32 gizProtocolCommonAck(protocolHead_t *head)
{
    int32 ret = 0;
    protocolCommon_t ack;

    if(NULL == head)
    {
        GIZWITS_LOG("ERROR: gizProtocolCommonAck data is empty \n");
        return -1;
    }
    osal_memcpy((uint8 *)&ack, (uint8 *)head, sizeof(protocolHead_t));
    ack.head.cmd = ack.head.cmd+1;
    ack.head.len = gizProtocolExchangeBytes(sizeof(protocolCommon_t)-4);
    ack.sum = gizProtocolSum((uint8 *)&ack, sizeof(protocolCommon_t));

    ret = uartWrite((uint8 *)&ack, sizeof(protocolCommon_t));
    if(ret < 0)
    {
        //打印日志
        GIZWITS_LOG("ERROR: uart write error %d \n", ret);
    }

    return ret;
}

/**
* @brief ACK逻辑处理函数

* 发送后的协议数据进行超时判断，超时200ms进行重发，重发上限为三次

* @param none 
*
* @return none
*/
static void gizProtocolAckHandle(void)
{
    if(1 == gizwitsProtocol.waitAck.flag)
    {
        if(SEND_MAX_NUM > gizwitsProtocol.waitAck.num)
        {
            // 超时未收到ACK重发
            if(SEND_MAX_TIME < (gizGetTimerCount() - gizwitsProtocol.waitAck.sendTime))
            {
                GIZWITS_LOG("Warning:gizProtocolResendData %d %d %d\n", gizGetTimerCount(), gizwitsProtocol.waitAck.sendTime, gizwitsProtocol.waitAck.num);
                gizProtocolResendData();
                gizwitsProtocol.waitAck.num++;
            }
        }
        else
        {
            osal_memset((uint8 *)&gizwitsProtocol.waitAck, 0, sizeof(protocolWaitAck_t));
        }
    }
}

/**
* @brief 协议4.1 WiFi模组请求设备信息
*
* @param head : 待发送的协议报文数据
*
* @return 返回有效数据长度; -1，错误返�?*/
static int32 gizProtocolGetDeviceInfo(protocolHead_t * head)
{
    int32 ret = 0;
    protocolDeviceInfo_t deviceInfo;

    if(NULL == head)
    {
        GIZWITS_LOG("gizProtocolGetDeviceInfo Error , Illegal Param\n");
        return -1;
    }

    gizProtocolHeadInit((protocolHead_t *)&deviceInfo);
    deviceInfo.head.cmd = ACK_GET_DEVICE_INFO;
    deviceInfo.head.sn = head->sn;
    osal_memcpy((uint8 *)deviceInfo.protocolVer, protocol_VERSION, 8);
    osal_memcpy((uint8 *)deviceInfo.p0Ver, P0_VERSION, 8);
    osal_memcpy((uint8 *)deviceInfo.softVer, SOFTWARE_VERSION, 8);
    osal_memcpy((uint8 *)deviceInfo.hardVer, HARDWARE_VERSION, 8);
    osal_memcpy((uint8 *)deviceInfo.productKey, PRODUCT_KEY, 32);//strlen(PRODUCT_KEY));
    osal_memcpy((uint8 *)deviceInfo.productSecret, PRODUCT_SECRET, 32);//strlen(PRODUCT_SECRET));
    osal_memset((uint8 *)deviceInfo.devAttr, 0, 8);
    deviceInfo.devAttr[7] |= DEV_IS_GATEWAY<<0;
    deviceInfo.devAttr[7] |= (0x01<<1);
    deviceInfo.ninableTime = gizProtocolExchangeBytes(NINABLETIME);
    deviceInfo.head.len = gizProtocolExchangeBytes(sizeof(protocolDeviceInfo_t)-4);
    deviceInfo.sum = gizProtocolSum((uint8 *)&deviceInfo, sizeof(protocolDeviceInfo_t));

    ret = uartWrite((uint8 *)&deviceInfo, sizeof(protocolDeviceInfo_t));
    if(ret < 0)
    {
        //打印日志
        GIZWITS_LOG("ERROR: uart write error %d \n", ret);
    }
    
    return ret;
}

/**
* @brief 协议4.7 非法消息通知 的处�?
* @param[in] head  : 协议头指�?* @param[in] errno : 非法消息通知类型
* @return 0�?执行成功�?�?0�?失败
*/
static int32 gizProtocolErrorCmd(protocolHead_t *head,errorPacketsType_t errno)
{
    int32 ret = 0;
    protocolErrorType_t errorType;

    if(NULL == head)
    {
        GIZWITS_LOG("gizProtocolErrorCmd Error , Illegal Param\n");
        return -1;
    }
    gizProtocolHeadInit((protocolHead_t *)&errorType);
    errorType.head.cmd = ACK_ERROR_PACKAGE;
    errorType.head.sn = head->sn;
    
    errorType.head.len = gizProtocolExchangeBytes(sizeof(protocolErrorType_t)-4);
    errorType.error = errno;
    errorType.sum = gizProtocolSum((uint8 *)&errorType, sizeof(protocolErrorType_t));
    
    ret = uartWrite((uint8 *)&errorType, sizeof(protocolErrorType_t));
    if(ret < 0)
    {
        //打印日志
        GIZWITS_LOG("ERROR: uart write error %d \n", ret);
    }

    return ret;
}

/**
* @brief 对应协议 4.13 接收返回的网络时�?�?*
* @param [in] head : 协议头地址
*
* @return 0�?执行成功�?�?0�?失败
*/
static int8 gizProtocolNTP(protocolHead_t *head)
{
    int32 ret = 0;
    
    protocolUTT_t *UTTInfo = (protocolUTT_t *)head;
    
    if(NULL == head)
    {
        GIZWITS_LOG("ERROR: NTP is empty \n");
        return -1;
    }
    
    osal_memcpy((uint8 *)&gizwitsProtocol.TimeNTP,(uint8 *)UTTInfo->time, (7 + 4));
    gizwitsProtocol.TimeNTP.year = gizProtocolExchangeBytes(gizwitsProtocol.TimeNTP.year);
    gizwitsProtocol.TimeNTP.ntp = gizExchangeWord(gizwitsProtocol.TimeNTP.ntp);

    gizwitsProtocol.NTPEvent.event[gizwitsProtocol.NTPEvent.num] = WIFI_NTP;
    gizwitsProtocol.NTPEvent.num++;
    
    gizwitsProtocol.issuedFlag = GET_NTP_TYPE;
    
    
    return 0;
}

/**
* @brief 协议4.4 设备MCU重置WiFi模组 的相关操�?
* @param none
* @return none
*/
static void gizProtocolReboot(void)
{
    uint32 timeDelay = gizGetTimerCount();
    
    /*Wait 600ms*/
    while((gizGetTimerCount() - timeDelay) <= 600);
    mcuRestart();
}

/**
* @brief 协议 4.5 WiFi模组向设备MCU通知WiFi模组工作状态的变化

* @param[in] status WiFi模组工作状�?* @return none
*/
static int8 gizProtocolModuleStatus(protocolWifiStatus_t *status)
{
    static wifiStatus_t lastStatus;

    if(NULL == status)
    {
        GIZWITS_LOG("gizProtocolModuleStatus Error , Illegal Param\n");
        return -1;
    }

    status->ststus.value = gizProtocolExchangeBytes(status->ststus.value);
    if(0 == status->ststus.types.onboarding){//�Ƿ�������ģʽ =0û��
      if(0 == status->ststus.types.con_route){//��·��������״̬ =0 û��
        HalLedBlink( HAL_LED_1, 8, 10, 2000 );//����������̬��û������·����ʱ2s��˸ 8��
      }
      else{// = 1����·����
        if(1 == status->ststus.types.con_m2m){//��������
          HalLedBlink( HAL_LED_1, 5, 150, 3000 );//����������̬&&����·����&&���ϻ�����ʱ����
        }
        else{
          HalLedBlink( HAL_LED_1, 10, 70, 3000 );//����������̬&&����·����&&û�����ϻ�����ʱ3s��˸ 10��
        }
      }
    }
    else{// =1 ��������ģʽ
      HalLedBlink( HAL_LED_1, 0xff, 50, 1000 );//��������̬ʱ1s��˸ 255��
    }

    //OnBoarding mode status
    if(lastStatus.types.onboarding != status->ststus.types.onboarding)
    {
        if(1 == status->ststus.types.onboarding)
        {
            if(1 == status->ststus.types.softap)
            {
                gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_SOFTAP;
                gizwitsProtocol.wifiStatusEvent.num++;
                GIZWITS_LOG("OnBoarding: SoftAP or Web mode\n");
            }

            if(1 == status->ststus.types.station)
            {
                gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_AIRLINK;
                gizwitsProtocol.wifiStatusEvent.num++;
                GIZWITS_LOG("OnBoarding: AirLink mode\n");
            }
        }
        else
        {
            if(1 == status->ststus.types.softap)
            {
                gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_SOFTAP;
                gizwitsProtocol.wifiStatusEvent.num++;
                GIZWITS_LOG("OnBoarding: SoftAP or Web mode\n");
            }

            if(1 == status->ststus.types.station)
            {
                gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_STATION;
                gizwitsProtocol.wifiStatusEvent.num++;
                GIZWITS_LOG("OnBoarding: Station mode\n");
            }
        }
    }

    //binding mode status
    if(lastStatus.types.binding != status->ststus.types.binding)
    {
        lastStatus.types.binding = status->ststus.types.binding;
        if(1 == status->ststus.types.binding)
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_OPEN_BINDING;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: in binding mode\n");
        }
        else
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CLOSE_BINDING;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: out binding mode\n");
        }
    }

    //router status
    if(lastStatus.types.con_route != status->ststus.types.con_route)
    {
        lastStatus.types.con_route = status->ststus.types.con_route;
        if(1 == status->ststus.types.con_route)
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CON_ROUTER;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: connected router\n");
        }
        else
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_DISCON_ROUTER;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: disconnected router\n");
        }
    }

    //M2M server status
    if(lastStatus.types.con_m2m != status->ststus.types.con_m2m)
    {
        lastStatus.types.con_m2m = status->ststus.types.con_m2m;
        if(1 == status->ststus.types.con_m2m)
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CON_M2M;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: connected m2m\n");
        }
        else
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_DISCON_M2M;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: disconnected m2m\n");
        }
    }

    //APP status
    if(lastStatus.types.app != status->ststus.types.app)
    {
        lastStatus.types.app = status->ststus.types.app;
        if(1 == status->ststus.types.app)
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CON_APP;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: app connect\n");
        }
        else
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_DISCON_APP;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: no app connect\n");
        }
    }

    //test mode status
    if(lastStatus.types.test != status->ststus.types.test)
    {
        lastStatus.types.test = status->ststus.types.test;
        if(1 == status->ststus.types.test)
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_OPEN_TESTMODE;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: in test mode\n");
        }
        else
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CLOSE_TESTMODE;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("WiFi status: out test mode\n");
        }
    }

    gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_RSSI;
    gizwitsProtocol.wifiStatusEvent.num++;
    gizwitsProtocol.wifiStatusData.rssi = status->ststus.types.rssi;
    GIZWITS_LOG("RSSI is %d \n", gizwitsProtocol.wifiStatusData.rssi);

    gizwitsProtocol.issuedFlag = WIFI_STATUS_TYPE;

    return 0;
}


/**@name Gizwits 用户API接口
* @{
*/

/**
* @brief gizwits协议初始化接�?
* 用户调用该接口可以完成Gizwits协议相关初始化（包括协议相关定时器、串口的初始�?
* 用户可在在此接口内完成数据点的初始化状态设�?
* @param none
* @return none
*/
void gizwitsInit(void)
{    
    pRb.rbCapacity = RB_MAX_LEN;
    pRb.rbBuff = rbBuf;
    rbCreate(&pRb);
    
    osal_memset((uint8 *)&gizwitsProtocol, 0, sizeof(gizwitsProtocol_t));
}

/**
* @brief WiFi配置接口

* 用户可以调用该接口使WiFi模组进入相应的配置模式或者复位模�?
* @param[in] mode 配置模式选择�?x0�?模组复位 ;0x01�?SoftAp模式 ;0x02�?AirLink模式 ;0x03�?产测模式模式; 0x04:允许用户绑定设备

* @return 错误命令�?*/
uint32 gizwitsSetMode(uint8 mode)
{
    uint32 ret = 0;
    protocolCfgMode_t cfgMode;
    protocolCommon_t setDefault;

    switch(mode)
    {
//        case WIFI_RESET_MODE:
//            gizProtocolHeadInit((protocolHead_t *)&setDefault);
//            setDefault.head.cmd = CMD_SET_DEFAULT;
//            setDefault.head.sn = gizwitsProtocol.sn++;
//            setDefault.head.len = gizProtocolExchangeBytes(sizeof(protocolCommon_t)-4);
//            setDefault.sum = gizProtocolSum((uint8 *)&setDefault, sizeof(protocolCommon_t));
//            ret = uartWrite((uint8 *)&setDefault, sizeof(protocolCommon_t));
//            if(ret < 0)
//            {
//                GIZWITS_LOG("ERROR: uart write error %d \n", ret);
//            }
//
//            gizProtocolWaitAck((uint8 *)&setDefault, sizeof(protocolCommon_t));   
//            break;
//        case WIFI_SOFTAP_MODE:
//            gizProtocolHeadInit((protocolHead_t *)&cfgMode);
//            cfgMode.head.cmd = CMD_WIFI_CONFIG;
//            cfgMode.head.sn = gizwitsProtocol.sn++;
//            cfgMode.cfgMode = mode;
//            cfgMode.head.len = gizProtocolExchangeBytes(sizeof(protocolCfgMode_t)-4);
//            cfgMode.sum = gizProtocolSum((uint8 *)&cfgMode, sizeof(protocolCfgMode_t));
//            ret = uartWrite((uint8 *)&cfgMode, sizeof(protocolCfgMode_t));
//            if(ret < 0)
//            {
//                GIZWITS_LOG("ERROR: uart write error %d \n", ret);
//            }
//            gizProtocolWaitAck((uint8 *)&cfgMode, sizeof(protocolCfgMode_t)); 
//            break;
        case WIFI_AIRLINK_MODE:
            gizProtocolHeadInit((protocolHead_t *)&cfgMode);
            cfgMode.head.cmd = CMD_WIFI_CONFIG;
            cfgMode.head.sn = gizwitsProtocol.sn++;
            cfgMode.cfgMode = mode;
            cfgMode.head.len = gizProtocolExchangeBytes(sizeof(protocolCfgMode_t)-4);
            cfgMode.sum = gizProtocolSum((uint8 *)&cfgMode, sizeof(protocolCfgMode_t));
            ret = uartWrite((uint8 *)&cfgMode, sizeof(protocolCfgMode_t));
            if(ret == 0)
            {
                GIZWITS_LOG("ERROR: uart write error %d \n", ret);
            }
            gizProtocolWaitAck((uint8 *)&cfgMode, sizeof(protocolCfgMode_t)); 
            break;
//        case WIFI_PRODUCTION_TEST:
//            gizProtocolHeadInit((protocolHead_t *)&setDefault);
//            setDefault.head.cmd = CMD_PRODUCTION_TEST;
//            setDefault.head.sn = gizwitsProtocol.sn++;
//            setDefault.head.len = gizProtocolExchangeBytes(sizeof(protocolCommon_t)-4);
//            setDefault.sum = gizProtocolSum((uint8 *)&setDefault, sizeof(protocolCommon_t));
//            ret = uartWrite((uint8 *)&setDefault, sizeof(protocolCommon_t));
//            if(ret < 0)
//            {
//                GIZWITS_LOG("ERROR: uart write error %d \n", ret);
//            }
//
//            gizProtocolWaitAck((uint8 *)&setDefault, sizeof(protocolCommon_t));
//            break;
//        case WIFI_NINABLE_MODE:
//            gizProtocolHeadInit((protocolHead_t *)&setDefault);
//            setDefault.head.cmd = CMD_NINABLE_MODE;
//            setDefault.head.sn = gizwitsProtocol.sn++;
//            setDefault.head.len = gizProtocolExchangeBytes(sizeof(protocolCommon_t)-4);
//            setDefault.sum = gizProtocolSum((uint8 *)&setDefault, sizeof(protocolCommon_t));
//            ret = uartWrite((uint8 *)&setDefault, sizeof(protocolCommon_t));
//            if(ret < 0)
//            {
//                GIZWITS_LOG("ERROR: uart write error %d \n", ret);
//            }
//
//            gizProtocolWaitAck((uint8 *)&setDefault, sizeof(protocolCommon_t)); 
//            break;
        default:
            GIZWITS_LOG("ERROR: CfgMode error!\n");
            break;
    }

    return ret;
}

/**
* @brief 获取网络时间的接�?
* 对应协议 4.13 MCU请求获取网络时间 中的 设备MCU发�?
* @param[in] none
* @return none
*/
void gizwitsGetNTP(void)
{
    int32 ret = 0;
    protocolCommon_t getNTP;

    gizProtocolHeadInit((protocolHead_t *)&getNTP);
    getNTP.head.cmd = CMD_GET_NTP;
    getNTP.head.sn = gizwitsProtocol.sn++;
    getNTP.head.len = gizProtocolExchangeBytes(sizeof(protocolCommon_t)-4);
    getNTP.sum = gizProtocolSum((uint8 *)&getNTP, sizeof(protocolCommon_t));
    ret = uartWrite((uint8 *)&getNTP, sizeof(protocolCommon_t));
    if(ret < 0)
    {
        GIZWITS_LOG("ERROR[NTP]: uart write error %d \n", ret);
    }

    gizProtocolWaitAck((uint8 *)&getNTP, sizeof(protocolCommon_t));
}

/**
* @brief 协议处理函数

* 该函数中完成了相应协议数据的处理及数据主动上报的相关操作

* @param [in] currentData 待上报的协议数据指针
* @return none
*/
int32 gizwitsHandle(dataPoint_t *currentData)
{
  uint8 asc_16[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  int8 ret = 0;
  uint16 i = 0;
  uint8 ackData[RB_MAX_LEN];
  uint16 protocolLen = 0;
  uint32 ackLen = 0;
  protocolHead_t *recvHead = NULL;
  static Node * nodeptr=NULL;
  Node * deletnodeptr;
  static Node * updatenodeptr=NULL;
    if(NULL == currentData)
    {
        GIZWITS_LOG("GizwitsHandle Error , Illegal Param\n");
        return -1;
    }

    /*重发机制*/
    gizProtocolAckHandle();
    ret = gizProtocolGetOnePacket(&pRb, gizwitsProtocol.protocolBuf, &protocolLen);

    if(0 == ret)
    {
        GIZWITS_LOG("Get One Packet!\n");
        
#ifdef PROTOCOL_DEBUG
        GIZWITS_LOG("WiFi2MCU[%4d:%4d]: ", gizGetTimerCount(), protocolLen);
        for(i=0; i<protocolLen;i++)
        {
            GIZWITS_LOG("%02x ", gizwitsProtocol.protocolBuf[i]);
        }
        GIZWITS_LOG("\n");
#endif

        recvHead = (protocolHead_t *)gizwitsProtocol.protocolBuf;
        switch (recvHead->cmd)
        {
            case CMD_GET_DEVICE_INTO:
                gizProtocolGetDeviceInfo(recvHead);
                break;
            case CMD_ISSUED_P0:
                if(0 == gizProtocolIssuedProcess(gizwitsProtocol.protocolBuf, protocolLen, ackData, &ackLen))
                {
                    gizProtocolIssuedDataAck(recvHead, ackData, ackLen);
                }
                break;
            case CMD_HEARTBEAT:
                gizProtocolCommonAck(recvHead);
                break;
            case CMD_WIFISTATUS:
                gizProtocolCommonAck(recvHead);
                gizProtocolModuleStatus((protocolWifiStatus_t *)recvHead);
                break;
            case ACK_REPORT_P0:
            case ACK_WIFI_CONFIG:
            case ACK_SET_DEFAULT:
            case ACK_NINABLE_MODE:
                gizProtocolWaitAckCheck(recvHead);
                break;
            case CMD_MCU_REBOOT:
                gizProtocolCommonAck(recvHead);
                GIZWITS_LOG("report:MCU reboot!\n");
                
                gizProtocolReboot();
                break;
            case CMD_ERROR_PACKAGE:
                break;
            case ACK_PRODUCTION_TEST:
                gizProtocolWaitAckCheck(recvHead);
                GIZWITS_LOG("Ack PRODUCTION_MODE success \n");
                break;           
            case ACK_GET_NTP:
                gizProtocolWaitAckCheck(recvHead);
                gizProtocolNTP(recvHead);
                GIZWITS_LOG("Ack GET_UTT success \n");
                break;   
            default:
                gizProtocolErrorCmd(recvHead,ERROR_CMD);
                GIZWITS_LOG("ERROR: cmd code error!\n");
                break;
        }
    }
    else if(-2 == ret)
    {
        //校验失败，报告异�?        recvHead = (protocolHead_t *)gizwitsProtocol.protocolBuf;
        gizProtocolErrorCmd(recvHead,ERROR_ACK_SUM);
        GIZWITS_LOG("ERROR: check sum error!\n");
        return -2;
    }
    
    switch(gizwitsProtocol.issuedFlag)
    {
        case ACTION_CONTROL_TYPE:
            gizwitsProtocol.issuedFlag = STATELESS_TYPE;
            gizwitsEventProcess(&gizwitsProtocol.issuedProcessEvent, (uint8 *)&gizwitsProtocol.gizCurrentDataPoint, sizeof(dataPoint_t));
            osal_memset((uint8 *)&gizwitsProtocol.issuedProcessEvent,0x0,sizeof(gizwitsProtocol.issuedProcessEvent));  
            break;
        case WIFI_STATUS_TYPE:
            gizwitsProtocol.issuedFlag = STATELESS_TYPE;
            gizwitsEventProcess(&gizwitsProtocol.wifiStatusEvent, (uint8 *)&gizwitsProtocol.wifiStatusData, sizeof(moduleStatusInfo_t));
            osal_memset((uint8 *)&gizwitsProtocol.wifiStatusEvent,0x0,sizeof(gizwitsProtocol.wifiStatusEvent));
            break;
        case ACTION_W2D_TRANSPARENT_TYPE:
            gizwitsProtocol.issuedFlag = STATELESS_TYPE;
            gizwitsEventProcess(&gizwitsProtocol.issuedProcessEvent, (uint8 *)gizwitsProtocol.transparentBuff, gizwitsProtocol.transparentLen);
            break;
        case GET_NTP_TYPE:
            gizwitsProtocol.issuedFlag = STATELESS_TYPE;
            gizwitsEventProcess(&gizwitsProtocol.NTPEvent, (uint8 *)&gizwitsProtocol.TimeNTP, sizeof(protocolTime_t));
            osal_memset((uint8 *)&gizwitsProtocol.NTPEvent,0x0,sizeof(gizwitsProtocol.NTPEvent));
            break;  
        default:
          if(MacToAppFlag ==1){
            if(nodeptr == NULL){
              nodeptr = nodelist;
              for(i=0;i<20;i++)
                currentDataPoint.valuepayload[i]=0;
            }
            for(i=0;nodeptr&&i<=1;nodeptr=nodeptr->next,i++)
            {
              osal_memcpy((uint8 *)&currentDataPoint.valuepayload[3+8*i],(uint8 *)&nodeptr->extaddr, 8);
            }

            if(nodeptr == NULL){
              MacToAppFlag = 0;
            }
            currentDataPoint.valuepayload[0]=0x10;
            currentDataPoint.valuepayload[1]=i;
            if(i==1)
              for(i=11;i<20;i++)
                currentDataPoint.valuepayload[i]=0;
          }
          else if(deletnodelist){
            AutoUpateMac = 1;
            deletnodeptr = deletnodelist->next;
            for(i=0;i<20;i++)
              currentDataPoint.valuepayload[i]=0;
            osal_memcpy((uint8 *)&currentDataPoint.valuepayload[3],(uint8 *)&deletnodelist->extaddr, 8);
            HalUARTWrite(0,"\nD:",3);
            for(i=0;i<8;i++)
            {
              HalUARTWrite(0,&asc_16[((Node*)deletnodelist)->extaddr[i]%256/16],1);  
              HalUARTWrite(0,&asc_16[((Node*)deletnodelist)->extaddr[i]%16],1); 
            }
            HalUARTWrite(0,"\r\n",2);
            osal_mem_free(((Node*)deletnodelist));
            deletnodelist = deletnodeptr;
            if(deletnodelist){
              deletnodeptr = deletnodelist->next;
              osal_memcpy((uint8 *)&currentDataPoint.valuepayload[3+8],(uint8 *)&deletnodelist->extaddr, 8);
              currentDataPoint.valuepayload[0] = 0x20;
              currentDataPoint.valuepayload[1] = 2;
              HalUARTWrite(0,"\nD:",3);
              for(i=0;i<8;i++)
              {
                HalUARTWrite(0,&asc_16[((Node*)deletnodelist)->extaddr[i]%256/16],1);  
                HalUARTWrite(0,&asc_16[((Node*)deletnodelist)->extaddr[i]%16],1); 
              }
              HalUARTWrite(0,"\r\n",2);
              osal_mem_free(((Node*)deletnodelist));
              deletnodelist = deletnodeptr;
            }
            else{
              currentDataPoint.valuepayload[0] = 0x20;
              currentDataPoint.valuepayload[1] = 1;
            }
            AutoUpateMac = 0;
          }
          else if(NewMacUpate){
            updatenodeptr = nodelist;
            for(i=0;updatenodeptr && i<2;updatenodeptr = updatenodeptr->next){
              if(updatenodeptr->element){
                updatenodeptr->element = 0;
                osal_memcpy((uint8 *)&currentDataPoint.valuepayload[3+8*i],(uint8 *)&updatenodeptr->extaddr, 8);
                i++;
              }

            }
            if(updatenodeptr==NULL){
              NewMacUpate = 0;
            }
            currentDataPoint.valuepayload[0] = 0x10;
            currentDataPoint.valuepayload[1] = i;
          }
            break;
    }

    gizDevReportPolicy(currentData);

    return 0;
}

/**
* @brief gizwits上报透传数据接口

* 用户调用该接口可以完成私有协议数据的上报

* @param [in] data 输入的私有协议数�?* @param [in] len 输入的私有协议数据长�?* @return -1 , 错误返回;0,正确返回
*/
int32 gizwitsPassthroughData(uint8 * data, uint32 len)
{
	int32 ret = 0;
	uint8 tx_buf[MAX_PACKAGE_LEN];
    if(NULL == data)
    {
        GIZWITS_LOG("[Error] gizwitsPassthroughData Error \n");
        return (-1);
    }
	uint8 *pTxBuf = tx_buf;
	uint16 data_len = 6+len;
	*pTxBuf ++= 0xFF;
	*pTxBuf ++= 0xFF;
	*pTxBuf ++= (uint8)(data_len>>8);//len
	*pTxBuf ++= (uint8)(data_len);
	*pTxBuf ++= CMD_REPORT_P0;//0x1b cmd
	*pTxBuf ++= gizwitsProtocol.sn++;//sn
	*pTxBuf ++= 0x00;//flag
	*pTxBuf ++= 0x00;//flag
	*pTxBuf ++= ACTION_D2W_TRANSPARENT_DATA;//P0_Cmd

    osal_memcpy(&tx_buf[9],data,len);
    tx_buf[data_len + 4 - 1 ] = gizProtocolSum( tx_buf , (data_len+4));
    
	ret = uartWrite(tx_buf, data_len+4);
    if(ret < 0)
    {
        //打印日志
        GIZWITS_LOG("ERROR: uart write error %d \n", ret);
    }

    gizProtocolWaitAck(tx_buf, data_len+4);

    return 0;
}
/**@} */
