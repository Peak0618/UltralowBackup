
//------------------------------------------------------------------------------
//包含文件声明
#include "r_cg_macrodriver.h"

#include "h_type_define.h"
#include "m_test.h"
#include "m_com.h"

//------------------------------------------------------------------------------
//函数声明
void com_init(void);      //通讯初始化程序，在初始化程序中调用

void com_rx_init(void);  //通讯接收初始化程序，在主循环程序中调用

void com_rx_data_deal(void);  //通讯接收数据处理程序，在主循环程序中调用

void com_tx_init(void);         //通讯发送初始化程序，在主循环程序中调用

void com_rx_delaytime(void);  //通讯接收延时程序，在1ms定时程序中调用

void com_tx_delaytime(void);  //通讯发送延时程序，在1ms定时程序中调用

void com_rx_end_delaytime(void);   //通讯接收完成延时程序，在1ms定时程序中调用

void com_fault_delaytime(void);    //通讯故障延时程序，在1s定时程序中调用

void com_rx_int(uint8_t rx_data);       //通讯接收中断程序

void com_tx_int(void);                  //通讯发送中断程序

unsigned int CRC16(unsigned char *puchmsg, unsigned int usDataLen);    //进行CRC校验的程序
//------------------------------------------------------------------------------
//标志定义
flag_type flg_com;
//------------------------------------------------------------------------------
//变量定义
uint8_t guc_com_tx_buffer[9];  //通讯发送缓存器
uint8_t guc_com_rx_buffer[53];  //通讯接收缓存器  23

int16_t gss_com_tx_delaytimer;     //通讯发送延时计时器
int16_t gss_com_rx_delaytimer;     //通讯接收延时计时器

int16_t gss_com_rx_end_delaytimer; //通讯接收完标志
int16_t gss_com_fault_delaytimer;  //通讯故障延时计时器

uint8_t guc_com_tx_point;     //通讯发送索引
uint8_t guc_com_rx_point;     //通讯接收索引
//------------------------------------------------------------------------------
uint8_t guc_led_buffer[LED_BUF_LEN];    //数码管寄存器
uint8_t guc_key_buffer[KEY_BUF_LEN];    //按键寄存器

uint8_t guc_key_val[KEY_BUF_LEN];       //按键值  现在只用了 guc_key_val[0]


/*************************************************************************************************************************************************************
函数功能：初始上电时的初始化通讯口，默认主机/从机的选择

函数位置：工程初始化---------------------------------ok
**************************************************************************************************************************************************************/
void com_init(void) 
{
    bflg_com_rx_delaytime = 1;   //默认为接收，从机
    gss_com_rx_delaytimer = 5;
    
    bflg_com_tx_delaytime = 0;
    gss_com_tx_delaytimer = 0;   
}

