/************************************************************************************************************************
overview: 对后备板进行工装检测时进入
          1、主板发特定的数据指令会激活后备板进入测试模式
          2、在测试程序运行
*************************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_timer.h"
#include "r_cg_serial.h"

#include "h_type_define.h"
#include "m_com.h"
#include "m_ad.h"
#include "m_test.h"
#include <stdlib.h>


//函数声明
void test_mode_com();
void test_com_rx_data_deal(void);   
void test_com_tx_init(void);
void test_in_out_pin(void);
void test_mode_LED_deal(void);     
void test_error_code_deal(void);

//变量
flag_type flg_test;


uint8_t guc_test_mode_code;     //错误代码

//宏定义 
//输入
#define TEST_CN6_IN_PIN     P3.0   //CN6端口输入检测
#define TEST_CN7_IN_PIN     P5.0   //CN7端口输入检测
#define TEST_SW1_1_IN_PIN   P1.7   //拨码开关1
#define TEST_SW1_2_IN_PIN   P1.6   //拨码开关2

//输出
#define TEST_LED1_OUT_PIN   P1.3    //LED1
#define TEST_LED2_OUT_PIN   P1.4    //LED2
#define TEST_CN5_OUT_PIN    P5.1    //控制端口CN5继电器
#define TEST_BAT_OUT_PIN    P14.7   //控制电池开关的引脚



/*******************************************************************************************************************************
函数功能：测试模式下进行的通讯处理

函数位置：主循环-----------------------------------------ok
********************************************************************************************************************************/
void test_mode_com(void)
{
    if (bflg_com_allow_rx == 1)  //如果允许接收
    {
        bflg_com_allow_rx = 0;
        
        com_rx_init();   
        COM_RX_MODE;
        R_UART0_Start();
    }
    if (bflg_com_rx_end == 1)    //如果接收结束
    {
        bflg_com_rx_end = 0;
        
        R_UART0_Stop();
    }
    if (bflg_com_rx_ok == 1)     //如果接收成功
    {
        bflg_com_rx_ok = 0;
        
        R_UART0_Stop();
        test_com_rx_data_deal();   
    }
    if (bflg_com_allow_tx == 1)  //如果允许发送
    {
        bflg_com_allow_tx = 0;
        
        R_UART0_Start();
        COM_TX_MODE;
        test_com_tx_init();   
    }
    if (bflg_com_tx_ok == 1)     //如果发送成功
    {
        bflg_com_tx_ok = 0;      
        
        R_UART0_Stop();
    }
}

/*******************************************************************************************************************************
函数功能：测试模式下对接收的数据处理

函数位置：主循环 test_mode_com()-----------------------------------------ok
********************************************************************************************************************************/
void test_com_rx_data_deal(void)   
{
    uint8_t i, msb_byte = 0,lsb_byte = 0;
    uint16_t lus_tmp;
    word_type com_crc;
    //------------------------------------------------------
    if ((guc_com_rx_buffer[1] == 0xaa)) //如果是查询命令
    {
        com_crc.uword = CRC16(guc_com_rx_buffer, 6);
        if ((com_crc.ubyte.low == guc_com_rx_buffer[6])
         && (com_crc.ubyte.high == guc_com_rx_buffer[7]))
        {
            bflg_com_tx_delaytime = 1;       //置发送延时标志
            gss_com_tx_delaytimer = 10;      //发送延时计时器赋值
            
            bflg_com_rx_delaytime = 0;       //清发送延时标志
            gss_com_rx_delaytimer = 0;       //发送延时计时器清零
            
            //bflg_com_fault = 0;              //清通讯故障标志
            //gss_com_fault_delaytimer = 0;    //清通讯故障计时器
        }
    }  
}

