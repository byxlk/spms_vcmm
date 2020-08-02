/*
*********************************************************************************************************
*                                          HT60XX
*                                       HT60XX程序
*
*                             Copyright 2013, Hi-Trend Tech, Corp.
*                                    All Rights Reserved
*
*
* Project      : HT60xx
* File         : ht60xx_it.c
* By           : Hitrendtech_SocTeam
* Version      : V1.0.5
* Description  :
*********************************************************************************************************
*/

#define  __HT60XX_IT_C

#include "ht6xxx_lib.h"
#include "ht6xxx_it.h"

/*
*********************************************************************************************************
*                                            本地宏/结构体
*********************************************************************************************************
*/
#define RX_Head1    0xAA
#define RX_Head2    0x55
#define RX_Over     0x16

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
*                                        NMI INTERRUPT HANDLER #16-14
*
*********************************************************************************************************
*/
void NMI_Handler(void)
{
    if (SET == HT_CMU_StopFlagGet(CMU_CLKSTA_LFFLAG))
    {

    }

    if (SET == HT_CMU_StopFlagGet(CMU_CLKSTA_HRCFLAG))
    {

    }

    if (SET == HT_CMU_StopFlagGet(CMU_CLKSTA_PLLFLAG))
    {

    }
}

/*
*********************************************************************************************************
*                                        HARDFAULT INTERRUPT HANDLER #16-13
*
*********************************************************************************************************
*/
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {

    }
}

/*
*********************************************************************************************************
*                                        SVC INTERRUPT HANDLER #16-5
*
*********************************************************************************************************
*/
void SVC_Handler(void)
{

}

/*
*********************************************************************************************************
*                                        PENDSV INTERRUPT HANDLER #16-2
*
*********************************************************************************************************
*/
void PendSV_Handler(void)
{

}

/*
*********************************************************************************************************
*                                        SYSTICK INTERRUPT HANDLER #16-1
*
*********************************************************************************************************
*/
void SysTick_Handler(void)
{

}

/*
*********************************************************************************************************
*                                        PMU INTERRUPT HANDLER #16+0
*
*********************************************************************************************************
*/

void PMU_IRQHandler()
{
    if(SET == HT_PMU_ITFlagStatusGet(PMU_PMUIF_VCCIF))           /*!< Vcc检测中断置位            */
    {

        HT_PMU_ClearITPendingBit(PMU_PMUIF_VCCIF);               /*!< 清除中断标志               */
    }

    if(SET == HT_PMU_ITFlagStatusGet(PMU_PMUIF_BORIF))           /*!< Bor检测中断置位            */
    {

        HT_PMU_ClearITPendingBit(PMU_PMUIF_BORIF);               /*!< 清除中断标志               */
    }

    if(SET == HT_PMU_ITFlagStatusGet(PMU_PMUIF_LVD0IF))           /*!< Lvdin0检测中断置位          */
    {

        HT_PMU_ClearITPendingBit(PMU_PMUIF_LVD0IF);               /*!< 清除中断标志               */
    }
 
#if  defined  HT6x2x  ||  defined  HT6x3x  ||  defined  HT501x  ||  defined  HT502x    
    if(SET == HT_PMU_ITFlagStatusGet(PMU_PMUIF_LVD1IF))           /*!< Lvdin1检测中断置位          */
    {

        HT_PMU_ClearITPendingBit(PMU_PMUIF_LVD1IF);               /*!< 清除中断标志               */
    }
#endif
    
#if  defined  HT6x3x
    if(SET == HT_PMU_ITFlagStatusGet(PMU_PMUIF_LVD2IF))           /*!< Lvdin2检测中断置位          */
    {

        HT_PMU_ClearITPendingBit(PMU_PMUIF_LVD2IF);               /*!< 清除中断标志               */
    }
#endif
}

/*
*********************************************************************************************************
*                                  3DES/AES INTERRUPT HANDLER  #16+1
*
*********************************************************************************************************
*/
#if defined HT6x1x
void TDES_IRQHandler()                      /*!< 3DES               */
{

}
#else
void AES_IRQHandler()                       /*!< AES                */
{

}
#endif

/*
*********************************************************************************************************
*                                   EXTI0 INTERRUPT HANDLER  #16+2
*
*********************************************************************************************************
*/

void EXTI0_IRQHandler()
{
    if(SET == HT_EXTIRise_ITFlagStatusGet(INT_EXTIF_RIF_INT0))         /*!< INT0上升沿中断           */
    {

        HT_EXTIRise_ClearITPendingBit(INT_EXTIF_RIF_INT0);             /*!< 清除中断标志             */
    }

    if(SET == HT_EXTIFall_ITFlagStatusGet(INT_EXTIF_FIF_INT0))         /*!< INT0下降沿中断           */
    {

        HT_EXTIFall_ClearITPendingBit(INT_EXTIF_FIF_INT0);             /*!< 清除中断标志             */
    }
}

