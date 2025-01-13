/******************************************************************************
;       Program  : Mod_TemperatureProtection_Ctrl.c
;       SWDD_ID  : SWDD_xxx
;       Project  : C120UAA01.0
;       Chip     : NXP S32K144
;       Date     : 2020 / 11 / 013
;       Author   : 
;       Describe : 
;
;
;
******************************************************************************/
/*---------------------------- Include File ---------------------------------*/
/* include global */
#include "gPinDef.h"
#include "UartApp.h"
/* include module */
#include "Mod_TemperatureProtection_Ctrl.h"
#include "Mod_DataManagement.h"

/* include APP */
#include "TestPattern.h"
#include "PWMApp.h"


/*MTPC_BacklightNtcAdcValue_Average_API*/
//static uint8_t u8Init_flag = DISABLE;
//static uint16_t u8PCBTempBuffIndex = NUM_ZERO;
//static uint16_t u16PCBTempBuffer[u8PCBTempBuffer_size] = {NUM_ZERO,NUM_ZERO,NUM_ZERO,NUM_ZERO,NUM_ZERO,NUM_ZERO,NUM_ZERO,NUM_ZERO};
//static uint16_t u16BacklightNtcAdcValue_sum = NUM_ZERO;
/*MTPC_BacklightNtcAdcValue_lookuptable_API*/
uint16_t u16PCBNtcAdcTable_RawData[166] =
{1522U, 1528U, 1534U, 1540U, 1547U, 1553U, 1559U, 1565U, 1571U, 1578U, 
1584U, 1590U, 1596U, 1603U, 1609U, 1615U, 1622U, 1628U, 1635U, 1641U, 
1647U, 1654U, 1661U, 1667U, 1673U, 1680U, 1686U, 1693U, 1699U, 1706U, 
1712U, 1719U, 1726U, 1732U, 1739U, 1745U, 1752U, 1758U, 1765U, 1772U, 
1778U, 1785U, 1791U, 1798U, 1804U, 1811U, 1818U, 1824U, 1831U, 1837U, 
1844U, 1850U, 1857U, 1864U, 1870U, 1877U, 1883U, 1890U, 1896U, 1903U, 
1909U, 1916U, 1922U, 1929U, 1935U, 1941U, 1948U, 1954U, 1961U, 1967U, 
1974U, 1980U, 1986U, 1993U, 1999U, 2005U, 2012U, 2018U, 2024U, 2031U, 
2037U, 2043U, 2050U, 2056U, 2062U, 2068U, 2075U, 2081U, 2087U, 2093U, 
2099U, 2106U, 2112U, 2118U, 2124U, 2130U, 2136U, 2142U, 2148U, 2154U, 
2160U, 2166U, 2172U, 2179U, 2184U, 2190U, 2196U, 2202U, 2208U, 2214U, 
2220U, 2226U, 2232U, 2238U, 2244U, 2250U, 2255U, 2261U, 2267U, 2273U, 
2279U, 2285U, 2290U, 2296U, 2302U, 2308U, 2313U, 2319U, 2325U, 2330U, 
2336U, 2342U, 2347U, 2353U, 2359U, 2364U, 2370U, 2376U, 2381U, 2387U, 
2392U, 2398U, 2403U, 2409U, 2414U, 2420U, 2425U, 2431U, 2436U, 2442U, 
2447U, 2453U, 2458U, 2464U, 2469U, 2474U, 2480U, 2485U, 2491U, 2496U, 
2501U, 2507U, 2512U, 2517U, 2523U, 2528U} ;

uint16_t u16PCB984NtcAdcTable_RawData[166] =
{1614U, 1621U, 1627U, 1633U, 1639U, 1645U, 1652U, 1658U, 1665U, 1671U, 
1677U, 1684U, 1690U, 1697U, 1703U, 1709U, 1716U, 1723U, 1729U, 1736U, 
1742U, 1749U, 1755U, 1762U, 1769U, 1775U, 1782U, 1788U, 1795U, 1802U, 
1808U, 1815U, 1821U, 1828U, 1835U, 1841U, 1848U, 1855U, 1861U, 1868U, 
1875U, 1881U, 1888U, 1895U, 1901U, 1908U, 1914U, 1921U, 1928U, 1934U, 
1941U, 1948U, 1954U, 1961U, 1967U, 1974U, 1980U, 1987U, 1994U, 2000U, 
2006U, 2013U, 2020U, 2026U, 2032U, 2039U, 2045U, 2052U, 2058U, 2065U, 
2071U, 2078U, 2084U, 2090U, 2097U, 2103U, 2109U, 2116U, 2122U, 2128U, 
2135U, 2141U, 2147U, 2153U, 2160U, 2166U, 2172U, 2178U, 2184U, 2191U, 
2197U, 2203U, 2209U, 2215U, 2221U, 2227U, 2233U, 2239U, 2245U, 2251U, 
2257U, 2263U, 2269U, 2275U, 2281U, 2287U, 2293U, 2299U, 2305U, 2311U, 
2317U, 2322U, 2328U, 2334U, 2340U, 2346U, 2351U, 2357U, 2363U, 2369U, 
2374U, 2380U, 2386U, 2392U, 2397U, 2403U, 2409U, 2414U, 2420U, 2425U, 
2431U, 2437U, 2442U, 2448U, 2453U, 2459U, 2464U, 2470U, 2475U, 2481U, 
2486U, 2492U, 2497U, 2503U, 2508U, 2513U, 2519U, 2524U, 2530U, 2535U, 
2540U, 2546U, 2551U, 2556U, 2562U, 2567U, 2572U, 2577U, 2583U, 2588U, 
2593U, 2598U, 2604U, 2609U, 2614U, 2619U } ;


