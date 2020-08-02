/*
*********************************************************************************************************
*                                          HT6XXX
*                                       HT6XXX程序
*
*                             Copyright 2013, Hi-Trend Tech, Corp.
*                                    All Rights Reserved
*
*
* Project      : HT6xxx
* File         : main.c
* By           : Hitrendtech_SocTeam
* Version      : V1.0.1
* Description  :
*********************************************************************************************************
*/

#define  __MAIN_C

#include <stdio.h>
#include "ht6xxx_lib.h"

/*
*********************************************************************************************************
*                                           本地宏/结构体
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            本地变量
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           本地函数申明
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              MAIN
*
*********************************************************************************************************
*/

int main(void)
{
    /*  SystemInit() has been called before enter main() */

    /*  Add your application code here  */

    /*  Infinite loop  */
    while (1)
    {
        HT_FreeDog();
        
    }
}


 /*
*********************************************************************************************************
*                                             ASSERT
*
*********************************************************************************************************
*/
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */

  while (1)
  {

  }
}
#endif

#if 0
/********************************************************************************************
                                    HT6x1x
                                    HT6015_HT7017_Demo板程序

版本形式：寄存器法
版权所有：上海本宏电子科技有限公司
项目名称：HT6015_Demo板测试程序——实现HT6015与HT7017之间的UART通信，通过COMChecksum、ChipID、
         DeviceID三个寄存器来确定
项目说明：用来检测HT6015与HT7017的UART通信是否正常
系统时钟：系统时钟采用外部晶振，倍频后为22.020096Mhz,其中CPU和GPIO经过分频后变为11Mhz，UART使用22.020096Mhz
测试现象：

特别说明：这里要将TIMER0所复用的IO口设置为复用TMR模式，CLKOUT所复用的IO口复用为CLKOUT功能
程序版本：HOBOS_HT6X1X_V1.0
项目日期：2017-2-8
编写人员：Bsyang
其他说明：版权归上海本宏电子科技有限公司所有，盗版必究
********************************************************************************************/
#include "ht6xxx.h"

#define uint   unsigned int
#define INT8U  unsigned char
#define INT16U unsigned short
#define INT32U unsigned long

#define	C_TaskTick		64u					//任务定时节拍频率（64Hz）
#define	C_ByteGap			(200u*C_TaskTick/1000)			//通讯字节间隔200毫秒

//计量参数寄存器
#define Rms_U           0x08            //电压通道的有效值
#define Freq_U          0x09            //电压频率
#define COMChecksum     0x17            //通信校验和寄存器
#define ChipID          0x1B            //HT7017的ChipID，默认值为7053B0
#define DeviceID        0x1C            //HT7017的DeviceID，默认值705304

INT8U g_ComBuf[256];    //通讯帧缓冲区
INT8U g_ComAdr;         //通讯帧指针地址
INT8U g_ComGap;

INT32U U_Rms        = 0;             //表示电压通道的有效值
INT32U U_Freq       = 0;             //表示电压频率
INT32U COM_Checksum = 0;             //表示通信校验和寄存器
INT32U Chip_ID      = 0;             //表示ChipID
INT32U Device_ID    = 0;             //表示DeviceID
INT32U  WPCFG,HFConst;




typedef int     BOOL;//布尔型
#define TRUE   (INT8U)0xAA//逻辑假
#define  FALSE (INT8U)0x55//逻辑真
//typedef	enum
//{
//	FALSE	= (INT8U)0x55,		//逻辑假
//	TRUE	= (INT8U)0xAA,		//逻辑真
//}BOOL;





//延时函数
void MY_Delay(uint z)
{
    uint i,j;
    for(i=500;i>0;i--)
    for(j=z;j>0;j--);
}


//Switch to Fpll(Fsys:22MHZ;CPU/GPIO:11MHZ)
void SwitchTo_Fpll(void)
{
    HT_CMU->WPREG = 0xA55A;
    
    HT_CMU->CLKCTRL0 |= 0x0010;
    HT_CMU->SYSCLKDIV = 0x0001;
     
    HT_CMU->SYSCLKCFG = 0x0083;
    HT_CMU->SYSCLKCFG = 0x0003;   
    HT_CMU->WPREG     = 0x0000;
}
/*******************************************************************************
功能描述：	开启UART5模块
输入参数：	head:		首字节
返回参数：
函数说明：	EMU
*******************************************************************************/
void Open_UART5(INT8U head)
{
    NVIC_DisableIRQ(UART5_IRQn);				
	
    HT_GPIOC->IOCFG |= 0x0030;
    HT_GPIOC->AFCFG |= 0x0030;

    HT_CMU->WPREG   = 0xA55A;         
    HT_CMU->CLKCTRL1  = 0x0200;         
    HT_CMU->WPREG   = 0x0000;          
    
    //配置UART5通信格式
    HT_UART5->MODESEL = 0x0000;					
    HT_UART5->UARTCON = 0x005B;					
	HT_UART5->SREL    = 0x08F5;				
	
	HT_UART5->SBUF    = head;
}
/*******************************************************************************
功能描述：	关闭UART5模块
输入参数：
返回参数：
函数说明：	EMU
*******************************************************************************/
void Close_UART5(void)
{
	NVIC_DisableIRQ(UART5_IRQn);				//禁止串口中断
	HT_UART5->UARTCON = 0x0000;					//禁止串口功能
}

