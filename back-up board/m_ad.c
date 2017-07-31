#ifndef _AD_C_
#define _AD_C_
//------------------------------------------------------------------------------
//包含文件声明
#include "h_type_define.h"
#include "m_ad.h"
#include "m_ad_tab.h"

//------------------------------------------------------------------------------
//函数声明
void ad_val_deal(void);  //ad值处理程序，在AD转换程序中调用

void ad_temp_calc(void); //ad温度计算程序，在主循环程序中调用

int16_t get_temp3(uint16_t lus_ad);     //根据ad值查表得到温度值3

int16_t get_temp4(uint16_t lus_ad);     //根据ad值查表得到温度值4

//------------------------------------------------------------------------------
//变量定义
flag_type flg_adc;
//----------------------------------------------------------
#define   AD_SUM_INDEX   6    //求平均移位值
#define   AD_SUM_CNT     64   //求平均个数

#define   AD_VAL_MIN     5    //AD下限值
#define   AD_VAL_MAX     1000 //AD上限值

//----------------------------------------------------------
uint16_t  gus_ad_val;             //从 ADCR转换结果寄存器中获取的值
uint16_t  gus_ad_buffer[12];      //只在求瞬时电压时用过
uint32_t  gul_ad_sum_buffer[12];  //12个元素，没个元素(通道)都是64次累加
uint16_t  gus_ad_aver_buffer[12];
uint8_t   guc_ad_index;
uint8_t   guc_ad_cnt;
//----------------------------------------------------------
uint8_t   guc_ad_fault_cnt[12];         //ad故障次数计数器
uint8_t   guc_ad_fault_release_cnt[12]; //ad故障释放次数计数器

//----------------------------------------------------------
int16_t   gss_TA;   //主传感器           放大10倍，其他不变
int16_t   gss_THW;  //环温传感器
int16_t   gss_TC;   //冷凝器温度传感器
int16_t   gss_TE;   //热交换器温度传感器

int16_t   gss_VAC;  //电源电压
uint8_t   guc_12V;  //12V电压

int16_t   gss_Tbat1;     //锂电池温度1
int16_t   gss_Tbat2;     //锂电池温度2

uint8_t   guc_bat_Vin;   //电池充电电压
uint8_t   guc_bat_Vout;  //电池放电电压___放大10倍
uint8_t   guc_bat_Vout_instant;  //电池放电电压瞬时值


uint32_t  gul_bat_Vin;   //peak计算充电电流时使用
uint32_t  gul_bat_Vout;  //peak计算充电电流时使用
int16_t   gss_bat_I;     //电池充电电流

int16_t   gss_adg_Temp[10];
int16_t   gss_adg_Temp2;

uint8_t   guc_ad_calc_cnt;



/************************************************************************************************************************
函数功能:  1、获取每一路AD采集的值，通过多次采集取平均;
         2、判断是否有故障

函数位置: 1ms定时器ad_convert_deal()-----------------------------ok 
*************************************************************************************************************************/
void ad_val_deal(void)   //ad值处理程序，在AD转换程序中调用  peak 1ms中 
{
    //------------------------------------------------------
    //ad值累计
    gus_ad_buffer[guc_ad_index] = gus_ad_val;
    gul_ad_sum_buffer[guc_ad_index] += gus_ad_val;
    //------------------------------------------------------
    
    //ad值求平均
    if (guc_ad_cnt == 0)
    {
        gus_ad_aver_buffer[guc_ad_index] = (uint16_t) (gul_ad_sum_buffer[guc_ad_index] >> AD_SUM_INDEX);
        gul_ad_sum_buffer[guc_ad_index] = 0;
    }
    
    //------------------------------------------------------
    //ad故障判断与故障解除
    if ((gus_ad_val < AD_VAL_MIN) || (gus_ad_val > AD_VAL_MAX))
    {
        guc_ad_fault_release_cnt[guc_ad_index] = 0;
        //--------------------------------------------------
        guc_ad_fault_cnt[guc_ad_index]++;
        if (guc_ad_fault_cnt[guc_ad_index] >= 100)
        {
            guc_ad_fault_cnt[guc_ad_index] = 0;
            
            switch (guc_ad_index)
            {
                //+错误代码
                case 0:

                    break;
                case 1:

                    break;

                default:
                    break;
            }
        }
    }
    else
    {
        guc_ad_fault_cnt[guc_ad_index] = 0;
        //--------------------------------------------------
        guc_ad_fault_release_cnt[guc_ad_index]++;
        if (guc_ad_fault_release_cnt[guc_ad_index] >= 100)
        {
            guc_ad_fault_release_cnt[guc_ad_index] = 0;
            
            switch (guc_ad_index)
            {
                //+错误代码
                case 0:

                    break;
                case 1:

                    break;

                default:
                    break;
            }
        }
    }
    //------------------------------------------------------
    //索引累加
    guc_ad_index++;
    if (guc_ad_index >= 2)              //2个通道都采集完一遍 ad2~ad3
    {
        guc_ad_index = 0;
        //------------------------------
        if (guc_ad_cnt == 0)
        {
            bflg_allow_ad_calc = 1;     //置允许ad计算标志, 即置1时 ad_temp_calc() 才能执行；又清0；
        }
        //------------------------------
        guc_ad_cnt++;
        if (guc_ad_cnt >= AD_SUM_CNT)   //12路AD都采集完64次后才允许计算；
        {
            guc_ad_cnt = 0;
        }
    }
}