int16_t i8PCBNtcAdcTable_Temp[166] =
{-40, -39, -38, -37, -36, -35,-34, -33, -32, -31,
-30, -29, -28, -27, -26, -25, -24, -23, -22, -21,
-20, -19,  -18, -17, -16, -15, -14, -13, -12, -11,
-10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 
0,   1, 2, 3, 4, 5, 6, 7, 8, 9, 
10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 
40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 
50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
70,  71, 72, 73, 74, 75, 76, 77, 78, 79,
80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 
90,  91, 92, 93, 94, 95, 96, 97, 98, 99, 
100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 
110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
120, 121, 122, 123, 124, 125 } ;

/*Backlight_derating_process*/
const uint8_t  u8GF_Value = 6;
const uint16_t u16TEMP_NTC_0 = NUM_ZERO;
const uint16_t u16TEMP_NTC_74 = 740U;           /* 750 -> 740 20220218 Joel */
const uint16_t u16TEMP_NTC_82 = 820U;           /* 850 -> 820 20220218 Joel */
const uint16_t u16TEMP_DQA_RESERVED = 60U;
const uint16_t u16TEMP_NTC_Offset_5 = 50U;      /* 20220218 Joel */
const uint16_t u16TEMP_NTC_Offset_8 = 80U;      /* 20220218 Joel */
const uint16_t u16TEMP_NTC_Offset_13 = 130U;    /* 20220218 Joel */
const uint16_t u16TEMP_NTC_Offset_20 = 200U;    /* 20220520 Leo  */
const uint16_t u16TEMP_NTC_Offset_16 = 160U;    /* 20220617 Leo  */
const uint16_t u16Duty_full = 1000U; //100PA * 10
const uint16_t u16Duty_52PA = 520U; //52PA * 10
const uint16_t u16Duty10Bit_17PA4 = 178U;   //1023*17PA4   /* 20220218 Joel */
const uint16_t u16Duty10Bit_30PA = 307U;    //1023*31PA    /* 20220218 Joel */
const uint16_t u16Duty10Bit_34PA = 340U;    //34*10        /* 20220616 Leo  */
const uint16_t u16Duty_17PA4 = 174U; //17.4PA * 10
const uint16_t u16Duty_end = NUM_ZERO;

const uint8_t u8AlphaParameter = 15;  //2^4 - 1
const uint8_t u8Devider = 4;

static	uint16_t u16backlightCounter = NUM_ZERO;
static	uint8_t	 u8backlightprocess_initial = NUM_ZERO;
static  uint16_t  u16BacklightDutyCycle = NUM_ZERO;
static	uint8_t u8BacklightStatus = NUM_ZERO;
static	int16_t i16BacklightStatus_Derating_temprecord = NUM_ZERO;
static	uint8_t u8BacklightStatus_Derating_mode = NUM_ZERO;

/*MTPC_TemperatureProtection_Ctrl*/
static int16_t i16BacklightTemperature = NUM_ZERO;
//static uint16_t u16BacklightNtcAdcValue_Average = NUM_ZERO;
// uint16_t u16BacklightUserDuty_Get;
/*------------------------ Declare External Var -----------------------------*/
static Mod_TemPro_Ctrl_TableTypedef gtsTempForm_ModTemPro;


/******************************************************************************
;       Function Name			:	
;       Function Description	:	
;       Parameters				:	
;       Return Values			:	
;		Source ID				:	SWE3_xxx_xx
******************************************************************************/
uint16_t MTPC_BacklightNtcAdcValue_Average_API_L(uint16_t u16BacklightNtcAdcValue)
{
    
    static uint32_t u32BacklightNtcAdcValue_Average= NUM_ZERO;
    uint16_t u16Result;
    
    
    u32BacklightNtcAdcValue_Average = u8AlphaParameter * u32BacklightNtcAdcValue_Average + u16BacklightNtcAdcValue;
    u32BacklightNtcAdcValue_Average = u32BacklightNtcAdcValue_Average >> u8Devider;
    
    
    u16Result = u32BacklightNtcAdcValue_Average;
    
    
#if 0        
    if(u8GlitchFlag == 1)
    {  
        if(((u16BacklightNtcAdcValue - u16BacklightNtcAdcValue_Average)) > u8GF_Value || ((u16BacklightNtcAdcValue_Average - u16BacklightNtcAdcValue)) > u8GF_Value )
        {
            ;
        }
        else
        {              
            u16PCBTempBuffer[u8PCBTempBuffIndex]= u16BacklightNtcAdcValue; /*Get ADC value on present*/
            
            for(u8Counter = 0; u8Counter < u8PCBTempBuffer_size; u8Counter++)
            {
                u32Sum += u16PCBTempBuffer[u8Counter];
            }
            
            u16BacklightNtcAdcValue_Average =  u32Sum >> 5U;
            
            u8PCBTempBuffIndex++;
            
            if (u8PCBTempBuffIndex==u8PCBTempBuffer_size) /*reset index*/
            {
                u8PCBTempBuffIndex=Reset_Index;
            }
            
        }
        
    }
    else
    {
        u16PCBTempBuffer[u8PCBTempBuffIndex]= u16BacklightNtcAdcValue;/*Get ADC value on present*/
        
        for(u8Counter = 0; u8Counter < u8PCBTempBuffer_size; u8Counter++)
        {
            u32Sum += u16PCBTempBuffer[u8Counter];
        }
        
        u16BacklightNtcAdcValue_Average =  u32Sum >> 5U;
        
        u8PCBTempBuffIndex++;
        
        if (u8PCBTempBuffIndex==u8PCBTempBuffer_size) /*reset index*/
        {
            u8PCBTempBuffIndex=Reset_Index;
        }
        
        if (u8PCBTempBuffIndex ==  u8PCBTempBuffer_size - 1)
        {
            u8GlitchFlag == 1;
        }
    }
#endif
    
	return u16Result;	
} 