/*******************************************************************************************************************************
函数功能：测试模式下初始化发送的数据

函数位置：主循环 test_mode_com()-----------------------------------------ok
********************************************************************************************************************************/
void test_com_tx_init(void)
{
    word_type com_crc;      
    uint8_t  msb_byte = 0, lsb_byte = 0;

    guc_com_tx_buffer[0] = guc_com_rx_buffer[0];
    guc_com_tx_buffer[1] = guc_com_rx_buffer[1];
    guc_com_tx_buffer[2] = 4;  //字节个数4
    //--------------------------------------------------
    guc_com_tx_buffer[3] = (uint8_t)gss_TA;   //先传输低位
    guc_com_tx_buffer[4] = (uint8_t)(gss_TA>>8);     
    guc_com_tx_buffer[5] = guc_test_mode_code;         
    guc_com_tx_buffer[6] = 0;          

    //--------------------------------------------------
    com_crc.uword = CRC16(guc_com_tx_buffer, 7);
    guc_com_tx_buffer[7] = com_crc.ubyte.low;
    guc_com_tx_buffer[8] = com_crc.ubyte.high;
    
    guc_com_tx_point = 0;
    bflg_com_tx_busy = 1;
    TXD0 = guc_com_tx_buffer[guc_com_tx_point++];
}



/*******************************************************************************************************************************
函数功能：测试后备板上的输入输出口

函数位置：主循环-----------------------------------------ok
********************************************************************************************************************************/
void test_in_out_pin(void)
{
    if(TEST_CN6_IN_PIN == 1)
    {
        TEST_CN5_OUT_PIN = 1;
    }
    else
    {
        TEST_CN5_OUT_PIN = 0;
    }
    
    //-------------------------------------
    if(TEST_CN7_IN_PIN == 1)
    {
        TEST_BAT_OUT_PIN = 1;
    }
    else
    {
        TEST_BAT_OUT_PIN = 0;
    }
    
    //------------------------------------
    if(TEST_SW1_1_IN_PIN == 1)
    {
        TEST_LED1_OUT_PIN = 1;
    }
    else
    {
        TEST_LED1_OUT_PIN = 0;
    }
    
    //-------------------------------------
    if(TEST_SW1_2_IN_PIN == 1)
    {
        TEST_LED2_OUT_PIN = 1;
    }
    else
    {
        TEST_LED2_OUT_PIN = 0;
    }    
}


/*******************************************************************************************************************************
函数功能：测试三色灯

函数位置：1s定时器-----------------------------------------ok
********************************************************************************************************************************/
void test_mode_LED_deal(void)     
{


    if (TEST_BLUE_OUT_PIN == 1)    //蓝
    {
        TEST_BLUE_OUT_PIN = 0;     
        TEST_GREEN_OUT_PIN = 0;         
        TEST_RED_OUT_PIN = 1;
    }
    else if (TEST_RED_OUT_PIN == 1)  //红
    {
        TEST_BLUE_OUT_PIN = 0;     
        TEST_GREEN_OUT_PIN = 1;         
        TEST_RED_OUT_PIN = 0;

    }
    else if (TEST_GREEN_OUT_PIN == 1)  //绿
    {
        TEST_BLUE_OUT_PIN = 1;     
        TEST_GREEN_OUT_PIN = 0;         
        TEST_RED_OUT_PIN = 0;

    }
}


/*******************************************************************************************************************************
函数功能：故障代码的处理，需要传送给主板，然后主板再让显示板显示
          1、AD口：工装上将这个AD口固定接到5v处，所以判断检测到的在5v左右即可；
          2、E2检测
          
函数位置：主循环-----------------------------------------ok
********************************************************************************************************************************/
void test_error_code_deal(void)
{
    if(abs(50 - guc_bat_Vout) >= 10)  //检测的到的5v电压在 正负1v之外则认为故障
    {
        guc_test_mode_code = 1;
    }
    /*else if()                       //E2待+
    {
        guc_test_mode_code = 2;
    }*/
    else
    {
        guc_test_mode_code = 0;       //无故障
    }
}











/**************************************END OF THE FILE******************************************************************/