/*************************************************************************************************************************************************************
函数功能：接收时接收缓存初始化

函数位置：主循环----------------------------------未调
**************************************************************************************************************************************************************/
void com_rx_init(void)    
{
    uint8_t i;
    
    for (i = 0; i < 53; i++)      //23
    {
        guc_com_rx_buffer[i] = 0;  //清接收缓存器
    }
    
    bflg_com_rx_busy = 0;
    guc_com_rx_point = 0;
}
/*************************************************************************************************************************************************************
函数功能：对接收的函数进行处理

函数位置：主循环----------------------------------未调
**************************************************************************************************************************************************************/
void com_rx_data_deal(void)   //通讯接收数据处理程序，在主循环程序中调用
{
    uint8_t i, msb_byte = 0,lsb_byte = 0;
    uint16_t lus_tmp;
    word_type com_crc;
    //------------------------------------------------------
    if ((guc_com_rx_buffer[1] == 0xaa)) //主机要从机数据的指令
    {
        com_crc.uword = CRC16(guc_com_rx_buffer, 6);
        if ((com_crc.ubyte.low == guc_com_rx_buffer[6])
         && (com_crc.ubyte.high == guc_com_rx_buffer[7]))
        {
            bflg_com_tx_delaytime = 1;       //置发送延时标志
            gss_com_tx_delaytimer = 10;      //发送延时计时器赋值
            
            bflg_com_rx_delaytime = 0;       //清发送延时标志
            gss_com_rx_delaytimer = 0;       //发送延时计时器清零
            
            bflg_com_fault = 0;              //清通讯故障标志
            gss_com_fault_delaytimer = 0;    //清通讯故障计时器
            //----------------------------------------------
            //数据处理
            bflg_read_write_data = 0;   //当前是读数据
        }
    }  
}
/*************************************************************************************************************************************************************
函数功能：发送初始化，将需要发送的数据赋给发送缓存

函数位置：主循环----------------------------------未调
**************************************************************************************************************************************************************/
void com_tx_init(void)   //通讯发送初始化程序，在主循环程序中调用
{
    word_type com_crc;      
    uint8_t  msb_byte = 0, lsb_byte = 0;
    //------------------------------------------------------cc
    if (bflg_read_write_data == 0) //如果是读数据  //peak 读取按键的值
    {
        guc_com_tx_buffer[0] = guc_com_rx_buffer[0];
        guc_com_tx_buffer[1] = guc_com_rx_buffer[1];
        guc_com_tx_buffer[2] = 4;  //字节个数4
        //--------------------------------------------------
        guc_com_tx_buffer[3] = guc_key_val[0];                //按键值
       
        guc_com_tx_buffer[4] = guc_key_val[1];   //peak 原来   
        guc_com_tx_buffer[5] = guc_key_val[2];         
        guc_com_tx_buffer[6] = guc_key_val[3];          

        //--------------------------------------------------
        com_crc.uword = CRC16(guc_com_tx_buffer, 7);
        guc_com_tx_buffer[7] = com_crc.ubyte.low; 
        guc_com_tx_buffer[8] = com_crc.ubyte.high;
    }
    else  //如果是写数据 //设置数码管、LED显示
    {
        guc_com_tx_buffer[0] = guc_com_rx_buffer[0];  //peak从设备地址
        guc_com_tx_buffer[1] = guc_com_rx_buffer[1];  //功能码
        guc_com_tx_buffer[2] = guc_com_rx_buffer[2];   
        guc_com_tx_buffer[3] = guc_com_rx_buffer[3];
        guc_com_tx_buffer[4] = guc_com_rx_buffer[4];
        guc_com_tx_buffer[5] = guc_com_rx_buffer[5];
        //--------------------------------------------------
        com_crc.uword = CRC16(guc_com_tx_buffer, 6);
        guc_com_tx_buffer[6] = com_crc.ubyte.low;
        guc_com_tx_buffer[7] = com_crc.ubyte.high;
    }
    
    guc_com_tx_point = 0;
    bflg_com_tx_busy = 1;
    TXD0 = guc_com_tx_buffer[guc_com_tx_point++];
}
/*************************************************************************************************************************************************************
函数功能：延时多长时间再接收

函数位置：1ms定时器----------------------------------ok
**************************************************************************************************************************************************************/
void com_rx_delaytime(void)     //通讯接收延时程序，在1ms定时程序中调用
{
    if (bflg_com_rx_delaytime == 1)
    {
        gss_com_rx_delaytimer--;
        if (gss_com_rx_delaytimer <= 0)
        {
            gss_com_rx_delaytimer = 0;
            bflg_com_rx_delaytime = 0;
            
            bflg_com_allow_rx = 1;
        }
    }
}
/*************************************************************************************************************************************************************
函数功能：延时多长时间再发送

函数位置：1ms定时器----------------------------------ok
**************************************************************************************************************************************************************/
void com_tx_delaytime(void)     //通讯发送延时程序，在1ms定时程序中调用
{
    if (bflg_com_tx_delaytime == 1)
    {
        gss_com_tx_delaytimer--;
        if (gss_com_tx_delaytimer <= 0)
        {
            gss_com_tx_delaytimer = 0;
            bflg_com_tx_delaytime = 0;
            
            bflg_com_allow_tx = 1;
        }
    }
}
/*******************************************************************************************************************************
函数功能：此函数判断每一个字节是否超时，因为gss_com_rx_end_delaytimer = 0 放在com_rx_int()中
          ？？实际用处应该是判断一帧数据是否超时吧？？

函数位置：1ms定时器中-------------------ok
********************************************************************************************************************************/
void com_rx_end_delaytime(void) //通讯接收完成延时程序，在1ms定时程序中调用
{
    if (bflg_com_rx_busy == 1)  //如果当前接收忙
    {
        gss_com_rx_end_delaytimer++;
        if (gss_com_rx_end_delaytimer >= 50) // >= 2   //调试
        {
            gss_com_rx_end_delaytimer = 0;
            bflg_com_rx_busy = 0;
            
            bflg_com_rx_end = 1;
            
            bflg_com_rx_delaytime = 1;
            gss_com_rx_delaytimer = 10;
        }
    }
}
/*******************************************************************************************************************************
函数功能：故障延时程序

函数位置：1ms定时器中-------------------未调
********************************************************************************************************************************/
void com_fault_delaytime(void)     //通讯故障延时程序，在1s定时程序中调用
{
    if (bflg_com_fault == 0)
    {
        gss_com_fault_delaytimer++;
        if (gss_com_fault_delaytimer >= 30)  //30
        {
            gss_com_fault_delaytimer = 0;
            bflg_com_fault = 1;
        }
    }
}

