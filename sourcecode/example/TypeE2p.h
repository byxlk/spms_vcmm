#include "TypeMeter.h"
//#define CMonItems	123		//��ǰ�µ���������������ʱ��������
//#define	CMonLength	574		//��ǰ�µ���������������ʱ���ܳ���

//����洢���б��浱ǰ���е�������һ������(ռǰ4ҳ 4*2*128 )
//�͵�ǰ�й����򡢵�ǰ�й����򡢵�ǰ���ڡ�һ����������������������������60���ӡ�120�ֽڣ����ޱ���(��4ҳ 4*2*128)
/***********************************************/

// ��һƬ24C512�д洢�����ݣ�
// 1����ǰ���е�������������������ʱ�䣻
// 2����ʷ���1-12���µ����е�������������������ʱ��
// 3���ڼ��ա������ա����ڡ�ʱ�εȵ�����
// 4��������Ȳ���
//У����� ����ǰ100�ֽ�
#define EE_CAL_IA			 0x0000			//IA(3)
#define EE_CAL_IB			 EE_CAL_IA+3		//IB(3)
#define EE_CAL_PA			 EE_CAL_IA+3*2		//PA(3)
#define EE_CAL_PB			 EE_CAL_IA+3*3		//PB(3)
#define EE_CAL_V			 EE_CAL_IA+3*4		//V(3)
#define EE_PHADJ_A			 EE_CAL_IA+3*5		//A����λ(3)
#define EE_PHADJ_B			 EE_CAL_IA+3*6		//B����λ(3)
#define EE_Offset_A			 EE_CAL_IA+3*7		//A��Offset(3)
#define EE_Offset_B			 EE_CAL_IA+3*8		//B��Offset(3)
#define EMUStatusJY			 EE_Offset_B+3		//У���(5)  
#define	ClockAdj			 EMUStatusJY+5				//��2��//������ַ:0x088D


#define IEC_LINK_ADDR                    ClockAdj+6      //2
#define IEC_COMM_ADDR                    IEC_LINK_ADDR+4  //2
#define	LoadRecord_Time		         IEC_COMM_ADDR+4 //����ȥ��ʱ�䣬ָ���¼

#define LOAD_RECORD_NUM                  96
#define ONE_RECORD_SIZE                  40
#define LOAD_RECORD_SIZE                 (LOAD_RECORD_NUM*ONE_RECORD_SIZE)



#define E2P_PGLEN		0x1000//0x0B00//0xAAA//0x1554		//���ݱ���ҳ����


#define FlashChip		YesCheck



///////////   FLASH	//ǰ300ҳΪ���ɼ�¼
#if (FlashChip == YesCheck)
#define	Mon_Data_Addr	 0x25800//0x19000	//(1840)��ҳ��
#else 
#define	Mon_Data_Addr	 0x7000///0x1554//0x19000	//(1840)��ҳ��
#endif
#define	Tamper_Data_Addr	 Mon_Data_Addr + 0xA00	//(6000)



#define FlashAT45DB		NoCheck		//ATMEL��AT25DF041Aϵ��оƬ AT26DF081��AT26DF161
#define FlashAT161Series	YesCheck	//ATMEL��AT45161оƬ ����2M�ֽ�

//#define GeneralLoadCurveSta		0x00000000		//0
//#define GeneralLoadCurveEnd		0x00090000		//384K  ��ͨ�������߷�Χ��
//#define GeneralMonCurveSta		0x00090000		//0
//#define GeneralMonCurveEnd		0x000A0000		//64K  ��ʷ���ݷ�Χ��
//#define GeneralTamperCurveSta	0x000A0000		//0
//#define GeneralTamperCurveEnd	0x000B0000		//64K  �¼���¼��Χ��

//------------------DLMS Profile tab���ݴ洢----------------
#define EventLog1Page					0x00000000
///#define EventLog1Page					GeneralTamperCurveEnd			//�¼�1��¼��ҳ��(6)
#define EventLog2Page					EventLog1Page+6*1						//�¼�2��¼��ҳ��(6)
#define EventLog3Page					EventLog1Page+6*2						//�¼�3��¼��ҳ��(6)
#define EventLog4Page					EventLog1Page+6*3						//�¼�4��¼��ҳ��(6)
#define EventLog5Page					EventLog1Page+6*4						//�¼�5��¼��ҳ��(6)
#define EventLog6Page					EventLog1Page+6*5						//�¼�6��¼��ҳ��(6)
#define EventLog7Page					EventLog1Page+6*6						//�¼�7��¼��ҳ��(6)
#define EventLog8Page					EventLog1Page+6*7						//�¼�8��¼��ҳ��(6)
#define EventLog9Page					EventLog1Page+6*8						//�¼�9��¼��ҳ��(6)
#define EventLog10Page					EventLog1Page+6*9						//�¼�10��¼��ҳ��(6)
#define EventLog11Page					EventLog1Page+6*10					//�¼�11��¼��ҳ��(6)