/*
*********************************************************************************************************
*                                   EXTI1 INTERRUPT HANDLER  #16+3
*
*********************************************************************************************************
*/

void EXTI1_IRQHandler()
{
    if(SET == HT_EXTIRise_ITFlagStatusGet(INT_EXTIF_RIF_INT1))         /*!< INT1上升沿中断           */
    {

        HT_EXTIRise_ClearITPendingBit(INT_EXTIF_RIF_INT1);             /*!< 清除中断标志             */
    }

    if(SET == HT_EXTIFall_ITFlagStatusGet(INT_EXTIF_FIF_INT1))         /*!< INT1下降沿中断           */
    {

        HT_EXTIFall_ClearITPendingBit(INT_EXTIF_FIF_INT1);             /*!< 清除中断标志             */
    }
}

/*
*********************************************************************************************************
*                                   EXTI2 INTERRUPT HANDLER  #16+4
*
*********************************************************************************************************
*/

void EXTI2_IRQHandler()
{
    if(SET == HT_EXTIRise_ITFlagStatusGet(INT_EXTIF_RIF_INT2))         /*!< INT2上升沿中断           */
    {

        HT_EXTIRise_ClearITPendingBit(INT_EXTIF_RIF_INT2);             /*!< 清除中断标志             */
    }

    if(SET == HT_EXTIFall_ITFlagStatusGet(INT_EXTIF_FIF_INT2))         /*!< INT2下降沿中断           */
    {

        HT_EXTIFall_ClearITPendingBit(INT_EXTIF_FIF_INT2);             /*!< 清除中断标志             */
    }
}

/*
*********************************************************************************************************
*                                   EXTI3 INTERRUPT HANDLER  #16+5
*
*********************************************************************************************************
*/

void EXTI3_IRQHandler()
{
    if(SET == HT_EXTIRise_ITFlagStatusGet(INT_EXTIF_RIF_INT3))         /*!< INT3上升沿中断           */
    {

        HT_EXTIRise_ClearITPendingBit(INT_EXTIF_RIF_INT3);             /*!< 清除中断标志             */
    }

    if(SET == HT_EXTIFall_ITFlagStatusGet(INT_EXTIF_FIF_INT3))         /*!< INT3下降沿中断           */
    {

        HT_EXTIFall_ClearITPendingBit(INT_EXTIF_FIF_INT3);             /*!< 清除中断标志             */
    }
}

/*
*********************************************************************************************************
*                                   EXTI4 INTERRUPT HANDLER  #16+6
*
*********************************************************************************************************
*/

void EXTI4_IRQHandler()
{
    if(SET == HT_EXTIRise_ITFlagStatusGet(INT_EXTIF_RIF_INT4))         /*!< INT4上升沿中断           */
    {

        HT_EXTIRise_ClearITPendingBit(INT_EXTIF_RIF_INT4);             /*!< 清除中断标志             */
    }

    if(SET == HT_EXTIFall_ITFlagStatusGet(INT_EXTIF_FIF_INT4))         /*!< INT4下降沿中断           */
    {

        HT_EXTIFall_ClearITPendingBit(INT_EXTIF_FIF_INT4);             /*!< 清除中断标志             */
    }
}

/*
*********************************************************************************************************
*                                   EXTI5 INTERRUPT HANDLER  #16+7
*
*********************************************************************************************************
*/

void EXTI5_IRQHandler()
{
    if(SET == HT_EXTIRise_ITFlagStatusGet(INT_EXTIF_RIF_INT5))         /*!< INT5上升沿中断           */
    {

        HT_EXTIRise_ClearITPendingBit(INT_EXTIF_RIF_INT5);             /*!< 清除中断标志             */
    }

    if(SET == HT_EXTIFall_ITFlagStatusGet(INT_EXTIF_FIF_INT5))         /*!< INT5下降沿中断           */
    {

        HT_EXTIFall_ClearITPendingBit(INT_EXTIF_FIF_INT5);             /*!< 清除中断标志             */
    }
}

/*
*********************************************************************************************************
*                                   EXTI6 INTERRUPT HANDLER  #16+8
*
*********************************************************************************************************
*/

void EXTI6_IRQHandler()
{
    if(SET == HT_EXTIRise_ITFlagStatusGet(INT_EXTIF_RIF_INT6))         /*!< INT6上升沿中断           */
    {

        HT_EXTIRise_ClearITPendingBit(INT_EXTIF_RIF_INT6);             /*!< 清除中断标志             */
    }

    if(SET == HT_EXTIFall_ITFlagStatusGet(INT_EXTIF_FIF_INT6))         /*!< INT6下降沿中断           */
    {

        HT_EXTIFall_ClearITPendingBit(INT_EXTIF_FIF_INT6);             /*!< 清除中断标志             */
    }
}

