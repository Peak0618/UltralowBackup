#ifndef _M_E2_H_
#define _M_E2_H_

//�ⲿ����
extern void init_ram_para2(void);
extern void init_ram_para(void);
extern void eeprom2_read_deal(void)  ;
extern void eeprom_read_deal(void);   
extern void eeprom_write(uint16_t lus_addr, int16_t lss_eep_data);    
extern int16_t eeprom_read(uint16_t lus_addr);  


extern flag_type flg_e2;
          #define   bflg_askfor_read_eeprom  flg_e2.bits.bit0    //�����eeprom��־
          #define   bflg_read_eeprom_error   flg_e2.bits.bit1    //��eeprom���־
          #define   bflg_eeprom_fault        flg_e2.bits.bit2    //eeprom���ϱ�־
          
          












#endif
/***************************************END OF THE FILE******************************************************************************************/