uint16_t MTPC_BacklightNtcAdcValue_Average_API_R(uint16_t u16BacklightNtcAdcValue)
{
    
	//static uint16_t u16BacklightNtcAdcValue_Average= NUM_ZERO;
    static uint32_t u32BacklightNtcAdcValue_Average= NUM_ZERO;
    //static uint32_t u32BacklightNtcAdcValue_Average_2nd= NUM_ZERO;
    
    
    
    uint16_t u16Result;
#if 0
    uint8_t u8Counter;
    uint32_t u32Sum = 0U;
    uint8_t u8GlitchFlag = 0U;
#endif
    
    u32BacklightNtcAdcValue_Average = u8AlphaParameter * u32BacklightNtcAdcValue_Average + u16BacklightNtcAdcValue;
    u32BacklightNtcAdcValue_Average = u32BacklightNtcAdcValue_Average >> u8Devider;
    
    //u32BacklightNtcAdcValue_Average_2nd = 511 * u32BacklightNtcAdcValue_Average_2nd + u32BacklightNtcAdcValue_Average;        
    //u32BacklightNtcAdcValue_Average_2nd = u32BacklightNtcAdcValue_Average_2nd >> 9;
    //u16Result = u32BacklightNtcAdcValue_Average_2nd;
    
    u16Result = u32BacklightNtcAdcValue_Average;
	return u16Result;	
} 

uint16_t MTPC_PCBNtcAdcValue_Average_API(uint16_t u16BacklightNtcAdcValue)
{
    
	//static uint16_t u16BacklightNtcAdcValue_Average= NUM_ZERO;
    static uint32_t u32BacklightNtcAdcValue_Average= NUM_ZERO;
    //static uint32_t u32BacklightNtcAdcValue_Average_2nd= NUM_ZERO;
    
    
    
    uint16_t u16Result;
#if 0
    uint8_t u8Counter;
    uint32_t u32Sum = 0U;
    uint8_t u8GlitchFlag = 0U;
#endif
    
    u32BacklightNtcAdcValue_Average = u8AlphaParameter * u32BacklightNtcAdcValue_Average + u16BacklightNtcAdcValue;
    u32BacklightNtcAdcValue_Average = u32BacklightNtcAdcValue_Average >> u8Devider;
    
    //u32BacklightNtcAdcValue_Average_2nd = 511 * u32BacklightNtcAdcValue_Average_2nd + u32BacklightNtcAdcValue_Average;        
    //u32BacklightNtcAdcValue_Average_2nd = u32BacklightNtcAdcValue_Average_2nd >> 9;
    //u16Result = u32BacklightNtcAdcValue_Average_2nd;
    
    u16Result = u32BacklightNtcAdcValue_Average;
	return u16Result;
} 


/******************************************************************************
;       Function Name			:	int16_t BacklightNtcAdcValue_lookuptable(uint16_t In1,uint16_t Table_x[],int16_t Table_y[],uint8_t maxIndex)
;       Function Description	:	
;       Parameters				:	uint16_t In1,uint16_t Table_x[],int16_t Table_y[],uint8_t maxIndex
;       Return Values			:	int16_t Temperature from NTC
;		Source ID				:	SWE3_xxx_xx
******************************************************************************/  
int16_t MTPC_BacklightNtcAdcValue_lookuptable_API(uint16_t In1,uint16_t Table_x[],int16_t Table_y[],uint8_t maxIndex)
{
    uint8_t i = NUM_ZERO;
	int16_t i16y = 0;
	int16_t Table_y_temp[166];
	uint8_t u8bpIdx = NUM_ZERO;
	for (i=0;i<=maxIndex;i++)
	{
        Table_y_temp[i]=Table_y[i]*10U;
	}
	Table_y=Table_y_temp;
    /* Linear Search */
	if (In1<Table_x[0U])/*Judge the input value below min value or not?*/
	{
		u8bpIdx=0U;
		i16y = Table_y[u8bpIdx];
	}
	else if (In1<Table_x[maxIndex])
	{
		for (u8bpIdx=maxIndex>>1U;In1<Table_x[u8bpIdx];u8bpIdx--)
		{
			
		}
		while (In1>=Table_x[u8bpIdx+1U])
		{
			u8bpIdx++;
		}
		i16y = ((In1 - Table_x[u8bpIdx]) *((Table_y[u8bpIdx + 1U] - Table_y[u8bpIdx]))/ (Table_x[u8bpIdx + 1U] - Table_x[u8bpIdx]))+  Table_y[u8bpIdx];
	}
	else
	{
		u8bpIdx=maxIndex;  /* Use last breakpoint for index at or above upper limitr*/
        i16y = Table_y[u8bpIdx];
	}
    
    return i16y;
}
/******************************************************************************
;       Function Name			:	uint32_t MTPC_Backlight_derating_process(int16_t i16Temp,uint16_t u16TEMP_NTC_0,uint16_t u16TEMP_NTC_74,uint16_t u16TEMP_NTC_82,uint16_t u16Duty_full,uint16_t u16Duty_end,int16_t u16BacklightUserDuty_Get)
;       Function Description	:	
;       Parameters				:	int16_t i16Temp,uint16_t u16TEMP_NTC_0,uint16_t u16TEMP_NTC_74,uint16_t u16TEMP_NTC_82,uint16_t u16Duty_full,uint16_t u16Duty_end,uint16_t u16BacklightUserDuty_Get
;       Return Values			:	uint32_t result ; uint8_t u8BacklightStatus=result>>16 �int16_t u16BacklightDutyCycle=result;
;		Source ID				:	SWE3_xxx_xx
******************************************************************************/ 

