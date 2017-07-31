/************************************************************************************************************************
overview: �Ժ󱸰���й�װ���ʱ����
          1�����巢�ض�������ָ��ἤ��󱸰�������ģʽ
          2���ڲ��Գ�������
*************************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_timer.h"
#include "r_cg_serial.h"

#include "h_type_define.h"
#include "m_com.h"
#include "m_ad.h"
#include "m_test.h"
#include <stdlib.h>


//��������
void test_mode_com();
void test_com_rx_data_deal(void);   
void test_com_tx_init(void);
void test_in_out_pin(void);
void test_mode_LED_deal(void);     
void test_error_code_deal(void);

//����
flag_type flg_test;


uint8_t guc_test_mode_code;     //�������

//�궨�� 
//����
#define TEST_CN6_IN_PIN     P3.0   //CN6�˿�������
#define TEST_CN7_IN_PIN     P5.0   //CN7�˿�������
#define TEST_SW1_1_IN_PIN   P1.7   //���뿪��1
#define TEST_SW1_2_IN_PIN   P1.6   //���뿪��2

//���
#define TEST_LED1_OUT_PIN   P1.3    //LED1
#define TEST_LED2_OUT_PIN   P1.4    //LED2
#define TEST_CN5_OUT_PIN    P5.1    //���ƶ˿�CN5�̵���
#define TEST_BAT_OUT_PIN    P14.7   //���Ƶ�ؿ��ص�����



/*******************************************************************************************************************************
�������ܣ�����ģʽ�½��е�ͨѶ����

����λ�ã���ѭ��-----------------------------------------ok
********************************************************************************************************************************/
void test_mode_com(void)
{
    if (bflg_com_allow_rx == 1)  //����������
    {
        bflg_com_allow_rx = 0;
        
        com_rx_init();   
        COM_RX_MODE;
        R_UART0_Start();
    }
    if (bflg_com_rx_end == 1)    //������ս���
    {
        bflg_com_rx_end = 0;
        
        R_UART0_Stop();
    }
    if (bflg_com_rx_ok == 1)     //������ճɹ�
    {
        bflg_com_rx_ok = 0;
        
        R_UART0_Stop();
        test_com_rx_data_deal();   
    }
    if (bflg_com_allow_tx == 1)  //���������
    {
        bflg_com_allow_tx = 0;
        
        R_UART0_Start();
        COM_TX_MODE;
        test_com_tx_init();   
    }
    if (bflg_com_tx_ok == 1)     //������ͳɹ�
    {
        bflg_com_tx_ok = 0;      
        
        R_UART0_Stop();
    }
}

/*******************************************************************************************************************************
�������ܣ�����ģʽ�¶Խ��յ����ݴ���

����λ�ã���ѭ�� test_mode_com()-----------------------------------------ok
********************************************************************************************************************************/
void test_com_rx_data_deal(void)   
{
    uint8_t i, msb_byte = 0,lsb_byte = 0;
    uint16_t lus_tmp;
    word_type com_crc;
    //------------------------------------------------------
    if ((guc_com_rx_buffer[1] == 0xaa)) //����ǲ�ѯ����
    {
        com_crc.uword = CRC16(guc_com_rx_buffer, 6);
        if ((com_crc.ubyte.low == guc_com_rx_buffer[6])
         && (com_crc.ubyte.high == guc_com_rx_buffer[7]))
        {
            bflg_com_tx_delaytime = 1;       //�÷�����ʱ��־
            gss_com_tx_delaytimer = 10;      //������ʱ��ʱ����ֵ
            
            bflg_com_rx_delaytime = 0;       //�巢����ʱ��־
            gss_com_rx_delaytimer = 0;       //������ʱ��ʱ������
            
            //bflg_com_fault = 0;              //��ͨѶ���ϱ�־
            //gss_com_fault_delaytimer = 0;    //��ͨѶ���ϼ�ʱ��
        }
    }  
}

/*******************************************************************************************************************************
�������ܣ�����ģʽ�³�ʼ�����͵�����

����λ�ã���ѭ�� test_mode_com()-----------------------------------------ok
********************************************************************************************************************************/
void test_com_tx_init(void)
{
    word_type com_crc;      
    uint8_t  msb_byte = 0, lsb_byte = 0;

    guc_com_tx_buffer[0] = guc_com_rx_buffer[0];
    guc_com_tx_buffer[1] = guc_com_rx_buffer[1];
    guc_com_tx_buffer[2] = 4;  //�ֽڸ���4
    //--------------------------------------------------
    guc_com_tx_buffer[3] = (uint8_t)gss_TA;   //�ȴ����λ
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
�������ܣ����Ժ󱸰��ϵ����������

����λ�ã���ѭ��-----------------------------------------ok
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
�������ܣ�������ɫ��

����λ�ã�1s��ʱ��-----------------------------------------ok
********************************************************************************************************************************/
void test_mode_LED_deal(void)     
{


    if (TEST_BLUE_OUT_PIN == 1)    //��
    {
        TEST_BLUE_OUT_PIN = 0;     
        TEST_GREEN_OUT_PIN = 0;         
        TEST_RED_OUT_PIN = 1;
    }
    else if (TEST_RED_OUT_PIN == 1)  //��
    {
        TEST_BLUE_OUT_PIN = 0;     
        TEST_GREEN_OUT_PIN = 1;         
        TEST_RED_OUT_PIN = 0;

    }
    else if (TEST_GREEN_OUT_PIN == 1)  //��
    {
        TEST_BLUE_OUT_PIN = 1;     
        TEST_GREEN_OUT_PIN = 0;         
        TEST_RED_OUT_PIN = 0;

    }
}


/*******************************************************************************************************************************
�������ܣ����ϴ���Ĵ�����Ҫ���͸����壬Ȼ������������ʾ����ʾ
          1��AD�ڣ���װ�Ͻ����AD�ڹ̶��ӵ�5v���������жϼ�⵽����5v���Ҽ��ɣ�
          2��E2���
          
����λ�ã���ѭ��-----------------------------------------ok
********************************************************************************************************************************/
void test_error_code_deal(void)
{
    if(abs(50 - guc_bat_Vout) >= 10)  //���ĵ���5v��ѹ�� ����1v֮������Ϊ����
    {
        guc_test_mode_code = 1;
    }
    /*else if()                       //E2��+
    {
        guc_test_mode_code = 2;
    }*/
    else
    {
        guc_test_mode_code = 0;       //�޹���
    }
}











/**************************************END OF THE FILE******************************************************************/