#define Billing_period1Page				EventLog11Page+6*1				//�˵���¼1��ҳ��(6)		//12���½���
#define Billing_period2Page				EventLog11Page+6*2			//�˵���¼1��ҳ��(20)		//31�춳��
#define Load_profile1Page				EventLog11Page+6*3			//��������1��ҳ��(20)		
#define Load_profile2Page				Load_profile1Page+40				//��������1��ҳ��(20)	
#define PowerQuality_profilePage			Load_profile2Page+6				//��������2��ҳ��(200)		

#define Profile_Page_end						PowerQuality_profilePage+40

#define GeneralMonCurveSta		Profile_Page_end		//0
#define GeneralMonCurveEnd		Profile_Page_end+0x10000		//64K  ��ʷ���ݷ�Χ��
#define GeneralTamperCurveSta	GeneralMonCurveEnd		//0
//#define GeneralTamperCurveEnd	0x000E0000		//64K  �¼���¼��Χ��
#define GeneralTamperCurveEnd	GeneralTamperCurveSta+0x10000		//64K  �¼���¼��Χ��
#define GeneralLoadCurveSta		GeneralTamperCurveEnd		//0
#define GeneralLoadCurveEnd		GeneralLoadCurveSta+0x60000		//384K  ��ͨ�������߷�Χ��


//�����ǰ���4MFLASH���в����ģ��������ַŵ�FLASH�����512K�ֽڲ���..
//����FLASH�洢�ռ䣬Ŀǰ����ռ�ΪAT25DF041A�����256*2+4��K =516K�ֽڵĿռ䡣
//��FLASHÿһҳΪ4K��4096�ֽڣ��߳�Ҳֻ����4Kһ�߳�
// 2 BYTE
#define FLASH_512	 0x200  			//512..
#define FLASH_4K	 0x1000  			//4096  = 4*1024..
#define FLASH_16K	 0x4000  			//16384 = 16*1024..
// 3 BYTE
#define FLASH_64K	 0x10000
#define FLASH_252K	 0x3F000			//252K..
#define FLASH_256K	 0x40000			//256K..
#define FLASH_184K	 0x2E000	//8213�ڲ�flash�洢�û�����Ŀռ�192K-8K=184K

//�����������--����..
//895 37F000h 37FFFFh
#define NEW_FIRMWARE_STATE			(unsigned long)(FLASH_4K)*383 				// 37F000h = 895*4K(1024) //������ʼ����ǰ��4���ֽڳ�ʼ��Ϊ��0xA5 B6 C7 D8
// page
// 4K = 4096
// ����������ʱ����Ϊ��128+252K�ֽڣ�����8���ֽڵ�CRCУ�飩����Ҫ���䵽256K�Լ�������������Ҫ��ÿ�������4K����252K-->256K..
//��������ĳɿɱ䳤�ȣ�ֻҪ��С��104���ֽ�һ��Ϳ��ԣ���ַ��Ҳ�����������������ʼ�ͽ�����ַ..
// NEW FIRMWARE UPGRADE -..
#define UPGRADE_NEW_START				(unsigned long)(FLASH_4K)*384					// 0x180000 = 384*4K(1024) 
//4k --��ʱ������128�ֽ��ã��Լ��汾��Ϣ��..
#define UPGRADE_START_PROGRAM		UPGRADE_NEW_START + FLASH_4K 					// 388*4K(1024)
#define UPGRADE_VECTOR_START 	 	UPGRADE_NEW_START + 73*FLASH_512 			// �ж���������ʼ��ַ..
#define UPGRADE_VECTOR_END 	    	UPGRADE_NEW_START + 74*FLASH_512 			// �ж���������Ҳ��0x10000��ַ�Ŀ�ʼ <1---74>..
//<75---504>.. 
#define UPGRADE_END_PROGRAM			UPGRADE_START_PROGRAM + FLASH_252K 		// 898--959 //4K(1024)

// ��������ǰ�ĳ��� 0LD FIRMWARE
#define CURRENT_NEW_START				(unsigned long)(FLASH_4K)*448					//(384+64)*FLASH_4K) 									
// 4k---��ŵ�ǰ�ļ�����ʹ��128���ֽ�ͷ�ļ���ֱ�Ӵ�ŵ�ǰ512�ֽڵ�Ӧ�ó����ж��������߳�Ӧ�ó���..
#define CURRENT_START_PROGRAM		CURRENT_NEW_START + FLASH_4K 					// 452*4K(1024)

#define CURRENT_VECTOR_START 	    CURRENT_NEW_START + 73*FLASH_512 			// ��ǰ�ж���������ʼ��ַ..
#define CURRENT_VECTOR_END 	    	CURRENT_NEW_START + 74*FLASH_512 			// ��ǰ�ж���������Ҳ��0x10000��ַ�Ŀ�ʼ <1---74>..
//<75---504> ��һ����504��512���ֽ�����..
#define CURRENT_END_PROGRAM			CURRENT_START_PROGRAM + FLASH_252K 		// 898--959 //4K(1024)