/*******************************************************************************************************************************
函数功能：接收数据，此函数在系统串口中断中，这个中断每中断一次是接收一个字节

函数位置：在系统串口接收中断中------------------ok
********************************************************************************************************************************/
void com_rx_int(uint8_t rx_data)   //通讯接收中断程序
{
    uint8_t luc_com_rx_byte;  //接收字节临时缓冲区
    
    gss_com_rx_end_delaytimer = 0; //清接收完延时计时器
    //--------------------------------------------------
    luc_com_rx_byte = rx_data;
    
    if(bflg_test_mode == 0)
    {
        if ((bflg_com_rx_busy == 0) && (luc_com_rx_byte == 0x00))    //LED地址
        {
            bflg_test_mode = 1;
            
            bflg_com_rx_busy = 1;            
            guc_com_rx_buffer[0] = luc_com_rx_byte;
            guc_com_rx_point = 1;
        }
        else if ((bflg_com_rx_busy == 0) && (luc_com_rx_byte == 0x01))    //液晶地址
        {
            bflg_com_rx_busy = 1;
            
            guc_com_rx_buffer[0] = luc_com_rx_byte;
            guc_com_rx_point = 1;
        }
        else if (bflg_com_rx_busy == 1)     //如果接收忙
        {
            guc_com_rx_buffer[guc_com_rx_point++] = luc_com_rx_byte;
            
            if ((guc_com_rx_buffer[1] == 0x03) && (guc_com_rx_point >= 8))     //如果是查询命令，且接收数据完毕
            {
                guc_com_rx_point = 0;  //清接收字节索引
                bflg_com_rx_busy = 0;  //清接收忙标志
                bflg_com_rx_ok = 1;    //置接收成功标志
                
                bflg_com_rx_delaytime = 1;   //置接收延时标志
                gss_com_rx_delaytimer = 50;  //接收延时计时器赋值
            }
            else if ((guc_com_rx_buffer[1] == 0x10) && (guc_com_rx_point >= 53))  //23   //如果是多个设定命令，且接收到字节数数据帧
            {
                guc_com_rx_point = 0;   
                bflg_com_rx_busy = 0;   
                bflg_com_rx_ok = 1;    
                
                bflg_com_rx_delaytime = 1;  
                gss_com_rx_delaytimer = 50;  
            }
            else if(guc_com_rx_point >= 53)
            {
                guc_com_rx_point = 0;   
                bflg_com_rx_busy = 0;   
                bflg_com_rx_ok = 1;    
                
                bflg_com_rx_delaytime = 1;  
                gss_com_rx_delaytimer = 50;  
            }
        }
    }
    else
    {
        if ((bflg_com_rx_busy == 0) && (luc_com_rx_byte == 0x00))    //LED的地址
        {
            bflg_com_rx_busy = 1;
            
            guc_com_rx_buffer[0] = luc_com_rx_byte;
            guc_com_rx_point = 1;
        }
        else if(bflg_com_rx_busy == 1)
        {
            guc_com_rx_buffer[guc_com_rx_point++] = luc_com_rx_byte;
            
            if ((guc_com_rx_buffer[1] == 0xaa) && (guc_com_rx_point >= 8))     
            {
                guc_com_rx_point = 0; 
                bflg_com_rx_busy = 0;  
                bflg_com_rx_ok = 1;    
                
                bflg_com_rx_delaytime = 1;   
                gss_com_rx_delaytimer = 50;  
            }
        }
    }

}