/*
*********************************************************************************************************
*                                   UART0 INTERRUPT HANDLER  #16+9
*
*********************************************************************************************************
*/

void UART0_IRQHandler()
{

    if(SET == HT_UART_ITFlagStatusGet(HT_UART0, UART_UARTSTA_TXIF))         /*!< UART0发送中断         */
    {

        HT_UART_ClearITPendingBit(HT_UART0, UART_UARTSTA_TXIF);             /*!< 清除中断标志          */
    }

    if(SET == HT_UART_ITFlagStatusGet(HT_UART0, UART_UARTSTA_RXIF))         /*!< UART0接收中断         */
    {

        HT_UART_ClearITPendingBit(HT_UART0, UART_UARTSTA_RXIF);             /*!< 清除中断标志          */

    }
}

/*
*********************************************************************************************************
*                                   UART1 INTERRUPT HANDLER  #16+10
*
*********************************************************************************************************
*/

void UART1_IRQHandler()
{
    if(SET == HT_UART_ITFlagStatusGet(HT_UART1, UART_UARTSTA_TXIF))         /*!< UART1发送中断         */
    {

        HT_UART_ClearITPendingBit(HT_UART1, UART_UARTSTA_TXIF);             /*!< 清除中断标志          */
    }

    if(SET == HT_UART_ITFlagStatusGet(HT_UART1, UART_UARTSTA_RXIF))         /*!< UART1接收中断         */
    {

        HT_UART_ClearITPendingBit(HT_UART1, UART_UARTSTA_RXIF);             /*!< 清除中断标志          */
    }
}

/*
*********************************************************************************************************
*                                   UART2 INTERRUPT HANDLER  #16+11
*
*********************************************************************************************************
*/

void UART2_IRQHandler()
{
    if(SET == HT_UART_ITFlagStatusGet(HT_UART2, UART_UARTSTA_TXIF))         /*!< UART2发送中断         */
    {

        HT_UART_ClearITPendingBit(HT_UART2, UART_UARTSTA_TXIF);             /*!< 清除中断标志          */
    }

    if(SET == HT_UART_ITFlagStatusGet(HT_UART2, UART_UARTSTA_RXIF))         /*!< UART2接收中断         */
    {

        HT_UART_ClearITPendingBit(HT_UART2, UART_UARTSTA_RXIF);             /*!< 清除中断标志          */
    }
}

/*
*********************************************************************************************************
*                                   UART3 INTERRUPT HANDLER  #16+12
*
*********************************************************************************************************
*/

void UART3_IRQHandler()
{

    if(SET == HT_UART_ITFlagStatusGet(HT_UART3, UART_UARTSTA_TXIF))            /*!< UART3发送中断         */
    {

        HT_UART_ClearITPendingBit(HT_UART3, UART_UARTSTA_TXIF);                /*!< 清除中断标志          */
    }

    if(SET == HT_UART_ITFlagStatusGet(HT_UART3, UART_UARTSTA_RXIF))            /*!< UART3接收中断         */
    {

        HT_UART_ClearITPendingBit(HT_UART3, UART_UARTSTA_RXIF);                /*!< 清除中断标志          */
    }

    if(SET == HT_ISO7816_ITFlagStatusGet(HT_ISO7816_1, UART_ISO7816STA_TXIF))  /*!< 7816发送中断          */
    {

        HT_ISO7816_ClearITPendingBit(HT_ISO7816_1, UART_ISO7816STA_TXIF);      /*!< 清除中断标志          */
    }

    if(SET == HT_ISO7816_ITFlagStatusGet(HT_ISO7816_1, UART_ISO7816STA_RXIF))  /*!< 7816接收中断          */
    {

        HT_ISO7816_ClearITPendingBit(HT_ISO7816_1, UART_ISO7816STA_RXIF);      /*!< 清除中断标志          */
    }

    if(SET == HT_ISO7816_ITFlagStatusGet(HT_ISO7816_1, UART_ISO7816STA_PRDIF)) /*!< 7816接收上溢中断标志  */
    {

        HT_ISO7816_ClearITPendingBit(HT_ISO7816_1, UART_ISO7816STA_PRDIF);     /*!< 清除中断标志          */
    }
}

/*
*********************************************************************************************************
*                                   UART4 INTERRUPT HANDLER  #16+13
*
*********************************************************************************************************
*/

