#ifndef _M_COM_H_
#define _M_COM_H_
//�ⲿ��������//

extern void com_init(void);   //ͨѶ��ʼ�������ڳ�ʼ�������е���

extern void com_rx_init(void);     //ͨѶ���ճ�ʼ����������ѭ�������е���

extern void com_rx_data_deal(void);     //ͨѶ�������ݴ����������ѭ�������е���

extern void com_tx_init(void);     //ͨѶ�������ݴ����������ѭ�������е���

extern void com_rx_delaytime(void);     //ͨѶ������ʱ������1ms��ʱ�����е���

extern void com_tx_delaytime(void);     //ͨѶ������ʱ������1ms��ʱ�����е���

extern void com_rx_end_delaytime(void); //ͨѶ���ս�����ʱ������1ms��ʱ�����е���

extern void com_fault_delaytime(void);  //ͨѶ������ʱ������1s��ʱ�����е���

extern void com_rx_int(uint8_t rx_data);     //ͨѶ�����жϳ���

extern void com_tx_int(void); //ͨѶ�����жϳ���

extern unsigned int CRC16(unsigned char *puchmsg, unsigned int usDataLen);    

//------------------------------------------------------------------------------
//�ⲿ��������//
extern flag_type flg_com;
          #define   bflg_com_tx_delaytime    flg_com.bits.bit0   //ͨѶ������ʱ��־
          #define   bflg_com_rx_delaytime    flg_com.bits.bit1   //ͨѶ������ʱ��־
          #define   bflg_com_allow_tx        flg_com.bits.bit2   //ͨѶ�����ͱ�־
          #define   bflg_com_allow_rx        flg_com.bits.bit3   //ͨѶ������ձ�־
          #define   bflg_com_tx_busy         flg_com.bits.bit4   //ͨѶ����æ��־
          #define   bflg_com_rx_busy         flg_com.bits.bit5   //ͨѶ����æ��־
          #define   bflg_com_tx_ok           flg_com.bits.bit6   //ͨѶ���ͳɹ���־
          #define   bflg_com_rx_ok           flg_com.bits.bit7   //ͨѶ���ճɹ���־
          #define   bflg_com_rx_end          flg_com.bits.bit8   //ͨѶ���ս�����־
          
          #define   bflg_com_fault           flg_com.bits.bit9   //ͨѶ���ϱ�־
          
          #define   bflg_read_write_data     flg_com.bits.bit10  //��д���ݱ�־

          
//------------------------------------------------------------------------------
#define   LED_ADDR  0x00
#define   KEY_ADDR  0x42

#define   LED_BUF_LEN    13
#define   KEY_BUF_LEN    4

#define   COM_RX_MODE    (P1.0 = 0)    //����ģʽ
#define   COM_TX_MODE    (P1.0 = 1)    //����ģʽ


extern uint8_t guc_led_buffer[LED_BUF_LEN];  //����ܼĴ���
extern uint8_t guc_key_buffer[KEY_BUF_LEN];  //�����Ĵ���

extern uint8_t guc_key_val[KEY_BUF_LEN];     //����ֵ
extern uint8_t guc_com_tx_buffer[9];  //ͨѶ���ͻ�����
extern uint8_t guc_com_rx_buffer[53];  //ͨѶ���ջ�����  //23

extern int16_t gss_com_tx_delaytimer;     //ͨѶ������ʱ��ʱ��
extern int16_t gss_com_rx_delaytimer;     //ͨѶ������ʱ��ʱ��
extern uint8_t guc_com_tx_point;          //ͨѶ��������

#endif
