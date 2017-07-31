/**********************************************************************************************************************
overview:    1、蜂鸣器
************************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_pclbuz.h"

#include "h_type_define.h"
#include "m_peripheral_control.h"

//函数声明
void key_buzz_delaytime(void);


//标志
flag_type flg_peripheral;


//变量
int16_t   gss_key_buzz_delaytimer;    // 蜂鸣器响一声延时


/*************************************************************************************************************************************************************
函数功能：蜂鸣器响延时

使用时：      bflg_key_buzz_delaytime = 1;
          gss_key_buzz_delaytimer = 0;

函数位置：10ms定时中断----------------------------------ok
**************************************************************************************************************************************************************/
void key_buzz_delaytime(void) //按键蜂鸣延时程序，在10ms多少程序中调用
{
    if (bflg_key_buzz_delaytime == 1)
    {
        gss_key_buzz_delaytimer++;
        if (gss_key_buzz_delaytimer >= 20)
        {
            gss_key_buzz_delaytimer = 0;
            bflg_key_buzz_delaytime = 0;
            
            R_PCLBUZ0_Stop();
        }
    }
}