void UART4_IRQHandler()
{
    if(SET == HT_UART_ITFlagStatusGet(HT_UART4, UART_UARTSTA_TXIF))            /*!< UART4发送中断         */
    {

        HT_UART_ClearITPendingBit(HT_UART4, UART_UARTSTA_TXIF);                /*!< 清除中断标志          */
    }

    if(SET == HT_UART_ITFlagStatusGet(HT_UART4, UART_UARTSTA_RXIF))            /*!< UART4接收中断         */
    {

        HT_UART_ClearITPendingBit(HT_UART4, UART_UARTSTA_RXIF);                /*!< 清除中断标志          */
    }

    if(SET == HT_ISO7816_ITFlagStatusGet(HT_ISO7816_0, UART_ISO7816STA_TXIF))  /*!< 7816发送中断          */
    {

        HT_ISO7816_ClearITPendingBit(HT_ISO7816_0, UART_ISO7816STA_TXIF);      /*!< 清除中断标志          */
    }

    if(SET == HT_ISO7816_ITFlagStatusGet(HT_ISO7816_0, UART_ISO7816STA_RXIF))  /*!< 7816接收中断          */
    {

        HT_ISO7816_ClearITPendingBit(HT_ISO7816_0, UART_ISO7816STA_RXIF);      /*!< 清除中断标志          */
    }

    if(SET == HT_ISO7816_ITFlagStatusGet(HT_ISO7816_0, UART_ISO7816STA_PRDIF)) /*!< 7816接收上溢中断标志  */
    {

        HT_ISO7816_ClearITPendingBit(HT_ISO7816_0, UART_ISO7816STA_PRDIF);     /*!< 清除中断标志          */
    }
}

/*
*********************************************************************************************************
*                                   UART5 INTERRUPT HANDLER  #16+14
*
*********************************************************************************************************
*/

void UART5_IRQHandler()
{
    if(SET == HT_UART_ITFlagStatusGet(HT_UART5, UART_UARTSTA_TXIF))         /*!< UART5发送中断         */
    {

        HT_UART_ClearITPendingBit(HT_UART5, UART_UARTSTA_TXIF);             /*!< 清除中断标志          */
    }

    if(SET == HT_UART_ITFlagStatusGet(HT_UART5, UART_UARTSTA_RXIF))         /*!< UART5接收中断         */
    {

        HT_UART_ClearITPendingBit(HT_UART5, UART_UARTSTA_RXIF);             /*!< 清除中断标志          */
    }
}

/*
*********************************************************************************************************
*                                   TIMER_0 INTERRUPT HANDLER  #16+15
*
*********************************************************************************************************
*/

void TIMER_0_IRQHandler()
{

    if(SET == HT_TMR_ITFlagStatusGet(HT_TMR0, TMR_TMRIF_PRDIF))                /*!< 周期中断           */
    {

        HT_TMR_ClearITPendingBit(HT_TMR0, TMR_TMRIF_PRDIF);                    /*!< 清除中断标志       */
    }

    if(SET == HT_TMR_ITFlagStatusGet(HT_TMR0, TMR_TMRIF_CAPIF))                /*!< 捕获中断           */
    {

        HT_TMR_ClearITPendingBit(HT_TMR0, TMR_TMRIF_CAPIF);                    /*!< 清除中断标志       */
    }

    if(SET == HT_TMR_ITFlagStatusGet(HT_TMR0, TMR_TMRIF_CMPIF))                /*!< 比较中断           */
    {

        HT_TMR_ClearITPendingBit(HT_TMR0, TMR_TMRIF_CMPIF);                    /*!< 清除中断标志       */
    }

}

/*
*********************************************************************************************************
*                                   TIMER_1 INTERRUPT HANDLER  #16+16
*
*********************************************************************************************************
*/

void TIMER_1_IRQHandler()
{

    if(SET == HT_TMR_ITFlagStatusGet(HT_TMR1, TMR_TMRIF_PRDIF))                /*!< 周期中断           */
    {

        HT_TMR_ClearITPendingBit(HT_TMR1, TMR_TMRIF_PRDIF);                    /*!< 清除中断标志       */
    }

    if(SET == HT_TMR_ITFlagStatusGet(HT_TMR1, TMR_TMRIF_CAPIF))                /*!< 捕获中断           */
    {

        HT_TMR_ClearITPendingBit(HT_TMR1, TMR_TMRIF_CAPIF);                    /*!< 清除中断标志       */
    }

    if(SET == HT_TMR_ITFlagStatusGet(HT_TMR1, TMR_TMRIF_CMPIF))                /*!< 比较中断           */
    {

        HT_TMR_ClearITPendingBit(HT_TMR1, TMR_TMRIF_CMPIF);                    /*!< 清除中断标志       */
    }

}

/*
*********************************************************************************************************
*                                   TIMER_2 INTERRUPT HANDLER  #16+17
*
*********************************************************************************************************
*/

