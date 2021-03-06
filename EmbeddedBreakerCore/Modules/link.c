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
	/*
		对框架而言，不显示模块的OLED部分
		对应用的模块而言，不显示框架的常量字符
		且需要使自己本身的显示生效
		框架设置为失能，模块设置为使能
	*/
	MOE_Switch			= MOE_Enable;					//MOE_Enable		MOE_Disable
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
	snprintf((char*)oled_dtbuf, OneRowMaxWord, (" DMP Attitude  "));
	OLED_ShowString(strPos(0u), ROW1, (StringCache*)oled_dtbuf, Font_Size);
	snprintf((char*)oled_dtbuf, OneRowMaxWord, (" Algorithm Demo"));
	OLED_ShowString(strPos(0u), ROW2, (StringCache*)oled_dtbuf, Font_Size);
	OLED_Refresh_Gram();
}

//OLED模块调用数据显示，链接到UIScreen_DisplayHandler函数
void OLED_DisplayModules (u8 page)
{
	switch (page)
	{
	case 5:
		/*
			欧拉角显示需要快速，直接将OLED更新放到中断中完成
			但包含延时函数的东西放到中断中会有各种神奇的现象
			你需要在代码执行效率和稳定性之间做权衡
		*/
		if (UIRef_ModeFlag == Stable_Ref)
			OLED_DisplayAA(&eas);				
		else
			return;
		break;
	}
}

//硬件底层初始化任务，链接到bspPeriSysCalls函数
void Modules_HardwareInit (void)
{
	GyroscopeTotalComponentInit();
#ifdef Use_TimerTrigger_DMP
	TIM3_IMURealTimeWork(ENABLE);
#endif
}

//硬件底层外部中断初始化，链接到EXTI_Config_Init函数
void Modules_ExternInterruptInit (void)
{
#ifndef Use_TimerTrigger_DMP
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);			
	MPU6050_INT_IO_Init();							//初始化INT引脚
	
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
						0x04, 
						0x01);
#endif
}

//外部中断任务，无需声明，使用时修改函数名
#ifndef Use_TimerTrigger_DMP
void EXTI15_10_IRQHandler (void)
{
#if SYSTEM_SUPPORT_OS 												
	OSIntEnter();    
#endif	

	/*	Here read one signal level not a key, handler method is different than last.
	 *	If you confirm test in external interrupt update dmp, please enable IMUINT_Enable.
	 *	Setting external interrupt may lead to program dump out, notice it.	
	**/
	
	dmpAttitudeAlgorithm_RT(IMUINT_Enable);							//读取MPU INT引脚信号
	EXTI_ClearITPendingBit(MPU_INT_EXTI_Line);  					//清除EXTI线路挂起位
	
#if SYSTEM_SUPPORT_OS 												
	OSIntExit();  											 
#endif
}
#endif

//模块非中断任务，链接到local_taskmgr.c，默认添加到第二任务
void Modules_NonInterruptTask (void)
{
	//MPU原始数据
	MPU6050_GetGyroAccelOriginData(&gas);
}

//模块中断任务，链接到time_base.c TIM2_IRQHandler函数中
void Modules_InterruptTask (void)
{
	
}

//基于RTC时间的任务计划，链接到local_taskmgr.c，默认添加到第四任务
void Modules_RTC_TaskScheduler (void)
{
	/*
		RTC API:
			*(rtcTotalData + 0): 年份
			*(rtcTotalData + 1): 月份
			*(rtcTotalData + 2): 日
			*(rtcTotalData + 3): 星期
			*(rtcTotalData + 4): 时
			*(rtcTotalData + 5): 分
			*(rtcTotalData + 6): 秒
	*/
	//example: 设置含有灯光效果的外设休眠
	if ((*(rtcTotalData + 4) >= 1 
		&& *(rtcTotalData + 4) <= 6) || *(rtcTotalData + 4) == 0)
	{
		OLED_Clear();
		OLED_Switch = OLED_Disable;
		Light_Switch = Light_Disable;
	}
	else
	{
		OLED_Switch = OLED_Enable;
		Light_Switch = Light_Enable;
	}
}

//模块状态内容打印请求，链接到sta_req.c displaySystemInfo函数中
void Modules_StatusReqHandler (void)
{
	//此项设计可以减少模块指令的多余添加
	char* aa_dtbuf;
	
#define AAPrintCacheSpace	250u
	//串口采集数据
	U1SD("Gather Attitude Algorithm Result:\r\n");
	aa_dtbuf = (char*)mymalloc(sizeof(char) * AAPrintCacheSpace);
	//欧拉角读取
	snprintf(aa_dtbuf, AAPrintCacheSpace, 
		("Euler Angle USART Outputs ->\r\n\
		[\r\n\
			Pitch(x): 	%8.4f Degree\r\n\
			Roll(y): 	%8.4f Degree\r\n\
			Yaw(z): 	%8.4f Degree\r\n\
		]\r\n"), 
		eas.pitch, eas.roll, eas.yaw);
	U1SD("%s", aa_dtbuf);	
	snprintf(aa_dtbuf, AAPrintCacheSpace, 
		("MPU Original Data ->\r\n\
		[\r\n\
			Gyro_X: 	%8d\r\n\
			Gyro_Y: 	%8d\r\n\
			Gyro_Z: 	%8d\r\n\
			Accel_X: 	%8d\r\n\
			Accel_Y: 	%8d\r\n\
			Accel_Z: 	%8d\r\n\
			Temp:		%6.2f Celsius\r\n\
		]\r\n"), 
		gas.gx, gas.gy, gas.gz, gas.ax, gas.ay, gas.az, MPU_GlobalTemp);
	U1SD("%s", aa_dtbuf);	
	myfree(aa_dtbuf);
}

//模块插入到exti.c的PB8外部中断函数EXTI9_5_IRQHandler内，触发外部中断打断
void Modules_EXTI8_IRQHandler (void)
{
	//通常来说可以在工控系统内紧急停止电机运转
}

//====================================================================================================
//code by </MATRIX>@Neod Anderjon
