#ifndef _M_PERIPHERAL_CONTROL_H_
#define _M_PERIPHERAL_CONTROL_H_


//�ⲿ����
extern void key_buzz_delaytime(void);


extern flag_type flg_peripheral;
        #define bflg_key_buzz_delaytime  flg_peripheral.bits.bit0   //��������һ��




extern  int16_t   gss_key_buzz_delaytimer;    // ��������һ����ʱ
















#endif