void TIMER_2_IRQHandler()
{

    if(SET == HT_TMR_ITFlagStatusGet(HT_TMR2, TMR_TMRIF_PRDIF))                /*!< 周期中断           */
    {

        HT_TMR_ClearITPendingBit(HT_TMR2, TMR_TMRIF_PRDIF);                    /*!< 清除中断标志       */
    }

    if(SET == HT_TMR_ITFlagStatusGet(HT_TMR2, TMR_TMRIF_CAPIF))                /*!< 捕获中断           */
    {

        HT_TMR_ClearITPendingBit(HT_TMR2, TMR_TMRIF_CAPIF);                    /*!< 清除中断标志       */
    }

    if(SET == HT_TMR_ITFlagStatusGet(HT_TMR2, TMR_TMRIF_CMPIF))                /*!< 比较中断           */
    {

        HT_TMR_ClearITPendingBit(HT_TMR2, TMR_TMRIF_CMPIF);                    /*!< 清除中断标志       */
    }

}

/*
*********************************************************************************************************
*                                   TIMER_3 INTERRUPT HANDLER  #16+18
*
*********************************************************************************************************
*/

void TIMER_3_IRQHandler()
{

    if(SET == HT_TMR_ITFlagStatusGet(HT_TMR3, TMR_TMRIF_PRDIF))                /*!< 周期中断           */
    {

        HT_TMR_ClearITPendingBit(HT_TMR3, TMR_TMRIF_PRDIF);                    /*!< 清除中断标志       */
    }

    if(SET == HT_TMR_ITFlagStatusGet(HT_TMR3, TMR_TMRIF_CAPIF))                /*!< 捕获中断           */
    {

        HT_TMR_ClearITPendingBit(HT_TMR3, TMR_TMRIF_CAPIF);                    /*!< 清除中断标志       */
    }

    if(SET == HT_TMR_ITFlagStatusGet(HT_TMR3, TMR_TMRIF_CMPIF))                /*!< 比较中断           */
    {

        HT_TMR_ClearITPendingBit(HT_TMR3, TMR_TMRIF_CMPIF);                    /*!< 清除中断标志       */
    }

}

/*
*********************************************************************************************************
*                                     TBS INTERRUPT HANDLER  #16+19
*
*********************************************************************************************************
*/

void TBS_IRQHandler()
{

    if(SET == HT_TBS_ITFlagStatusGet(TBS_TBSIF_TMPIF))                 /*!< 温度测量中断          */
    {

        HT_TBS_ClearITPendingBit(TBS_TBSIF_TMPIF);                     /*!< 清除中断标志          */
    }

    if(SET == HT_TBS_ITFlagStatusGet(TBS_TBSIF_VBATIF))                /*!< 电池测量中断          */
    {

        HT_TBS_ClearITPendingBit(TBS_TBSIF_VBATIF);                    /*!< 清除中断标志          */
    }

    if(SET == HT_TBS_ITFlagStatusGet(TBS_TBSIF_ADC0IF))                /*!< ADC0测量中断          */
    {

        HT_TBS_ClearITPendingBit(TBS_TBSIF_ADC0IF);                    /*!< 清除中断标志          */
    }

    if(SET == HT_TBS_ITFlagStatusGet(TBS_TBSIF_ADC1IF))                /*!< ADC1测量中断          */
    {

        HT_TBS_ClearITPendingBit(TBS_TBSIF_ADC1IF);                    /*!< 清除中断标志          */
    }

    if(SET == HT_TBS_ITFlagStatusGet(TBS_TBSIF_VBATCMPIF))             /*!< 电池电压比较中断      */
    {

        HT_TBS_ClearITPendingBit(TBS_TBSIF_VBATCMPIF);                 /*!< 清除中断标志          */
    }

    if(SET == HT_TBS_ITFlagStatusGet(TBS_TBSIF_VCCIF))                 /*!< 电源测量中断          */
    {

        HT_TBS_ClearITPendingBit(TBS_TBSIF_VCCIF);                     /*!< 清除中断标志          */
    }
    
#if defined  HT501x  ||  defined  HT502x
    if(SET == HT_TBS_ITFlagStatusGet(TBS_TBSIF_ADC2IF))                /*!< ADC2测量中断          */
    {

        HT_TBS_ClearITPendingBit(TBS_TBSIF_ADC2IF);                    /*!< 清除中断标志          */
    }
#endif

#if defined  HT501x
    if(SET == HT_TBS_ITFlagStatusGet(TBS_TBSIF_VREFIF))                /*!< VREF测量中断          */
    {

        HT_TBS_ClearITPendingBit(TBS_TBSIF_VREFIF);                    /*!< 清除中断标志          */
    }
#endif

#if defined  HT6x2x  ||  defined  HT6x3x
    if(SET == HT_TBS_ITFlagStatusGet(TBS_TBSIF_ADC3IF))                /*!< ADC3测量中断          */
    {

        HT_TBS_ClearITPendingBit(TBS_TBSIF_ADC3IF);                    /*!< 清除中断标志          */
    }

    if(SET == HT_TBS_ITFlagStatusGet(TBS_TBSIF_ADC4IF))                /*!< ADC4测量中断          */
    {

        HT_TBS_ClearITPendingBit(TBS_TBSIF_ADC4IF);                    /*!< 清除中断标志          */
    }

    if(SET == HT_TBS_ITFlagStatusGet(TBS_TBSIF_ADC5IF))                /*!< ADC5测量中断          */
    {

        HT_TBS_ClearITPendingBit(TBS_TBSIF_ADC5IF);                    /*!< 清除中断标志          */
    }
#endif

#if defined  HT6x2x  ||  defined  HT6x3x  ||  defined  HT501x  ||  defined  HT502x
    if(SET == HT_TBS_ITFlagStatusGet(TBS_TBSIF_ADC0CMPIF))             /*!< ADC0CMP测量中断       */
    {

        HT_TBS_ClearITPendingBit(TBS_TBSIF_ADC0CMPIF);                 /*!< 清除中断标志          */
    }
#endif

#if defined  HT501x  ||  defined  HT502x
    if(SET == HT_TBS_ITFlagStatusGet(TBS_TBSIF_ADC1CMPIF))             /*!< ADC1CMP测量中断       */
    {

        HT_TBS_ClearITPendingBit(TBS_TBSIF_ADC1CMPIF);                 /*!< 清除中断标志          */
    }
#endif

}