INT32U Read_Reg(INT8U addr)
{
	INT8U	i, sum;
	
	g_ComBuf[0] = 0x6A;                                     
	g_ComBuf[1] = addr & 0x7F;                            
	for (i=0; i<3; i++)                                   
	{
		g_ComGap = 0;                                      
		g_ComAdr = 1;                                     
		Open_UART5(g_ComBuf[0]);							
		while ((g_ComAdr < 6)&& (g_ComGap <= C_ByteGap))  
		{
			if (HT_UART5->UARTSTA & 0x0001)                
			{
				HT_UART5->UARTSTA &= ~0x0001;       
				g_ComGap = 0;							
				if (g_ComAdr < 2)						
				{
					HT_UART5->SBUF = g_ComBuf[g_ComAdr++];
				}
			}
			if (HT_UART5->UARTSTA & 0x0002)					
			{
				HT_UART5->UARTSTA &= ~0x0002;              
				g_ComGap = 0;								
				if (g_ComAdr < 6)
				{
					g_ComBuf[g_ComAdr++] = HT_UART5->SBUF;
				}
			}
		}
		Close_UART5();										
		if (g_ComAdr == 6)
		{
			sum = g_ComBuf[0] +g_ComBuf[1] +g_ComBuf[2] +g_ComBuf[3] +g_ComBuf[4];
			sum = ~sum;
			if (sum == g_ComBuf[5])
			{
				return (g_ComBuf[2]<<16 | g_ComBuf[3]<<8 | g_ComBuf[4]);
			}
		}
	}
	return 0x00000000;
}

BOOL Write_Reg(INT8U addr, INT16U para)
{
	INT8U	i, sum;
	
	g_ComBuf[0] = 0x6A;
	g_ComBuf[1] = addr | 0x80;
	g_ComBuf[2] = para>>8;
	g_ComBuf[3] = para;
	sum	= g_ComBuf[0] +g_ComBuf[1] +g_ComBuf[2] +g_ComBuf[3];
	g_ComBuf[4] = ~sum;
	g_ComBuf[5] = 0x00;
	for (i=0; i<3; i++)
	{
		g_ComGap = 0;
		g_ComAdr = 1;
		Open_UART5(g_ComBuf[0]);							
		while ((g_ComAdr < 6)&& (g_ComGap <= C_ByteGap))
		{
			if (HT_UART5->UARTSTA & 0x0001)
			{
				HT_UART5->UARTSTA &= ~0x0001;
				g_ComGap = 0;								
				if (g_ComAdr < 5)							
				{
					HT_UART5->SBUF = g_ComBuf[g_ComAdr++];
				}
			}
			if (HT_UART5->UARTSTA & 0x0002)				
			{
				HT_UART5->UARTSTA &= ~0x0002;
				g_ComGap = 0;								
				if (g_ComAdr < 6)
				{
					g_ComBuf[g_ComAdr++] = HT_UART5->SBUF;
				}
			}
		}
		Close_UART5();										
		if (g_ComAdr == 6)
		{
			if (g_ComBuf[5] == 0x54)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

void ht7017_check(void)
{
	Write_Reg(0x33, 0x55);							//软复位HT7017
	MY_Delay(2000);									//充分延时
	Write_Reg(0x32, 0xBC);							//写保护寄存器打开
	MY_Delay(2000);									//充分延时
    
    
    /*将“ATT7053BU HT7017功率校表法_V0.5.pdf”文档的校表流程添加进来即可*/
    /*细节上的问题可联系www.hobos.com.cn或yangbaoshan@hobos.com.cn*/
    
	Write_Reg(0x32, 0xA6);							//写保护寄存器关闭
	MY_Delay(2000);									//充分延时
	Write_Reg(0x61, 0x55);							//输出脉冲频率设置
	MY_Delay(2000);									//充分延时
}

int main(void)
{
    SwitchTo_Fpll();
	ht7017_check();
	
    while(1)
    {
        U_Freq       = Read_Reg(Freq_U);//读取HT7017的电压频率        
        MY_Delay(10);
        U_Rms        = Read_Reg(Rms_U);//读取电压通道的有效值
        MY_Delay(10);
        COM_Checksum = Read_Reg(COMChecksum);//读取通信校验和寄存器
        MY_Delay(10);
        Chip_ID = Read_Reg(ChipID);//读取HT7017的ChipID，默认值为7053B0
        MY_Delay(10);
        Device_ID = Read_Reg(DeviceID);//读取HT7017的DeviceID，默认值705304
        MY_Delay(10);
		
		WPCFG = Read_Reg(0x32);//读取HT7017的写保护寄存器
        MY_Delay(10);
		HFConst = Read_Reg(0x61);//读取HT7017的输出脉冲频率设置
        MY_Delay(10);
    }
}

#endif
