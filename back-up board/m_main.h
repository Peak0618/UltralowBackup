#ifndef _M_MAIN_H_
#define _M_MAIN_H_

//�ⲿ����
extern void system_init(void);   
extern void system_start(void);
extern void main_loop(void);    
extern void timer_op(void); 
extern void timer_int(void);    


//�ⲿ��������//
extern flag_type flg_time;
          
          #define   bflg_1ms_reach      flg_time.bits.bit0  //1ms��ʱ����־
          #define   bflg_5ms_reach      flg_time.bits.bit1  //5ms��ʱ����׼
          #define   bflg_10ms_reach     flg_time.bits.bit2  //10ms��ʱ����׼



#endif 
/**************************************END OF THE FILE******************************************************************/