/*************************************************************************************************************************************************************
函数功能：字节在中断中一个个发送

函数位置：系统中断程序中---------------------------------ok
**************************************************************************************************************************************************************/
void com_tx_int(void)    //通讯发送中断程序
{
    if(bflg_test_mode == 0)
    {
        if (bflg_com_tx_busy == 1)
        {                                   //包含开锁指令 buff[6]的最高位代表开锁指令
            if (bflg_read_write_data == 0)  //如果是读数据 peak 主控向显示板读取按键值，此时显示板向主控返回按键值
            {
                if (guc_com_tx_point < 9)
                {
                    TXD0 = guc_com_tx_buffer[guc_com_tx_point++];    //发送数据
                }
                else
                {
                    guc_com_tx_point = 0;   //清发送字节索引
                    bflg_com_tx_busy = 0;   //清发送忙标志
                    bflg_com_tx_ok = 1;
                    
                    bflg_com_rx_delaytime = 1;   //置com通讯接收延时标志
                    gss_com_rx_delaytimer = 5;   //com通讯接收延时计时器赋值
                }
            }
            else    //如果是写数据 peak主控向显示板写数码管显示的内容
            {
                if (guc_com_tx_point < 8)
                {
                    TXD0 = guc_com_tx_buffer[guc_com_tx_point++];   //发送数据
                }
                else
                {
                    guc_com_tx_point = 0;   //清发送字节索引
                    bflg_com_tx_busy = 0;   //清发送忙标志
                    bflg_com_tx_ok = 1;
                    
                    bflg_com_rx_delaytime = 1;   //置通讯接收延时标志
                    gss_com_rx_delaytimer = 5;   //通讯接收延时计时器赋值
                }
            }
        }
    }
    else
    {
        if (bflg_com_tx_busy == 1)
        {
            if (guc_com_tx_point < 9)
            {
                TXD0 = guc_com_tx_buffer[guc_com_tx_point++];    //发送数据
            }
            else
            {
                guc_com_tx_point = 0;   //清发送字节索引
                bflg_com_tx_busy = 0;   //清发送忙标志
                bflg_com_tx_ok = 1;
                
                bflg_com_rx_delaytime = 1;   //置com通讯接收延时标志
                gss_com_rx_delaytimer = 5;   //com通讯接收延时计时器赋值
            }
        }
    }
}
/*************************************************************************************************************************************************************
函数功能：CRC校验

函数位置：发送/接收数据时对数据进行的校验----------------------------------未调
**************************************************************************************************************************************************************/
unsigned int CRC16(unsigned char *puchmsg, unsigned int usDataLen)    
{
    unsigned int uchCRC;
    unsigned int uIndex_x, uIndex_y;
    
    uchCRC = 0xFFFF;
    
    if (usDataLen > 0)
    {
        for (uIndex_x = 0; uIndex_x <= (usDataLen - 1); uIndex_x++)
        {
            uchCRC = uchCRC ^ (unsigned int) (*puchmsg);
            puchmsg++;
            
            for (uIndex_y = 0; uIndex_y <= 7; uIndex_y++)
            {
                if ((uchCRC & 0x0001) != 0)
                {
                    uchCRC = (uchCRC >> 1) ^ 0xA001;
                }
                else
                {
                    uchCRC = uchCRC >> 1;
                }
            }
        }
    }
    else
    {
        uchCRC = 0;
    }
    
    return uchCRC;
}

/*****************************************END OF THE FILE*********************************************************************************************************************/
