/************************************************************************************************************************************
overview:  1����e2�Ĳ���
           
*************************************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_wdt.h"

#include "h_type_define.h"
#include "m_rom_para2.h"
#include "m_e2.h"

//��������
void init_ram_para2(void);
void init_ram_para(void);
void eeprom2_read_deal(void)  ;
void eeprom_read_deal(void);   
void delay_10us(uint16_t _10us);
void delay_1ms(uint16_t _1ms);      
void eeprom_write(uint16_t lus_addr, int16_t lss_eep_data);    
int16_t eeprom_read(uint16_t lus_addr);  

//��������
flag_type flg_e2;

//------------------------------------------------------------------------------
void init_ram_para2(void)
{
    uint8_t i;
    for (i = 0; i < MAX_PARA_NUMBER2; i++)
    {
        //ram_para2[i] = EEPROM_init2[i][0];
        eeprom_write(i + 256, EEPROM_init2[i][0]);
    }
}
//------------------------------------------------------------------------------
void init_ram_para(void)
{
    uint8_t i;
    for (i = 0; i < MAX_PARA_NUMBER; i++)
    {
        //ram_para[i] = EEPROM_init[i][0];
        eeprom_write(i, EEPROM_init[i][0]);
    }
}
//------------------------------------------------------------------------------
void eeprom2_read_deal(void)  //eeprom2��ȡ��������ڳ�ʼ�������е���
{
    int16_t i;
    uint8_t luc_read_para_cnt;
    int16_t lss_verify_word;
    
    do
    {
        bflg_askfor_read_eeprom = 0;    //�������EEP��־
        //----------------------------------------
        for (i = 0; i < MAX_PARA_NUMBER2; i++)
        {
            ram_para2[i] = eeprom_read(i + 256); //��EEP��������RAM��
            R_WDT_Restart();
            
            if ((ram_para2[i] < EEPROM_init2[i][min_row])
               || (ram_para2[i] > EEPROM_init2[i][max_row]))
            {
                bflg_read_eeprom_error = 1;  //�ö�EEPROM���־
                break;
            }
        }
        //--------------------------------------------------
        if (bflg_read_eeprom_error == 1)     //����ж�EEPROM���־
        {
            bflg_read_eeprom_error = 0;      //���EEPROM���־
            
            luc_read_para_cnt++;   //��EEP������1
            if (luc_read_para_cnt >= 3)      //�����3�δ�
            {
                //EEPROM���ϴ���
                bflg_eeprom_fault = 1;  //��EEP���ϱ�־
                break;
            }
            else
            {
                bflg_askfor_read_eeprom = 1; //�������EEPROM��־
            }
        }
    }
    while (bflg_askfor_read_eeprom == 1);
}
//------------------------------------------------------------------------------
void eeprom_read_deal(void)   //eeprom��ȡ��������ڳ�ʼ�������е���
{
    int16_t i;
    uint8_t luc_read_para_cnt;
    int16_t lss_verify_word;
    
    do
    {
        bflg_askfor_read_eeprom = 0;    //�������EEP��־
        //----------------------------------------
        for (i = 0; i < MAX_PARA_NUMBER; i++)
        {
            ram_para[i] = eeprom_read(i);     //��EEP��������RAM��
            R_WDT_Restart();
            
            if ((ram_para[i] < EEPROM_init[i][min_row])
               || (ram_para[i] > EEPROM_init[i][max_row]))
            {
                bflg_read_eeprom_error = 1;  //�ö�EEPROM���־
                break;
            }
        }
        //--------------------------------------------------
        if (bflg_read_eeprom_error == 1)     //����ж�EEPROM���־
        {
            bflg_read_eeprom_error = 0;      //���EEPROM���־
            
            luc_read_para_cnt++;   //��EEP������1
            if (luc_read_para_cnt >= 3)      //�����3�δ�
            {
                //EEPROM���ϴ���
                bflg_eeprom_fault = 1;  //��EEP���ϱ�־
                break;
            }
            else
            {
                bflg_askfor_read_eeprom = 1; //�������EEPROM��־
            }
        }
    }
    while (bflg_askfor_read_eeprom == 1);
}
//------------------------------------------------------------------------------
void delay_10us(uint16_t _10us)    //10us��ʱ������eeprom��д�����е���
{
    uint16_t i,j;
    for (i = 0; i < (_10us * 2); i++)
    {
        j = i * 3;
        R_WDT_Restart();
    }
}
//------------------------------------------------------------------------------
void delay_1ms(uint16_t _1ms)      //1ms��ʱ������eeprom��д�����е���
{
    uint16_t i,j;
    for (i = 0; i < (_1ms * 159); i++)
    {
        j = i * 3;
        R_WDT_Restart();
    }
}
//------------------------------------------------------------------------------
void eeprom_write(uint16_t lus_addr, int16_t lss_eep_data)    //дeeprom����
{
    uint8_t luc_page_addr;
    uint8_t luc_unit_addr;
    uint8_t luc_driver_write;
    
    uint8_t luc_eep_data_H;
    uint8_t luc_eep_data_L;
    
    luc_page_addr = (uint8_t) (lus_addr * 2 / 256);
    luc_unit_addr = (uint8_t) (lus_addr * 2 % 256);
    luc_driver_write = (uint8_t) (( luc_page_addr << 1) & 0x0E | 0xA0);
    
    luc_eep_data_H = (uint8_t) (((uint16_t) lss_eep_data) / 256);
    luc_eep_data_L = (uint8_t) (((uint16_t) lss_eep_data) % 256);
    //------------------------------------------------------
    R_WDT_Restart();
    while (IICBSY0 == 1) {}        //����æ����ȴ�
    
    STT0 = 1;                      //����start�ź�
    WREL0 = 0;                     //����ģʽ
    ACKE0 = 0U;                    //��ACK�ź�
    IICA0 = luc_driver_write;      //д������ַ
    delay_10us(20);                //�ȴ�
    
    IICA0 = luc_unit_addr;         //��Ԫ��ַ
    delay_10us(20);                //�ȴ�
    
    IICA0 = luc_eep_data_H;            
    delay_10us(20);                //�ȴ�
    SPT0 = 1;                      //����stop�ź�
    //------------------------------------------------------
    delay_1ms(3);
    //------------------------------------------------------
    R_WDT_Restart();
    while (IICBSY0 == 1){}         //����æ����ȴ�
    
    STT0 = 1;                      //����start�ź�
    WREL0 = 0;                     //����ģʽ
    ACKE0 = 0U;                    //��ACK�ź�
    IICA0 = luc_driver_write;      //д������ַ
    delay_10us(20);                //�ȴ�
    
    IICA0 = (uint8_t)(luc_unit_addr + 1);     //��Ԫ��ַ
    delay_10us(20);                //�ȴ�
    
    IICA0 = luc_eep_data_L;
    delay_10us(20);                //�ȴ�
    SPT0 = 1;                      //����stop�ź�
    //------------------------------------------------------
    delay_1ms(3);
}
//------------------------------------------------------------------------------
int16_t eeprom_read(uint16_t lus_addr)  //��eeprom����
{
    uint8_t luc_page_addr;
    uint8_t luc_unit_addr;
    uint8_t luc_driver_write;
    uint8_t luc_driver_read;
    uint8_t luc_eep_data_H;
    uint8_t luc_eep_data_L;
    
    luc_page_addr = (uint8_t) (lus_addr * 2 / 256);
    luc_unit_addr = (uint8_t) (lus_addr * 2 % 256);
    luc_driver_write = (uint8_t) (( luc_page_addr << 1) & 0x0E | 0xA0);
    luc_driver_read = (uint8_t) (( luc_page_addr << 1) & 0x0E | 0xA1);
    //------------------------------------------------------
    R_WDT_Restart();
    while (IICBSY0 == 1) {}        //����æ����ȴ�
    
    STT0 = 1;                      //����start�ź�
    WREL0 = 0;                     //����ģʽ
    ACKE0 = 0U;                    //��ACK�ź�
    IICA0 = luc_driver_write;      //д������ַ
    delay_10us(20);                //�ȴ�
    
    IICA0 = luc_unit_addr;         //��Ԫ��ַ
    delay_10us(20);                //�ȴ�
    
    STT0 = 1;                      //����start�ź�
    IICA0 = luc_driver_read;       //��������ַ    
    delay_10us(20);                //�ȴ�
    WREL0 = 1;                     //����ģʽ
    
    delay_10us(20);                //�ȴ�
    luc_eep_data_H = IICA0;        //�õ�����
    ACKE0 = 1;                     //����ACK�ź�
    SPT0 = 1;                      //����stop�ź�
    //------------------------------------------------------
    delay_1ms(3);
    //------------------------------------------------------
    R_WDT_Restart();
    while (IICBSY0 == 1){}         //����æ����ȴ�
    
    STT0 = 1;                      //����start�ź�
    WREL0 = 0;                     //����ģʽ
    ACKE0 = 0U;                    //��ACK�ź�
    IICA0 = luc_driver_write;      //д������ַ
    delay_10us(20);                //�ȴ�
    
    IICA0 = (uint8_t)(luc_unit_addr + 1);     //��Ԫ��ַ
    delay_10us(20);                //�ȴ�
    
    STT0 = 1;                      //����start�ź�
    IICA0 = luc_driver_read;       //��������ַ
    delay_10us(20);                //�ȴ�
    WREL0 = 1;                     //����ģʽ
    
    delay_10us(20);                //�ȴ�
    luc_eep_data_L = IICA0;        //������
    ACKE0 = 1;                     //����ACK�ź�
    SPT0 = 1;                      //����stop�ź�
    //------------------------------------------------------
    delay_1ms(3);
    //------------------------------------------------------
    return ((int16_t) (luc_eep_data_H) * 256 + luc_eep_data_L);
}







/*****************************************END OF THE FILE*********************************************************************************************************************/