/*
*********************************************************************************************************
*                                    RTC INTERRUPT HANDLER  #16+20
*
*********************************************************************************************************
*/

void RTC_IRQHandler()
{
    uint8_t Flag = HT_RTC->RTCIF;
    HT_RTC->RTCIF=0;                                               /*!< 清除中断标志          */

    if(Flag & RTC_RTCIF_SECIF)                                     /*!< 秒中断                */
    {

    }

    if(Flag & RTC_RTCIF_MINIF)                                     /*!< 分中断                */
    {

    }

    if(Flag & RTC_RTCIF_HRIF)                                      /*!< 小时中断              */
    {

    }

    if(Flag & RTC_RTCIF_DAYIF)                                     /*!< 日中断                */
    {

    }

    if(Flag & RTC_RTCIF_MTHIF)                                     /*!< 月中断                */
    {

    }

    if(Flag & RTC_RTCIF_RTC1IF)                                    /*!< RTC定时器1中断        */
    {

    }

    if(Flag & RTC_RTCIF_RTC2IF)                                    /*!< RTC定时器2中断        */
    {

    }

    if(Flag & RTC_RTCIF_ALMIF)                                     /*!< 闹钟中断              */
    {

    }
}

/*
*********************************************************************************************************
*                                    I2C INTERRUPT HANDLER  #16+21
*
*********************************************************************************************************
*/

void I2C_IRQHandler()
{

}

/*
*********************************************************************************************************
*                                    SPI0 INTERRUPT HANDLER  #16+22
*
*********************************************************************************************************
*/

void SPI0_IRQHandler()
{

    if(SET == HT_SPI_ITFlagStatusGet(HT_SPI0,SPI_SPISTA_MODF))                 /*!< SPI0模式故障标志位     */
    {

        HT_SPI_ClearITPendingBit(HT_SPI0, SPI_SPISTA_MODF);                    /*!< 清除中断标志          */
    }

    if(SET == HT_SPI_ITFlagStatusGet(HT_SPI0, SPI_SPISTA_SPIF))                /*!< SPI0数据传输完成标志位*/
    {

        HT_SPI_ClearITPendingBit(HT_SPI0, SPI_SPISTA_SPIF);                    /*!< 清除中断标志          */
    }

}

/*
*********************************************************************************************************
*                                  WDT/SPI1 INTERRUPT HANDLER  #16+23
*
*********************************************************************************************************
*/
#if defined  HT6x1x
void WDT_IRQHandler()
{

}
#elif defined HT6x2x  ||  defined  HT6x3x
void SPI1_IRQHandler()
{
    if(SET == HT_SPI_ITFlagStatusGet(HT_SPI1,SPI_SPISTA_MODF))                 /*!< SPI1模式故障标志位     */
    {

        HT_SPI_ClearITPendingBit(HT_SPI1, SPI_SPISTA_MODF);                    /*!< 清除中断标志          */
    }

    if(SET == HT_SPI_ITFlagStatusGet(HT_SPI1, SPI_SPISTA_SPIF))                /*!< SPI1数据传输完成标志位*/
    {

        HT_SPI_ClearITPendingBit(HT_SPI1, SPI_SPISTA_SPIF);                    /*!< 清除中断标志          */
    }
}
#endif

