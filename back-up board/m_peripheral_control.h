#ifndef _M_PERIPHERAL_CONTROL_H_
#define _M_PERIPHERAL_CONTROL_H_


//外部函数
extern void key_buzz_delaytime(void);


extern flag_type flg_peripheral;
        #define bflg_key_buzz_delaytime  flg_peripheral.bits.bit0   //蜂鸣器响一声




extern  int16_t   gss_key_buzz_delaytimer;    // 蜂鸣器响一声延时
















#endif