uint32_t MTPC_Backlight_derating_process(int16_t i16Temp,uint16_t u16TEMP_NTC_0,uint16_t u16TEMP_NTC_74,uint16_t u16TEMP_NTC_82,uint16_t u16Duty_full,uint16_t u16Duty_end, uint16_t u16BacklightUserDuty_Get)
{
    
        uint32_t result;
        uint16_t u16BacklightDutyCycle_10bit;
        uint16_t u16CoolingFanDutyCycle = NUM_ZERO;
        uint16_t u16TemperatureOffset = NUM_ZERO;
        /*53-17.4/97.5-90 = 5,表示每升高0.1度減少0.5 duty*/
        uint16_t u16TemperatureSlope = 5U;
        
        /*
        -    DVT1-1 version : 13/8/5   
        -    DVT1-2 version : 20/20/20
        -    DVT1-2 version : 20/16/13
        -    T-05.01.00 : 16/16/13
        */
        if(u16BacklightDutyCycle >= u16Duty10Bit_34PA)    
        {
            u16TemperatureOffset = u16TEMP_NTC_Offset_16;
        }
        else if((u16BacklightDutyCycle < u16Duty10Bit_34PA)&&(u16BacklightDutyCycle >= u16Duty10Bit_17PA4))     
        {
            u16TemperatureOffset = u16TEMP_NTC_Offset_16;
        }
        else    
        {
            u16TemperatureOffset = u16TEMP_NTC_Offset_13;
        }
    
	u16CoolingFanDutyCycle =  COOLING_FAN_Pwm_Duty_Output_Get();
    
	if (u8backlightprocess_initial==DISABLE)
	{
            u8backlightprocess_initial=ENABLE;
            u8BacklightStatus=initial_mode;
            u16BacklightDutyCycle = (u16BacklightUserDuty_Get*u16Duty_full)/PWM_Resolution_10bit;
	}
	else
	{
            switch (u8BacklightStatus)
            {
                case initial_mode:
                    if	(((int16_t)u16TEMP_NTC_0 <= i16Temp) && (i16Temp < (int16_t)(u16TEMP_NTC_74 + u16TemperatureOffset)))
                    {
                        u8BacklightStatus=normal_mode;
                        //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, NUM_ZERO);
                    }
                    else
                    {
                        if (((int16_t)u16TEMP_NTC_0 > i16Temp) || (i16Temp >= (int16_t)(u16TEMP_NTC_74 + u16TemperatureOffset)))
                        {
                            if(i16Temp >= (int16_t)(u16TEMP_NTC_74 + u16TemperatureOffset))
                            {
                                u16CoolingFanDutyCycle =  COOLING_FAN_Pwm_Duty_Output_Get();
                                u16CoolingFanDutyCycle += COOLING_FAN_DUTY_1PA;
                                if(u16CoolingFanDutyCycle >= MAX_COOLING_FAN_DUTY)
                                {
                                    u16CoolingFanDutyCycle = MAX_COOLING_FAN_DUTY;
                                }
                                //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, u16CoolingFanDutyCycle);
                               
                                
                                //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, u16CoolingFanDutyCycle);
                                
                            }
                            
                            u8BacklightStatus=boot_mode;
                            u16backlightCounter = Reset_Counter;
                        }
                        else
                        {
                            u16BacklightDutyCycle = (u16BacklightUserDuty_Get*u16Duty_full)/PWM_Resolution_10bit;
                        }
                    }
                    /* record derating status */
                    MMIM_Derating_Status_Set(NUM_ZERO);
                    
                    
                                break;
                    
                case normal_mode:
                    if	(((int16_t)(u16TEMP_NTC_74 + u16TemperatureOffset) <= i16Temp) && (i16Temp <= (int16_t)(u16TEMP_NTC_82 + u16TemperatureOffset)))
                    {
                        u8BacklightStatus=derating_mode;
                        u8BacklightStatus_Derating_mode=ENABLE;
                        i16BacklightStatus_Derating_temprecord=i16Temp;
                        //u16BacklightDutyCycle = u16Duty_52PA-(u16Duty_52PA-u16Duty_17PA4)*((uint16_t)i16Temp-u16TEMP_NTC_74-u16TemperatureOffset)/(u16TEMP_NTC_82-u16TEMP_NTC_74);
                        u16BacklightDutyCycle = u16Duty_52PA-(((uint16_t)i16Temp-u16TEMP_NTC_74-u16TemperatureOffset)*u16TemperatureSlope);
                        
                        u16CoolingFanDutyCycle =  COOLING_FAN_Pwm_Duty_Output_Get();
                        u16CoolingFanDutyCycle += COOLING_FAN_DUTY_1PA;
                        if(u16CoolingFanDutyCycle >= MAX_COOLING_FAN_DUTY)
                        {
                            u16CoolingFanDutyCycle = MAX_COOLING_FAN_DUTY;
                        }
                        //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, u16CoolingFanDutyCycle);
                        
                        
                        //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, u16CoolingFanDutyCycle);
                        
                    }
                    else if((i16Temp > ((int16_t)(u16TEMP_NTC_82 + u16TemperatureOffset)))&&(i16Temp <= ((int16_t)(u16TEMP_NTC_82 + u16TemperatureOffset + u16TEMP_DQA_RESERVED))))
                    {
                        u8BacklightStatus=derating_mode;
                        u8BacklightStatus_Derating_mode=ENABLE;
                        i16BacklightStatus_Derating_temprecord=i16Temp;
                        u16BacklightDutyCycle = 50;
                        
                        u16CoolingFanDutyCycle =  COOLING_FAN_Pwm_Duty_Output_Get();
                        u16CoolingFanDutyCycle += COOLING_FAN_DUTY_1PA;
                        if(u16CoolingFanDutyCycle >= MAX_COOLING_FAN_DUTY)
                        {
                            u16CoolingFanDutyCycle = MAX_COOLING_FAN_DUTY;
                        }
                        //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, u16CoolingFanDutyCycle);
                        
                        
                        //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, u16CoolingFanDutyCycle);
                        
                    }
                    else if ((i16Temp > (int16_t)(u16TEMP_NTC_82 + u16TemperatureOffset + u16TEMP_DQA_RESERVED)))
                    {
                        u8BacklightStatus_Derating_mode=DISABLE;
                        u8BacklightStatus=shutdown_mode;
                        u16BacklightDutyCycle=u16Duty_end;
                        
                        //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, MAX_COOLING_FAN_DUTY);
                    }
                    else
                    {
                        u16BacklightDutyCycle = (u16BacklightUserDuty_Get*u16Duty_full)/PWM_Resolution_10bit;
                        u16CoolingFanDutyCycle =  COOLING_FAN_Pwm_Duty_Output_Get();
                        //u16CoolingFanDutyCycle += COOLING_FAN_DUTY_1PA;
                        //DEBUG_PF("u16CoolingFanDutyCycle = %d\r\n",u16CoolingFanDutyCycle);
                        if(u16CoolingFanDutyCycle >= MAX_COOLING_FAN_DUTY)
                        {
                            u16CoolingFanDutyCycle = MAX_COOLING_FAN_DUTY;
                        }
                        
                        
                        COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, u16CoolingFanDutyCycle);
                        
                        
                    }
                    
                    /* record derating status */
                    MMIM_Derating_Status_Set(NUM_ZERO);
                                break;
                                
                case derating_mode:
                    if (((int16_t)u16TEMP_NTC_0 <= i16Temp) && (i16Temp < (int16_t)(u16TEMP_NTC_74 + u16TemperatureOffset)))
                    {
                        u8BacklightStatus_Derating_mode=DISABLE;
                        u8BacklightStatus=normal_mode;
                        u16BacklightDutyCycle = (u16BacklightUserDuty_Get*u16Duty_full)/PWM_Resolution_10bit;
                        //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, NUM_ZERO);
                    }
                    else
                    {
                        if (i16Temp > (int16_t)(u16TEMP_NTC_82 + u16TemperatureOffset + u16TEMP_DQA_RESERVED))
                        {
                            u8BacklightStatus_Derating_mode=DISABLE;
                            u8BacklightStatus=shutdown_mode;
                            u16BacklightDutyCycle=u16Duty_end;
                            //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, MAX_COOLING_FAN_DUTY);
                        }
                        else if (u8BacklightStatus_Derating_mode == ENABLE)
                        {
                            if  (((int16_t)(u16TEMP_NTC_74 + u16TemperatureOffset) <= i16Temp) && (i16Temp <= (int16_t)(u16TEMP_NTC_82 + u16TemperatureOffset)))
                            {
                                if (abs(i16Temp-i16BacklightStatus_Derating_temprecord)>=temperature_diff_record)
                                {
                                    u8BacklightStatus_Derating_mode=ENABLE;
                                    i16BacklightStatus_Derating_temprecord=i16Temp;
                                    //u16BacklightDutyCycle = u16Duty_52PA-(u16Duty_52PA-u16Duty_17PA4)*((uint16_t)i16Temp-u16TEMP_NTC_74-u16TemperatureOffset)/(u16TEMP_NTC_82-u16TEMP_NTC_74);
                                    u16BacklightDutyCycle = u16Duty_52PA-(((uint16_t)i16Temp-u16TEMP_NTC_74-u16TemperatureOffset)*u16TemperatureSlope);
                                    
                                    u16CoolingFanDutyCycle =  COOLING_FAN_Pwm_Duty_Output_Get();
                                    u16CoolingFanDutyCycle += COOLING_FAN_DUTY_1PA;
                                    if(u16CoolingFanDutyCycle >= MAX_COOLING_FAN_DUTY)
                                    {
                                        u16CoolingFanDutyCycle = MAX_COOLING_FAN_DUTY;
                                    }
                                    //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, u16CoolingFanDutyCycle);
                                    
                                    
                                    //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, u16CoolingFanDutyCycle);
                                    
                                }
                                else
                                {
                                    ;// u8BacklightStatus_Derating_mode=ENABLE;
                                }
                            }
                            else
                            {
                                // u8BacklightStatus_Derating_mode=ENABLE;
                            }
                            
                            if ((i16Temp > ((int16_t)(u16TEMP_NTC_82 + u16TemperatureOffset)))&&(i16Temp <= ((int16_t)(u16TEMP_NTC_82 + u16TemperatureOffset + u16TEMP_DQA_RESERVED))))
                            {
                                u8BacklightStatus_Derating_mode=ENABLE;
                                i16BacklightStatus_Derating_temprecord=i16Temp;
                                u16BacklightDutyCycle = 50;  //50 = 5 PA
                                
                                u16CoolingFanDutyCycle =  COOLING_FAN_Pwm_Duty_Output_Get();
                                u16CoolingFanDutyCycle += COOLING_FAN_DUTY_1PA;
                                if(u16CoolingFanDutyCycle >= MAX_COOLING_FAN_DUTY)
                                {
                                    u16CoolingFanDutyCycle = MAX_COOLING_FAN_DUTY;
                                }
                                //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, u16CoolingFanDutyCycle);
                                
                                
                                //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, u16CoolingFanDutyCycle);
                                
                            }
                            
                        }
                        else
                        {
                            ;//
                        }
                    }
                    /* record derating status */
                    MMIM_Derating_Status_Set(NUM_ONE);
                                break;
                                
                case boot_mode:
                    u16backlightCounter=u16backlightCounter+1U;
                    
                    if (((int16_t)u16TEMP_NTC_0 <= i16Temp) && (i16Temp < (int16_t)(u16TEMP_NTC_74 + u16TemperatureOffset)))
                    {
                        u16backlightCounter = Reset_Counter;
                        u8BacklightStatus=initial_mode;
                        //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, NUM_ZERO);
                    }
                    else
                    {
                        if (u16backlightCounter>=BootMode_delaytimes)
                        {
                            u16backlightCounter = Reset_Counter;
                            if (((int16_t)u16TEMP_NTC_0 <= i16Temp) && (i16Temp < (int16_t)(u16TEMP_NTC_74 + u16TemperatureOffset)))
                            {
                                  u8BacklightStatus=normal_mode;
                                  //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, NUM_ZERO);
                            }
                            else if(((int16_t)(u16TEMP_NTC_74 + u16TemperatureOffset) <= i16Temp) && (i16Temp <= (int16_t)(u16TEMP_NTC_82 + u16TemperatureOffset)))
                            {
                                  u8BacklightStatus=derating_mode;
                                  u8BacklightStatus_Derating_mode=ENABLE;
                                  i16BacklightStatus_Derating_temprecord=i16Temp;
                                  //u16BacklightDutyCycle = u16Duty_52PA-(u16Duty_52PA-u16Duty_17PA4)*((uint16_t)i16Temp-u16TEMP_NTC_74-u16TemperatureOffset)/(u16TEMP_NTC_82-u16TEMP_NTC_74);
                                  u16BacklightDutyCycle = u16Duty_52PA-(((uint16_t)i16Temp-u16TEMP_NTC_74-u16TemperatureOffset)*u16TemperatureSlope);
          
                                  u16CoolingFanDutyCycle =  COOLING_FAN_Pwm_Duty_Output_Get();
                                  u16CoolingFanDutyCycle += COOLING_FAN_DUTY_1PA;
                                  if(u16CoolingFanDutyCycle >= MAX_COOLING_FAN_DUTY)
                                  {
                                          u16CoolingFanDutyCycle = MAX_COOLING_FAN_DUTY;
                                  }
                                  //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, u16CoolingFanDutyCycle);
                                  
                                  
                                    //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, u16CoolingFanDutyCycle);
                                  
                            }
                            else if((i16Temp > ((int16_t)(u16TEMP_NTC_82 + u16TemperatureOffset)))&&(i16Temp <= ((int16_t)(u16TEMP_NTC_82 + u16TemperatureOffset + u16TEMP_DQA_RESERVED))))
                            {
                                  u8BacklightStatus=derating_mode;
                                  u8BacklightStatus_Derating_mode=ENABLE;
                                  i16BacklightStatus_Derating_temprecord=i16Temp;
                                  u16BacklightDutyCycle = 50;  //50 = 5 PA
          
                                  u16CoolingFanDutyCycle =  COOLING_FAN_Pwm_Duty_Output_Get();
                                  u16CoolingFanDutyCycle += COOLING_FAN_DUTY_1PA;
                                  if(u16CoolingFanDutyCycle >= MAX_COOLING_FAN_DUTY)
                                  {
                                          u16CoolingFanDutyCycle = MAX_COOLING_FAN_DUTY;
                                  }
                                  //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, u16CoolingFanDutyCycle);
                                  
                                  
                                //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, u16CoolingFanDutyCycle);
                                  
                            }
                            else if (i16Temp > (int16_t)u16TEMP_NTC_82 + u16TemperatureOffset + u16TEMP_DQA_RESERVED)
                            {
                                  u8BacklightStatus=shutdown_mode;
                                  u16BacklightDutyCycle=u16Duty_end;
                                  //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, MAX_COOLING_FAN_DUTY);
                                  
                                  
                                //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, u16CoolingFanDutyCycle);
                                  
                            }
                            else
                            {
                                                        ;//						  
                            }
                        }
                    }
                    /* record derating status */
                    MMIM_Derating_Status_Set(NUM_ZERO);
                                break;
                                
                case shutdown_mode:
                    if (i16Temp <= ((int16_t)(u16TEMP_NTC_82 + u16TemperatureOffset)))
                    {
                        u16backlightCounter=u16backlightCounter+1U;
                        if (u16backlightCounter>=ShutDownrecover_delaytimes)
                        {
                            u16backlightCounter = Reset_Counter;
                            u8BacklightStatus=derating_mode;
                            u8BacklightStatus_Derating_mode=ENABLE;
                            i16BacklightStatus_Derating_temprecord=i16Temp;
                            //u16BacklightDutyCycle = u16Duty_52PA-(u16Duty_52PA-u16Duty_17PA4)*((uint16_t)i16Temp-u16TEMP_NTC_74-u16TemperatureOffset)/(u16TEMP_NTC_82-u16TEMP_NTC_74);
                            u16BacklightDutyCycle = u16Duty_52PA-(((uint16_t)i16Temp-u16TEMP_NTC_74-u16TemperatureOffset)*u16TemperatureSlope);
                        }
                        if(i16Temp >= ((int16_t)(u16TEMP_NTC_74 + u16TemperatureOffset)))
                        {
                            u16CoolingFanDutyCycle =  COOLING_FAN_Pwm_Duty_Output_Get();
                            u16CoolingFanDutyCycle += COOLING_FAN_DUTY_1PA;
                            if(u16CoolingFanDutyCycle >= MAX_COOLING_FAN_DUTY)
                            {
                                u16CoolingFanDutyCycle = MAX_COOLING_FAN_DUTY;
                            }
                            //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, u16CoolingFanDutyCycle);
                            
                            
                            //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, u16CoolingFanDutyCycle);
                            
                        }
                    }
                    else if((i16Temp > ((int16_t)(u16TEMP_NTC_82 + u16TemperatureOffset)))&&(i16Temp <= ((int16_t)(u16TEMP_NTC_82 + u16TemperatureOffset + u16TEMP_DQA_RESERVED))))
                    {
                        u16backlightCounter=u16backlightCounter+1U;
                        if (u16backlightCounter>=ShutDownrecover_delaytimes)
                        {
                            u16backlightCounter = Reset_Counter;
                            u8BacklightStatus=derating_mode;
                            u8BacklightStatus_Derating_mode=ENABLE;
                            i16BacklightStatus_Derating_temprecord=i16Temp;
                            u16BacklightDutyCycle = 50;  //50 = 5 PA
                        }
                        if(i16Temp >= ((int16_t)(u16TEMP_NTC_74 + u16TemperatureOffset)))
                        {
                            u16CoolingFanDutyCycle =  COOLING_FAN_Pwm_Duty_Output_Get();
                            u16CoolingFanDutyCycle += COOLING_FAN_DUTY_1PA;
                            if(u16CoolingFanDutyCycle >= MAX_COOLING_FAN_DUTY)
                            {
                                u16CoolingFanDutyCycle = MAX_COOLING_FAN_DUTY;
                            }
                            //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, u16CoolingFanDutyCycle);
                            
                            
                            //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, u16CoolingFanDutyCycle);
                            
                        }
                    }
                    else
                    {
                        u16backlightCounter = Reset_Counter;
                        u16BacklightDutyCycle=u16Duty_end;
                    }
                    /* record derating status */
                    MMIM_Derating_Status_Set(NUM_ZERO);
                    
                                break;
                                
                default:
                                // ;
                                break;
            }
	}
    
	u16BacklightDutyCycle_10bit=(PWM_Resolution_10bit*u16BacklightDutyCycle)/u16Duty_full;
    
	if (u16BacklightDutyCycle_10bit==0)
	{
		u16BacklightDutyCycle_10bit=0;
	}
	else if (u16BacklightDutyCycle_10bit>=PWM_Resolution_10bit)
	{
		u16BacklightDutyCycle_10bit=PWM_Resolution_10bit;
	}
	else
	{
		u16BacklightDutyCycle_10bit=u16BacklightDutyCycle_10bit;
	}
	u16BacklightDutyCycle_10bit=min(u16BacklightDutyCycle_10bit,u16BacklightUserDuty_Get);
	
	result=((uint32_t)u8BacklightStatus<<16U) + ((uint16_t)u16BacklightDutyCycle_10bit);
	return result;
}

/*---------------------------- Start Program ---------------------------------*/
/******************************************************************************
;       Function Name			:	DeratingControl_Process
;       Function Description	:	
;       Parameters				:	uint16_t u16BacklightNtcAdcAverage,uint16_t u16MCUBandGap_ADCValues
;       Return Values			:	uint32_t DeratingControl_Process_result ; uint8_t u8BacklightStatus=DeratingControl_Process_result>>8 �int16_t u16BacklightDutyCycle=DeratingControl_Process_result;
;		Source ID				:	SWEDD_003_01
******************************************************************************/


uint32_t MTPC_TemperatureProtection_Ctrl(uint8_t DataSource, uint16_t u16BacklightNtcAdcValue,uint16_t u16MCURef_1V_ADCValues,uint16_t u16BacklightUserDuty_Get)
{
	uint32_t TemperatureProtection_Ctrl_result;
    
	u16BacklightNtcAdcValue=((u16BacklightNtcAdcValue*MCU_BandGap*u16MCURef_1V_ADCValues)/(ADC_Value_Max * FIXEDPOINT_MODIFY_PARAMETER));
    //u16BacklightNtcAdcValue_Average=MTPC_BacklightNtcAdcValue_Average_API(u16BacklightNtcAdcValue);
    
    
    
	if(DataSource == TEMPSOUREFROM_BL_R)
	{
		gtsTempForm_ModTemPro.DataSource = TEMPSOUREFROM_BL_R;
		i16BacklightTemperature = MTPC_BacklightNtcAdcValue_lookuptable_API(u16BacklightNtcAdcValue,u16PCBNtcAdcTable_RawData,i8PCBNtcAdcTable_Temp, (sizeof(u16PCBNtcAdcTable_RawData)/sizeof(u16PCBNtcAdcTable_RawData[0]))-1);
        //DEBUG_PF("i16BacklightTemperature(BL_R)=%d\r\n",i16BacklightTemperature);
	}
	else if(DataSource == TEMPSOUREFROM_BL_L)
	{
		gtsTempForm_ModTemPro.DataSource = TEMPSOUREFROM_BL_L;
		i16BacklightTemperature = MTPC_BacklightNtcAdcValue_lookuptable_API(u16BacklightNtcAdcValue,u16PCBNtcAdcTable_RawData,i8PCBNtcAdcTable_Temp, (sizeof(u16PCBNtcAdcTable_RawData)/sizeof(u16PCBNtcAdcTable_RawData[0]))-1);
        //DEBUG_PF("i16BacklightTemperature(BL_L)=%d\r\n",i16BacklightTemperature);
	}
	else if(DataSource == TEMPSOUREFROM_984)
	{	
		gtsTempForm_ModTemPro.DataSource = TEMPSOUREFROM_984;
		i16BacklightTemperature = MTPC_BacklightNtcAdcValue_lookuptable_API(u16BacklightNtcAdcValue,u16PCB984NtcAdcTable_RawData,i8PCBNtcAdcTable_Temp, (sizeof(u16PCB984NtcAdcTable_RawData)/sizeof(u16PCB984NtcAdcTable_RawData[0]))-1);
        //DEBUG_PF("i16BacklightTemperature(984)=%d\r\n",i16BacklightTemperature);
	}
	else
	{
		;
	}
	
	/*For DQA test*/
	if (i16BacklightTemperature<=0)
	{
		gtsTempForm_ModTemPro.symbol=Negative;
		gtsTempForm_ModTemPro.Temperature_Value=i16BacklightTemperature*(-1);
	}
	else
	{
		gtsTempForm_ModTemPro.symbol=Positive;
		gtsTempForm_ModTemPro.Temperature_Value=i16BacklightTemperature;
	}
	TemperatureProtection_Ctrl_result=MTPC_Backlight_derating_process(i16BacklightTemperature,u16TEMP_NTC_0,u16TEMP_NTC_74,u16TEMP_NTC_82,u16Duty_full,u16Duty_end,u16BacklightUserDuty_Get);
    
	return TemperatureProtection_Ctrl_result;
}

/*For DQA test*/
Mod_TemPro_Ctrl_TableTypedef Mod_TemPro_Ctrl_Table_Get(void)
{
    return gtsTempForm_ModTemPro;
}


int16_t MTPC_Temperature_Get(uint8_t DataSource, uint16_t u16BacklightNtcAdcValue,uint16_t u16MCURef_1V_ADCValues)
{
	uint16_t u16BacklightNtcAdcValue_temp;
	int16_t i16_Temperature;
	u16BacklightNtcAdcValue_temp = u16BacklightNtcAdcValue;
	u16BacklightNtcAdcValue_temp=((u16BacklightNtcAdcValue_temp*MCU_BandGap*u16MCURef_1V_ADCValues)/(ADC_Value_Max * FIXEDPOINT_MODIFY_PARAMETER));
	if(DataSource == TEMPSOUREFROM_BL_R)
	{	
		MMIM_BLNTCADC_Set (u16BacklightNtcAdcValue);
		gtsTempForm_ModTemPro.DataSource = TEMPSOUREFROM_BL_R;
		i16_Temperature = MTPC_BacklightNtcAdcValue_lookuptable_API(u16BacklightNtcAdcValue_temp,u16PCBNtcAdcTable_RawData,i8PCBNtcAdcTable_Temp, (sizeof(u16PCBNtcAdcTable_RawData)/sizeof(u16PCBNtcAdcTable_RawData[0]))-1);
	}
	else if(DataSource == TEMPSOUREFROM_BL_L)
	{
		MMIM_BLNTCADC_Set (u16BacklightNtcAdcValue);
		gtsTempForm_ModTemPro.DataSource = TEMPSOUREFROM_BL_L;
		i16_Temperature = MTPC_BacklightNtcAdcValue_lookuptable_API(u16BacklightNtcAdcValue_temp,u16PCBNtcAdcTable_RawData,i8PCBNtcAdcTable_Temp, (sizeof(u16PCBNtcAdcTable_RawData)/sizeof(u16PCBNtcAdcTable_RawData[0]))-1);
	}
	else if(DataSource == TEMPSOUREFROM_984)
	{
		gtsTempForm_ModTemPro.DataSource = TEMPSOUREFROM_984;
		i16_Temperature = MTPC_BacklightNtcAdcValue_lookuptable_API(u16BacklightNtcAdcValue_temp,u16PCB984NtcAdcTable_RawData,i8PCBNtcAdcTable_Temp, (sizeof(u16PCB984NtcAdcTable_RawData)/sizeof(u16PCB984NtcAdcTable_RawData[0]))-1);
	}
	else
	{
		;
	}
    
	if(gtsTempForm_ModTemPro.DataSource == TEMPSOUREFROM_BL_R || gtsTempForm_ModTemPro.DataSource == TEMPSOUREFROM_BL_L)
	{
		if (i16_Temperature<=0)
		{
			MMIM_BLTemperatureVal_Set(Negative,(i16_Temperature*(-1)),DataSource);
		}
		else
		{
			MMIM_BLTemperatureVal_Set(Positive,i16_Temperature,DataSource);
		}
	}
	else if(gtsTempForm_ModTemPro.DataSource == TEMPSOUREFROM_984)
	{
		if (i16_Temperature<=0)
		{
			MMIM_PCBTemperatureVal_Set(Negative,(i16_Temperature*(-1)),DataSource);
		}
		else
		{
			MMIM_PCBTemperatureVal_Set(Positive,i16_Temperature,DataSource);
		}	
	}
	else
	{
		;
	}
    
	return i16_Temperature;
}