/*
*********************************************************************************************************
*                                 FREQ TEST INTERRUPT HANDLER  #16+24
*
*********************************************************************************************************
*/
#if defined  HT6x2x  ||  defined  HT6x3x
void SelfTestFreq_IRQHandler(void)
{

}
#endif

/*
*********************************************************************************************************
*                               TIMER_4/EMU INTERRUPT HANDLER  #16+25
*
*********************************************************************************************************
*/
#if defined  HT6x2x  ||  defined  HT6x3x
void TIMER_4_IRQHandler(void)
{

    if(SET == HT_TMR_ITFlagStatusGet(HT_TMR4, TMR_TMRIF_PRDIF))                /*!< 周期中断           */
    {

        HT_TMR_ClearITPendingBit(HT_TMR4, TMR_TMRIF_PRDIF);                    /*!< 清除中断标志       */
    }

    if(SET == HT_TMR_ITFlagStatusGet(HT_TMR4, TMR_TMRIF_CAPIF))                /*!< 捕获中断           */
    {

        HT_TMR_ClearITPendingBit(HT_TMR4, TMR_TMRIF_CAPIF);                    /*!< 清除中断标志       */
    }

    if(SET == HT_TMR_ITFlagStatusGet(HT_TMR4, TMR_TMRIF_CMPIF))                /*!< 比较中断           */
    {

        HT_TMR_ClearITPendingBit(HT_TMR4, TMR_TMRIF_CMPIF);                    /*!< 清除中断标志       */
    }

}
#elif defined HT501x
void EMU_IRQHandler(void)
{

}
#endif

/*
*********************************************************************************************************
*                               TIMER_5/DMA INTERRUPT HANDLER  #16+26
*
*********************************************************************************************************
*/

#if defined  HT6x2x  ||  defined  HT6x3x
void TIMER_5_IRQHandler(void)
{

    if(SET == HT_TMR_ITFlagStatusGet(HT_TMR5, TMR_TMRIF_PRDIF))                /*!< 周期中断           */
    {

        HT_TMR_ClearITPendingBit(HT_TMR5, TMR_TMRIF_PRDIF);                    /*!< 清除中断标志       */
    }

    if(SET == HT_TMR_ITFlagStatusGet(HT_TMR5, TMR_TMRIF_CAPIF))                /*!< 捕获中断           */
    {

        HT_TMR_ClearITPendingBit(HT_TMR5, TMR_TMRIF_CAPIF);                    /*!< 清除中断标志       */
    }

    if(SET == HT_TMR_ITFlagStatusGet(HT_TMR5, TMR_TMRIF_CMPIF))                /*!< 比较中断           */
    {

        HT_TMR_ClearITPendingBit(HT_TMR5, TMR_TMRIF_CMPIF);                    /*!< 清除中断标志       */
    }

}
#elif defined HT501x
void DMA_IRQHandler(void)
{

}
#endif

/*
*********************************************************************************************************
*                               UART6/KEY INTERRUPT HANDLER  #16+27
*
*********************************************************************************************************
*/
#if defined  HT6x2x  ||  defined  HT6x3x
void UART6_IRQHandler(void)
{
    if(SET == HT_UART_ITFlagStatusGet(HT_UART6, UART_UARTSTA_TXIF))         /*!< UART0发送中断         */
    {

        HT_UART_ClearITPendingBit(HT_UART6, UART_UARTSTA_TXIF);             /*!< 清除中断标志          */
    }

    if(SET == HT_UART_ITFlagStatusGet(HT_UART6, UART_UARTSTA_RXIF))         /*!< UART0接收中断         */
    {

        HT_UART_ClearITPendingBit(HT_UART6, UART_UARTSTA_RXIF);             /*!< 清除中断标志          */

    }
}
#elif defined HT501x
void KEY_IRQHandler(void)
{

}
#endif

