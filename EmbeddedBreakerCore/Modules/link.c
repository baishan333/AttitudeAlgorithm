#include "stdafx.h"
//code by </MATRIX>@Neod Anderjon
//author: Neod Anderjon
//====================================================================================================
/*
	模块对框架EmbeddBreakerCore的链接
	该文件写入对框架的函数调用支持
*/

//选项设置，链接到Universal_Resource_Config函数的模块库
void Modules_UniResConfig (void)
{
	//该函数设置内容可以更新Universal_Resource_Config函数原设置
}

//模块选项映射表，链接到urcMapTable_Print函数
void Modules_URCMap (void)
{
	
}

//选项处理，链接到pclURC_DebugHandler函数
void Modules_urcDebugHandler (u8 ed_status, Modules_SwitchNbr sw_type)
{
   //使用前请先更新Modules_SwitchNbr内容
}

//协议调用指令响应，链接到OrderResponse_Handler函数
void Modules_ProtocolTask (void)
{
	
}

//OLED常量显示屏，链接到OLED_DisplayInitConst和UIScreen_DisplayHandler函数
void OLED_ScreenModules_Const (void)
{
	OLED_ShowString(strPos(1u), ROW1, (const u8*)"   Attitude   ", Font_Size);	
	OLED_ShowString(strPos(1u), ROW2, (const u8*)"   Algorithm  ", Font_Size);	
	OLED_Refresh_Gram();
}

//OLED模块调用数据显示，链接到UIScreen_DisplayHandler函数
void OLED_DisplayModules (void)
{
	OLED_DisplayAA(&eas);
}

//硬件底层初始化任务，链接到bspPeriSysCalls函数
void Modules_HardwareInit (void)
{
	GyroscopeTotalComponentInit();
	TIM3_IMURealTimeWork(ENABLE);
}

//硬件底层外部中断初始化，链接到EXTI_Config_Init函数
void Modules_ExternInterruptInit (void)
{
	/*
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);			
	MPU6050_INT_IO_Init();
	ucEXTI_ModeConfig(	GPIO_PortSourceGPIOB, 
						GPIO_PinSource12, 
						MPU_INT_EXTI_Line, 
						EXTI_Mode_Interrupt, 
#if MPU_DataTransferFinishedINTLevel == Bit_RESET
						EXTI_Trigger_Falling, 						
#elif MPU_DataTransferFinishedINTLevel == Bit_SET
						EXTI_Trigger_Rising,
#endif
						EXTI15_10_IRQn, 
						0x03, 
						0x03);
*/
}

//外部中断任务，无需声明，使用时修改函数名
/*
void EXTI15_10_IRQHandler (void)
{
#if SYSTEM_SUPPORT_OS 												
	OSIntEnter();    
#endif	
	*/

	/*	Here read one signal level not a key, handler method is different than last.
	 *	If you confirm test in external interrupt update dmp, please enable IMUINT_Enable.
	 *	Setting external interrupt may lead to program dump out, notice it.	
	**/
	/*
	dmpAttitudeAlgorithm_RT(IMUINT_Enable);
	EXTI_ClearITPendingBit(MPU_INT_EXTI_Line);  					//清除EXTI线路挂起位
	
#if SYSTEM_SUPPORT_OS 												
	OSIntExit();  											 
#endif
}
*/

//模块非中断任务，链接到local_taskmgr.c，默认添加到第二任务
void Modules_NonInterruptTask (void)
{
	
}

//模块中断任务，链接到time_base.c TIM2_IRQHandler函数中
void Modules_InterruptTask (void)
{
	
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon