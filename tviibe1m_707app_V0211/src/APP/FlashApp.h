/*
* FlashApp.h
*
*  Created on: 2020�~11��14��
*      Author: JoshuaLuo
*/

#ifndef FLASHAPP_H_
#define FLASHAPP_H_




#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>

//----------------------- Define Constant -------------------------------------//
#define SIZE_4096_BIT_IN_BYTE (0x200ul)
#define SIZE_4096_BIT_IN_WORD (0x200ul / 4ul)

#define SIZE_WORK_FLASH_LSECTOR_BYTE (2048UL)  //2KB
#define SIZE_WORK_FLASH_SSECTOR_BYTE (128UL)    //128BYTE

#define SIZE_CODE_FLASH_LSECTOR_BYTE (32768UL) //32KB
#define SIZE_CODE_FLASH_SSECTOR_BYTE (8192UL)   //8KB


// #define PROGRAM_DATA_SIZE_IN_BYTE(buffer) (sizeof(programData)/sizeof(uint8_t))
// #define PROGRAM_DATA_SIZE_IN_WORD(buffer)  (sizeof(programData)/sizeof(uint32_t))





//Sector Number 		    Address		    Size (KB)		
#define	ADDR_WorkFlash_L0	0x14000000UL		//2	<-- Bank 0				
#define	ADDR_WorkFlash_L1	0x14000800UL		//4	                    	
#define	ADDR_WorkFlash_L2	0x14001000UL		//6	                    	
#define	ADDR_WorkFlash_L3	0x14001800UL		//8	                    	
#define	ADDR_WorkFlash_L4	0x14002000UL		//10	                    	
#define	ADDR_WorkFlash_L5	0x14002800UL		//12	                    	
#define	ADDR_WorkFlash_L6	0x14003000UL		//14	                    	
#define	ADDR_WorkFlash_L7	0x14003800UL		//16	                    	
#define	ADDR_WorkFlash_L8	0x14004000UL		//18	                    	
#define	ADDR_WorkFlash_L9	0x14004800UL		//20	                    	
#define	ADDR_WorkFlash_L10	0x14005000UL		//22	                    	
#define	ADDR_WorkFlash_L11	0x14005800UL		//24	                    	
#define	ADDR_WorkFlash_L12	0x14006000UL		//26	<-- Bank 1          	
#define	ADDR_WorkFlash_L13	0x14006800UL		//28	                    	
#define	ADDR_WorkFlash_L14	0x14007000UL		//30	                    	
#define	ADDR_WorkFlash_L15	0x14007800UL		//32	                    	
#define	ADDR_WorkFlash_L16	0x14008000UL		//34	                    	
#define	ADDR_WorkFlash_L17	0x14008800UL		//36	                    	
#define	ADDR_WorkFlash_L18	0x14009000UL		//38	                    	
#define	ADDR_WorkFlash_L19	0x14009800UL		//40	                    	
#define	ADDR_WorkFlash_L20	0x1400A000UL		//42	                    	
#define	ADDR_WorkFlash_L21	0x1400A800UL		//44	                    	
#define	ADDR_WorkFlash_L22	0x1400B000UL		//46	                    	
#define	ADDR_WorkFlash_L23	0x1400B800UL		//48	                    	


//Sector Number 			    Address			Size (Byte)        	
#define	ADDR_WorkFlash_S0		0x1400C000UL		//128	<--Bank 0       
#define	ADDR_WorkFlash_S1		0x1400C080UL		//256	                
#define	ADDR_WorkFlash_S2		0x1400C100UL		//384	                
#define	ADDR_WorkFlash_S3		0x1400C180UL		//512	                
#define	ADDR_WorkFlash_S4		0x1400C200UL		//640	                
#define	ADDR_WorkFlash_S5		0x1400C280UL		//768	                
#define	ADDR_WorkFlash_S6		0x1400C300UL		//896	                
#define	ADDR_WorkFlash_S7		0x1400C380UL		//1024	                
#define	ADDR_WorkFlash_S8		0x1400C400UL		//1152	                
#define	ADDR_WorkFlash_S9		0x1400C480UL		//1280	                
#define	ADDR_WorkFlash_S10		0x1400C500UL		//1408	                
#define	ADDR_WorkFlash_S11		0x1400C580UL		//1536	                
#define	ADDR_WorkFlash_S12		0x1400C600UL		//1664	                
#define	ADDR_WorkFlash_S13		0x1400C680UL		//1792	                
#define	ADDR_WorkFlash_S14		0x1400C700UL		//1920	                
#define	ADDR_WorkFlash_S15		0x1400C780UL		//2048	                
#define	ADDR_WorkFlash_S16		0x1400C800UL		//2176	                
#define	ADDR_WorkFlash_S17		0x1400C880UL		//2304	                
#define	ADDR_WorkFlash_S18		0x1400C900UL		//2432	                
#define	ADDR_WorkFlash_S19		0x1400C980UL		//2560	                
#define	ADDR_WorkFlash_S20		0x1400CA00UL		//2688	                
#define	ADDR_WorkFlash_S21		0x1400CA80UL		//2816	                
#define	ADDR_WorkFlash_S22		0x1400CB00UL		//2944	                
#define	ADDR_WorkFlash_S23		0x1400CB80UL		//3072	                
#define	ADDR_WorkFlash_S24		0x1400CC00UL		//3200	                
#define	ADDR_WorkFlash_S25		0x1400CC80UL		//3328	                
#define	ADDR_WorkFlash_S26		0x1400CD00UL		//3456	                
#define	ADDR_WorkFlash_S27		0x1400CD80UL		//3584	                
#define	ADDR_WorkFlash_S28		0x1400CE00UL		//3712	                
#define	ADDR_WorkFlash_S29		0x1400CE80UL		//3840	                
#define	ADDR_WorkFlash_S30		0x1400CF00UL		//3968	                
#define	ADDR_WorkFlash_S31		0x1400CF80UL		//4096	                
#define	ADDR_WorkFlash_S32		0x1400D000UL		//4224	                
#define	ADDR_WorkFlash_S33		0x1400D080UL		//4352	                
#define	ADDR_WorkFlash_S34		0x1400D100UL		//4480	                
#define	ADDR_WorkFlash_S35		0x1400D180UL		//4608	                
#define	ADDR_WorkFlash_S36		0x1400D200UL		//4736	                
#define	ADDR_WorkFlash_S37		0x1400D280UL		//4864	                
#define	ADDR_WorkFlash_S38		0x1400D300UL		//4992	                
#define	ADDR_WorkFlash_S39		0x1400D380UL		//5120	                
#define	ADDR_WorkFlash_S40		0x1400D400UL		//5248	                
#define	ADDR_WorkFlash_S41		0x1400D480UL		//5376	                
#define	ADDR_WorkFlash_S42		0x1400D500UL		//5504	                
#define	ADDR_WorkFlash_S43		0x1400D580UL		//5632	                
#define	ADDR_WorkFlash_S44		0x1400D600UL		//5760	                
#define	ADDR_WorkFlash_S45		0x1400D680UL		//5888	                
#define	ADDR_WorkFlash_S46		0x1400D700UL		//6016	                
#define	ADDR_WorkFlash_S47		0x1400D780UL		//6144	                
#define	ADDR_WorkFlash_S48		0x1400D800UL		//6272	                
#define	ADDR_WorkFlash_S49		0x1400D880UL		//6400	                
#define	ADDR_WorkFlash_S50		0x1400D900UL		//6528	                
#define	ADDR_WorkFlash_S51		0x1400D980UL		//6656	                
#define	ADDR_WorkFlash_S52		0x1400DA00UL		//6784	                
#define	ADDR_WorkFlash_S53		0x1400DA80UL		//6912	                
#define	ADDR_WorkFlash_S54		0x1400DB00UL		//7040	                
#define	ADDR_WorkFlash_S55		0x1400DB80UL		//7168	                
#define	ADDR_WorkFlash_S56		0x1400DC00UL		//7296	                
#define	ADDR_WorkFlash_S57		0x1400DC80UL		//7424	                
#define	ADDR_WorkFlash_S58		0x1400DD00UL		//7552	                
#define	ADDR_WorkFlash_S59		0x1400DD80UL		//7680	                
#define	ADDR_WorkFlash_S60		0x1400DE00UL		//7808	                
#define	ADDR_WorkFlash_S61		0x1400DE80UL		//7936	                
#define	ADDR_WorkFlash_S62		0x1400DF00UL		//8064	                
#define	ADDR_WorkFlash_S63		0x1400DF80UL		//8192	                
#define	ADDR_WorkFlash_S64		0x1400E000UL		//8320	<-- Bank 1      
#define	ADDR_WorkFlash_S65		0x1400E080UL		//8448	                
#define	ADDR_WorkFlash_S66		0x1400E100UL		//8576	                
#define	ADDR_WorkFlash_S67		0x1400E180UL		//8704	                
#define	ADDR_WorkFlash_S68		0x1400E200UL		//8832	                
#define	ADDR_WorkFlash_S69		0x1400E280UL		//8960	                
#define	ADDR_WorkFlash_S70		0x1400E300UL		//9088	                
#define	ADDR_WorkFlash_S71		0x1400E380UL		//9216	                
#define	ADDR_WorkFlash_S72		0x1400E400UL		//9344	                
#define	ADDR_WorkFlash_S73		0x1400E480UL		//9472	                
#define	ADDR_WorkFlash_S74		0x1400E500UL		//9600	                
#define	ADDR_WorkFlash_S75		0x1400E580UL		//9728	                
#define	ADDR_WorkFlash_S76		0x1400E600UL		//9856	                
#define	ADDR_WorkFlash_S77		0x1400E680UL		//9984	                
#define	ADDR_WorkFlash_S78		0x1400E700UL		//10112	                
#define	ADDR_WorkFlash_S79		0x1400E780UL		//10240	                
#define	ADDR_WorkFlash_S80		0x1400E800UL		//10368	                
#define	ADDR_WorkFlash_S81		0x1400E880UL		//10496	                
#define	ADDR_WorkFlash_S82		0x1400E900UL		//10624	                
#define	ADDR_WorkFlash_S83		0x1400E980UL		//10752	                
#define	ADDR_WorkFlash_S84		0x1400EA00UL		//10880	                
#define	ADDR_WorkFlash_S85		0x1400EA80UL		//11008	                
#define	ADDR_WorkFlash_S86		0x1400EB00UL		//11136	                
#define	ADDR_WorkFlash_S87		0x1400EB80UL		//11264	                
#define	ADDR_WorkFlash_S88		0x1400EC00UL		//11392	                
#define	ADDR_WorkFlash_S89		0x1400EC80UL		//11520	                
#define	ADDR_WorkFlash_S90		0x1400ED00UL		//11648	                
#define	ADDR_WorkFlash_S91		0x1400ED80UL		//11776	                
#define	ADDR_WorkFlash_S92		0x1400EE00UL		//11904	                
#define	ADDR_WorkFlash_S93		0x1400EE80UL		//12032	                
#define	ADDR_WorkFlash_S94		0x1400EF00UL		//12160	                
#define	ADDR_WorkFlash_S95		0x1400EF80UL		//12288	                
#define	ADDR_WorkFlash_S96		0x1400F000UL		//12416	                
#define	ADDR_WorkFlash_S97		0x1400F080UL		//12544	                
#define	ADDR_WorkFlash_S98		0x1400F100UL		//12672	                
#define	ADDR_WorkFlash_S99		0x1400F180UL		//12800	                
#define	ADDR_WorkFlash_S100		0x1400F200UL		//12928	                	
#define	ADDR_WorkFlash_S101		0x1400F280UL		//13056	                	
#define	ADDR_WorkFlash_S102		0x1400F300UL		//13184	                	
#define	ADDR_WorkFlash_S103		0x1400F380UL		//13312	                	
#define	ADDR_WorkFlash_S104		0x1400F400UL		//13440	                	
#define	ADDR_WorkFlash_S105		0x1400F480UL		//13568	                	
#define	ADDR_WorkFlash_S106		0x1400F500UL		//13696	                	
#define	ADDR_WorkFlash_S107		0x1400F580UL		//13824	                	
#define	ADDR_WorkFlash_S108		0x1400F600UL		//13952	                	
#define	ADDR_WorkFlash_S109		0x1400F680UL		//14080	                	
#define	ADDR_WorkFlash_S110		0x1400F700UL		//14208	                	
#define	ADDR_WorkFlash_S111		0x1400F780UL		//14336	                	
#define	ADDR_WorkFlash_S112		0x1400F800UL		//14464	                	
#define	ADDR_WorkFlash_S113		0x1400F880UL		//14592	                	
#define	ADDR_WorkFlash_S114		0x1400F900UL		//14720	                	
#define	ADDR_WorkFlash_S115		0x1400F980UL		//14848	                	
#define	ADDR_WorkFlash_S116		0x1400FA00UL		//14976	                	
#define	ADDR_WorkFlash_S117		0x1400FA80UL		//15104	                	
#define	ADDR_WorkFlash_S118		0x1400FB00UL		//15232	                	
#define	ADDR_WorkFlash_S119		0x1400FB80UL		//15360	                	
#define	ADDR_WorkFlash_S120		0x1400FC00UL		//15488	                	
#define	ADDR_WorkFlash_S121		0x1400FC80UL		//15616	                	
#define	ADDR_WorkFlash_S122		0x1400FD00UL		//15744	                	
#define	ADDR_WorkFlash_S123		0x1400FD80UL		//15872	                	
#define	ADDR_WorkFlash_S124		0x1400FE00UL		//16000	                	
#define	ADDR_WorkFlash_S125		0x1400FE80UL		//16128	                	
#define	ADDR_WorkFlash_S126		0x1400FF00UL		//16256	                	
#define	ADDR_WorkFlash_S127		0x1400FF80UL		//16384	 ,16KB   


