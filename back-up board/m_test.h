#ifndef _M_TEST_H_
#define _M_TEST_H_

//�ⲿ����
extern void test_mode_com();
extern void test_in_out_pin(void);
extern void test_mode_LED_deal(void);   
extern void test_error_code_deal(void);



extern flag_type flg_test;
       #define   bflg_test_mode     flg_test.bits.bit0


       //��ɫ��
#define TEST_RED_OUT_PIN    P0.1    //�� �ߵ�ƽ�����ͳ�����������෴�õ������ܲ�ͬ
#define TEST_BLUE_OUT_PIN   P0.0    //��
#define TEST_GREEN_OUT_PIN  P12.0   //��

















#endif
/**************************************END OF THE FILE******************************************************************/