/************************************************************************************************************************
函数功能: 通过采集的AD值，转换为实际的温度、电压；

函数位置: 主循环中     --------------------------------ok
*************************************************************************************************************************/
void ad_temp_calc(void)  //ad温度计算程序，在主循环程序中调用
{
    int32_t lsl_tmp=0;
    
    gss_TA = get_temp3(gus_ad_aver_buffer[0]);     

    //电池放电电压计算（单位：0.1V）
    lsl_tmp = gus_ad_aver_buffer[1];
    //lsl_tmp *= 224;
    lsl_tmp *= 224;                   //放大10倍
    gul_bat_Vout = lsl_tmp;           //gul_bat_Vout只在计算电流时用,没有>>10，所以是放大了1024倍
    lsl_tmp >>= 10;
    guc_bat_Vout = (uint8_t) ((guc_bat_Vout * 3 + lsl_tmp) >> 2);

    
    guc_ad_calc_cnt++;              //判断采集是否错误时，刚开始为了稳定多采集几次再判断
    if (guc_ad_calc_cnt >= 5)
    {
    	  bflg_ad_calc_ok = 1;
    }
}


/*******************************************************************************************************
函数功能: 

函数位置: ad_temp_calc() 中；

获取内容: 获取PT100的温度值 
********************************************************************************************************/
int16_t get_temp3(uint16_t lus_ad)      //根据ad值查表得到温度值3
{
    int16_t lss_delt;
    
    int16_t lss_temp_index;
    lss_temp_index = (TEMP3_TAB_LENGTH >> 1);
    
    if (lus_ad > ad_to_temp3[lss_temp_index])
    {
        do
        {
            lss_temp_index--;
            if (lss_temp_index < 0)
            {
                lss_temp_index = 0;
                break;
            }
            //------------------------------------
            if (lus_ad < ad_to_temp3[lss_temp_index])
            {
                //lss_temp_index++;
                break;
            }
        }
        while (lus_ad > ad_to_temp3[lss_temp_index]);
    }
    else if (lus_ad < ad_to_temp3[lss_temp_index])
    {
        do
        {
            lss_temp_index++;
            if (lss_temp_index >= TEMP3_TAB_LENGTH)
            {
                lss_temp_index = TEMP3_TAB_LENGTH;
                break;
            }
            //------------------------------------
            if (lus_ad > ad_to_temp3[lss_temp_index])
            {
                lss_temp_index--;
                break;
            }
        }
        while (lus_ad < ad_to_temp3[lss_temp_index]);
    }
    //查表温度高1℃，在次减一个指针，温度降1℃
    /*lss_temp_index--;
    if (lss_temp_index < 0)
    {
        lss_temp_index = 0;
    }*/
    
    lss_delt = lus_ad;
    lss_delt -= ad_to_temp3[lss_temp_index];
    lss_delt *= 10;    //因为pt100在下面放大了10倍，所以此处求小数部分的值也要放大10倍
    lss_delt >>= 2;    //参见"血液"
    
    //return temp3_for_ad[lss_temp_index];
    return (temp3_for_ad[lss_temp_index] * 10 - lss_delt);
}
/*******************************************************************************************************
函数功能: 

函数位置: ad_temp_calc() 中；

获取内容: 获取电池的温度值 
********************************************************************************************************/
int16_t get_temp4(uint16_t lus_ad)      //根据ad值查表得到温度值4
{
    int16_t lss_temp_index;
    lss_temp_index = (TEMP4_TAB_LENGTH >> 1);
    
    if (lus_ad > ad_to_temp4[lss_temp_index])
    {
        do
        {
            lss_temp_index--;
            if (lss_temp_index < 0)
            {
                lss_temp_index = 0;
                break;
            }
            //------------------------------------
            if (lus_ad < ad_to_temp4[lss_temp_index])
            {
                //lss_temp_index++;
                break;
            }
        }
        while (lus_ad > ad_to_temp4[lss_temp_index]);
    }
    else if (lus_ad < ad_to_temp4[lss_temp_index])
    {
        do
        {
            lss_temp_index++;
            if (lss_temp_index >= TEMP4_TAB_LENGTH)
            {
                lss_temp_index = TEMP4_TAB_LENGTH;
                break;
            }
            //------------------------------------
            if (lus_ad > ad_to_temp4[lss_temp_index])
            {
                lss_temp_index--;
                break;
            }
        }
        while (lus_ad < ad_to_temp4[lss_temp_index]);
    }
    //查表温度高1℃，在次减一个指针，温度降1℃
    /*lss_temp_index--;
    if (lss_temp_index < 0)
    {
        lss_temp_index = 0;
    }*/
    
    return temp4_for_ad[lss_temp_index];
}
//------------------------------------------------------------------------------
#endif