#define		ADDR_EEPROM					0x14000000
#define		ADDR_BLU					0x14000000
#define		ADDR_SN						0x14000100
#define		ADDR_PS						0x14000200
#define		ADDR_BL_B1					0x14000300			// 1 Bytes
#define		ADDR_BL_B2					0x14000301			// 1 Bytes
#define		ADDR_BL_T1					0x14000302			// 2 Bytes, 0x14000302 ~ 0x14000303
#define		ADDR_BL_PWM_DUTY			0x14000309			// 1 Bytes, for de-rating normal
#define		ADDR_BL_DR_SAVE				0x14000310			// 1 Bytes


#define CUSTOMER_ID_SECTION         SECTION(".customer_id")
#define MCU_CHECKSUM_SECTION        SECTION(".mcu_checksum")
#define SW_VERSION_SECTION          SECTION(".sw_version")

#define DisplayIDAddr                       ADDR_WorkFlash_S0  // 0x01h, 2byte
#define CoreAssemblyAddr                    ADDR_WorkFlash_S1  // 0x31h, 25byte
#define DeliveryAssemblyAddr                ADDR_WorkFlash_S2  // 0x32h, 25byte
#define MainCalibrationFordPartNumberAddr   ADDR_WorkFlash_S3  // 0x35h, 25byte
#define ErrorBackupAddr                     ADDR_WorkFlash_S4  // 
#define SoftwareFordPartNumberAddr          ADDR_WorkFlash_S5  // 0x33h, 25byte
/*Bootloader use S6,s7,s8*/
#define SerialNumAddr                       ADDR_WorkFlash_S9  // 0x33  /* Leo 20220621 */
// #define PowerErrorBackUp 0x1400001A  TBD



//----------------------- Declare Function ------------------------------------//
void Flash_Initialize(void);
uint8_t Flash_Sector_Erase(uint32_t SectorAddr);
uint8_t Flash_Work_Sector_Write(uint32_t SectorAddr, uint32_t SectorSizeInByte, uint8_t * p_ProgramBuffData, uint32_t DataSizeInByte);
void Flash_Work_Sector_Read(uint32_t SectorAddr, uint32_t SectorSizeInByte, uint8_t * p_ProgramBuffData, uint32_t DataSizeInByte);

#endif /* FLASHAPP_H_ */