/*
*********************************************************************************************************
*                                 EXTI7 INTERRUPT HANDLER  #16+28
*
*********************************************************************************************************
*/
#if defined  HT6x2x
void EXTI7_IRQHandler(void)
{
    if(SET == HT_EXTIRise_ITFlagStatusGet(INT_EXTIF2_RIF_INT7))         /*!< INT7上升沿中断           */
    {

        HT_EXTIRise_ClearITPendingBit(INT_EXTIF2_RIF_INT7);             /*!< 清除中断标志             */
    }

    if(SET == HT_EXTIFall_ITFlagStatusGet(INT_EXTIF2_FIF_INT7))         /*!< INT7下降沿中断           */
    {

        HT_EXTIFall_ClearITPendingBit(INT_EXTIF2_FIF_INT7);             /*!< 清除中断标志             */
    }
}
#elif  defined  HT6x3x
void EXTI7_IRQHandler(void)
{
    if(SET == HT_EXTIRise_ITFlagStatusGet(INT_EXTIF2_RIF_INT7))         /*!< INT7上升沿中断           */
    {

        HT_EXTIRise_ClearITPendingBit(INT_EXTIF2_RIF_INT7);             /*!< 清除中断标志             */
    }

    if(SET == HT_EXTIFall_ITFlagStatusGet(INT_EXTIF2_FIF_INT7))         /*!< INT7下降沿中断           */
    {

        HT_EXTIFall_ClearITPendingBit(INT_EXTIF2_FIF_INT7);             /*!< 清除中断标志             */
    }


    if(SET == HT_EXTIRise_ITFlagStatusGet(INT_EXTIF2_RIF_INT8))         /*!< INT8上升沿中断           */
    {

        HT_EXTIRise_ClearITPendingBit(INT_EXTIF2_RIF_INT8);             /*!< 清除中断标志             */
    }

    if(SET == HT_EXTIFall_ITFlagStatusGet(INT_EXTIF2_FIF_INT8))         /*!< INT8下降沿中断           */
    {

        HT_EXTIFall_ClearITPendingBit(INT_EXTIF2_FIF_INT8);             /*!< 清除中断标志             */
    }


    if(SET == HT_EXTIRise_ITFlagStatusGet(INT_EXTIF2_RIF_INT9))         /*!< INT9上升沿中断           */
    {

        HT_EXTIRise_ClearITPendingBit(INT_EXTIF2_RIF_INT9);             /*!< 清除中断标志             */
    }

    if(SET == HT_EXTIFall_ITFlagStatusGet(INT_EXTIF2_FIF_INT9))         /*!< INT9下降沿中断           */
    {

        HT_EXTIFall_ClearITPendingBit(INT_EXTIF2_FIF_INT9);             /*!< 清除中断标志             */
    }
}
#endif
/*
*********************************************************************************************************
*                                 EXTI8 INTERRUPT HANDLER  #16+29
*
*********************************************************************************************************
*/
#if defined  HT6x2x
void EXTI8_IRQHandler(void)
{
    if(SET == HT_EXTIRise_ITFlagStatusGet(INT_EXTIF2_RIF_INT8))         /*!< INT8上升沿中断           */
    {

        HT_EXTIRise_ClearITPendingBit(INT_EXTIF2_RIF_INT8);             /*!< 清除中断标志             */
    }

    if(SET == HT_EXTIFall_ITFlagStatusGet(INT_EXTIF2_FIF_INT8))         /*!< INT8下降沿中断           */
    {

        HT_EXTIFall_ClearITPendingBit(INT_EXTIF2_FIF_INT8);             /*!< 清除中断标志             */
    }
}
#endif

/*
*********************************************************************************************************
*                                 EXTI9/SPI INTERRUPT HANDLER  #16+30
*
*********************************************************************************************************
*/
#if defined  HT6x2x
void EXTI9_IRQHandler(void)
{
    if(SET == HT_EXTIRise_ITFlagStatusGet(INT_EXTIF2_RIF_INT9))         /*!< INT9上升沿中断           */
    {

        HT_EXTIRise_ClearITPendingBit(INT_EXTIF2_RIF_INT9);             /*!< 清除中断标志             */
    }

    if(SET == HT_EXTIFall_ITFlagStatusGet(INT_EXTIF2_FIF_INT9))         /*!< INT9下降沿中断           */
    {

        HT_EXTIFall_ClearITPendingBit(INT_EXTIF2_FIF_INT9);             /*!< 清除中断标志             */
    }
}
#elif  defined  HT6x3x
void SPI2_IRQHandler()
{
    if(SET == HT_SPI_ITFlagStatusGet(HT_SPI2,SPI_SPISTA_MODF))          /*!< SPI1模式故障标志位     */
    {

        HT_SPI_ClearITPendingBit(HT_SPI2, SPI_SPISTA_MODF);             /*!< 清除中断标志          */
    }

    if(SET == HT_SPI_ITFlagStatusGet(HT_SPI2, SPI_SPISTA_SPIF))         /*!< SPI1数据传输完成标志位*/
    {

        HT_SPI_ClearITPendingBit(HT_SPI2, SPI_SPISTA_SPIF);             /*!< 清除中断标志          */
    }
}
#endif
/*
*********************************************************************************************************
*                                 DMA INTERRUPT HANDLER  #16+31
*
*********************************************************************************************************
*/
#if defined  HT6x2x  ||  defined  HT6x3x
void DMA_IRQHandler(void)
{
    HT_DMA_ClearITPendingBit(DMA_DMAIF_TCIF1);
    DMA_Cmd(HT_DMA_Channel1, DISABLE);
    DMA_Cmd(HT_DMA_Channel0, DISABLE);
}
#endif




