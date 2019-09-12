//************************************************************
//  Data        :   2019/04/09
//  Programmer  :   Garfield.Lin
//  Function    :
//  MCU         :   MSP430G2553IPW28
//  Oscillator  :   Internal DOC 12MHZ
//  Version     :   V0.1
//
/*
 *
 *     V1.1  20190521    AID   修改;
 *
 *
 *     V1.4   20190530 fenglong
 *      1. 客户要求通信能关机
 *      2. 客户要求加入SOC， 未经过验证
 *      3. 充电判饱截至电流修改为150mA
 *      4. 增加了低温起充和高温起充功能，
 *      5. 增加了高温起放功能
 *      6. PF_OCC调整为12A
 *      7. 增加通信开机指示灯不亮，按键开机再亮。
 *
 *
 *      V1.5    20190611 fenglong
 *
 *       1. 增加放电起始温度保护，设定61度
 *       2. 修改了高温MOSFET保护BUG
 *       3. 2.8V保护后，上电激活禁止输出，直到充电到3.4V
 *       4. 增加了电芯高温低温报警，MOSFET高温报警功能
 *
 *       5. 所有功能测试OK, 准备封存
 *
 *
 *      V1.6    20190615  fenglong
 *
 *       1.增加充电均衡， 增加lifetimes ,CELLS max/min voltage record
 *       2. 增加生产日期写入读取。
 *       3. 增加制造商读取。
 *       2.修改发送实时参数结构体及发送方式，以节省RAM
 *       3. 按键100ms延时修改位20ms
 *       4. 关闭4.17V 过压警告功能。29.
 *       5. 增加了电量显示功能。
 *       6. 修改充电后不能查询电量BUG
 *       7. 修改查询电量不能呼吸BUG
 *
 *      V1.7    20190701 fenglong
 *       1. 加入SLEEP,实测功耗小于150uA
 *
 *      V1.8    20190702 fenglong
 *       1. 加入报警灯效
 *       2. 修正按键关机后进入休眠按键失效BUG
 *       3. 修改开机等效
 *       4.
 *
 *
 *待改：  1  高温起充
 *待改： 2. 灯效更改
 *待改： 3 .通信清PF
 *
 *      V1.81    20190708 fenglong
 *       1. 修正充电无法激活BUG
 *       2. 高温起充按客户要求修改为不红灯警报
 *       3. 增加通信清除PF
 *       4. 修改了写入生产日期指令码
 *
 *      V1.82    20190708 fenglong
 *       1. 修正充电一级过压无法显示满电BUG
 *		 2. 修正过温起放无法报警			20190709
 *
 *      V1.83    20190709 fenglong
 *       1. 取消虚拟电量补偿功能
 *		 2. 删除部分没有用的查询电量代码
 *
 *
 *       V1.84    20190709 fenglong 20190710
 *       1. 更改开机流水灯效
 *		 2. 待改 写日期无法改 DD
 *
 *       V1.86    20190713 fenglong 20190713
 *       1. 修正开机流水灯效BUG
 *		 2. 待改 写日期无法改 DD
 *		 3. 改电压查表,增加10秒延时查表
 *
 *        V1.87    20190714 fenglong 20190714
 *       1. 应客户要求现场更改开机流水灯效 *
 *		 2. 待改 写日期无法改 DD
 *
 *         V1.88    20190714 fenglong 20190714
 *       1. 应客户要求现场更改SOC查表时间
 *       2. 加入软件一级低压进SHIPPING
 *		 2. 待改 写日期无法改 DD
 *
 *        V1.89 軟件瘦身,軟件修改PT欠壓保護不能恢復BUG
 *
 *
 *
 * 		  V2.00    20190801
 *		 1. 硬件对调LED3,LED4引脚，相应的软件变更
 *		 2. 加入充电循环次数，定为累计冲90%为一个循环
 *		 3. 将PF报警等效改为最高优先级
 *		 4. 修改了放电高温保护后立即充电灯不亮BUG
 *		 5. 应客户要求修改短路保护电流为80A
 *		 6. 应客户要求修改一级过流为25A
 *
 *       V2.02    20190903
 *       1. 加入了低温起充和低温充电判断，防止适配器在位检测后，本该闪动的灯效关闭（告警灯
 *       2.适配移除判断后，加入充电温度相关标志位的清除，防止在放电过程中执行一些不必要的充电保护相关的判断。
 *       3.加入RT3温度采集并纳入充放电高低温保护
 *
 *
 *
 */
//****************************************************************


//#include <msp430.h>
#include "System.h"
//#include "source/System.h"
#include"RS_485.h"
#include"uart.h"
#include"soft_soc.h"
#include"ADC_temp.h"
//#include"string.h"

extern _BatteryData BatteryData;
extern UINT8 T0_INT_count;
uchar BQ_GetData_Delay =0;
uchar Powrer_On_Delay =0;
extern UINT16 CellMiniVoltage;
extern UINT16 CellMAXiVoltage;
extern UINT8 SystemMode;
extern UINT16 RT3_AD;
extern  UINT16 Sleep_delay;
extern union FW_PF FW_PF_Status;        //  三级保护？
extern union FW_PT FW_PT_Status;               //CPU           一级保护
extern unsigned char  power_first_flag;
UINT16 his_data[FLASH_DATA_NUM];
extern UINT16 BatteryCapacityRefreshCount;
#define FLASH_CHECK    0x0910


void main(void)
{
	uchar i;
	unsigned short crc16;
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	_nop ();
    SystemInitial ();
    InitAdc();

#ifndef UART_PRINTF

    Uart_State_Init();

#endif

    BatteryRealData_Refresh();
    //get_RT3_temp();

    CellMiniVoltage = 3600;
    CellMAXiVoltage = 3600;

    __bic_SR_register(  GIE);       // Enter LPM3 w/ interrupt
    for(i =0;i<FLASH_DATA_NUM;i++)
    {
    	his_data[i] = Flash_Read_int((unsigned int *)Segment_C,i);
    }
    crc16 =0;
    for(i=0;i<FLASH_DATA_NUM-1;i++)
    {
        crc16 += his_data[i];
    }
    if((crc16 == his_data[HIS_CRC16])&&(his_data[0] == FLASH_CHECK))
    {
        FW_PF_Status.Word = his_data[PF_STATE_ADDR];
//        if(his_data[HIS_DSG_UVP_ADD])
        if( (his_data[HIS_DSG_UVP_ADD] & 0x01) != 0 )           //20190912 fenglong 为追查无故进SHIPPING增加的代码
        {
            FW_PT_Status.Bits.PT_UV = 1; // 临时屏蔽
        }
        sys_cap.val.bat_cycle_cnt = his_data[FLASH_CYCLES_ADDR];              //fenglong 20190615
        sys_cap.val.chg_cap_cnt = (uchar)(his_data[CHG_CAP_CNT]) ;
    }
	else
	{
		for(i =0;i<FLASH_DATA_NUM;i++)
		{
			his_data[i] = Flash_Read_int((unsigned int *)Segment_D,i);
		}
		crc16 =0;
        for(i=0;i<FLASH_DATA_NUM-1;i++)
        {
            crc16 += his_data[i];
        }
		if((his_data[0] == FLASH_CHECK)&&(crc16 == his_data[HIS_CRC16]))
		{
			FW_PF_Status.Word = his_data[PF_STATE_ADDR];
    //        if(his_data[HIS_DSG_UVP_ADD])
            if( (his_data[HIS_DSG_UVP_ADD] & 0x01) != 0 )               //20190912 fenglong 为追查无故进SHIPPING增加的代码
            {
                FW_PT_Status.Bits.PT_UV = 1;// 临时屏蔽
            }
			sys_cap.val.bat_cycle_cnt=his_data[FLASH_CYCLES_ADDR];                      //fenglong 20190615
			sys_cap.val.chg_cap_cnt = (uchar)(his_data[CHG_CAP_CNT]) ;
			Falsh_Write_Arr((unsigned int *)Segment_C,his_data,FLASH_DATA_NUM);         //fenglong 20190615
			//
		}
		else
		{
		    crc16 =0;
			his_data[0] = FLASH_CHECK;
			crc16 += his_data[0];
			for(i=1;i<FLASH_DATA_NUM-1;i++)
			{
				his_data[i] =0;
			}
			his_data[HIS_CRC16] = crc16;
			Falsh_Write_Arr((unsigned int *)Segment_C,his_data,FLASH_DATA_NUM);         //fenglong 20190615
			Falsh_Write_Arr((unsigned int *)Segment_D,his_data,FLASH_DATA_NUM);         //fenglong 20190615
			FW_PF_Status.Word =0;
			FW_PT_Status.Bits.PT_UV =0;
			sys_cap.val.bat_cycle_cnt=0;                                                //fenglong 20190615
			sys_cap.val.chg_cap_cnt = 0;

			his_data_init();

//			his_data[HIS_Cell1MiniVoltage]=4200;                                        //fenglong 20190617
//			his_data[HIS_Cell2MiniVoltage]=4200;                                        //fenglong 20190617
//			his_data[HIS_Cell3MiniVoltage]=4200;                                        //fenglong 20190617
//			his_data[HIS_Cell4MiniVoltage]=4200;                                        //fenglong 20190617
//			his_data[HIS_Cell5MiniVoltage]=4200;                                        //fenglong 20190617
//			his_data[HIS_Cell6MiniVoltage]=4200;                                        //fenglong 20190617
//			his_data[HIS_Cell7MiniVoltage]=4200;                                        //fenglong 20190617
//
//			his_data[HIS_Cell1MAXiVoltage]=0;                                        //fenglong 20190730
//			his_data[HIS_Cell2MAXiVoltage]=0;                                        //fenglong 20190730
//			his_data[HIS_Cell3MAXiVoltage]=0;                                        //fenglong 20190730
//			his_data[HIS_Cell4MAXiVoltage]=0;                                        //fenglong 20190730
//			his_data[HIS_Cell5MAXiVoltage]=0;                                        //fenglong 20190730
//			his_data[HIS_Cell6MAXiVoltage]=0;                                        //fenglong 20190730
//			his_data[HIS_Cell7MAXiVoltage]=0;                                        //fenglong 20190730
//
//			his_data[HIS_MOSFET_MAXTEMP]=4000;
		}
	}

	__bis_SR_register(  GIE);       // Enter LPM3 w/ interrupt
	SystemMode = SystemSleepMode;
	if(sys_cap.val.bat_cycle_cnt < 1000)
	{
	    sys_cap.val.full_cap = 2500-sys_cap.val.bat_cycle_cnt;
	}

	BatteryData.TS2.TS2Word = 4000;
	BatteryData.TS1.TS1Word = 4000;


//	if( his_data[HIS_Cell1MiniVoltage] ==0 )
//	{
//
//	}
//	his_data[HIS_Cell1MiniVoltage]=4200;                                        //fenglong 20190726
//	his_data[HIS_Cell2MiniVoltage]=4200;                                        //fenglong 20190726
//	his_data[HIS_Cell3MiniVoltage]=4200;                                        //fenglong 20190726
//	his_data[HIS_Cell4MiniVoltage]=4200;                                        //fenglong 20190726
//	his_data[HIS_Cell5MiniVoltage]=4200;                                        //fenglong 20190726
//	his_data[HIS_Cell6MiniVoltage]=4200;                                        //fenglong 20190726
//	his_data[HIS_Cell7MiniVoltage]=4200;                                        //fenglong 20190726
//	his_data[HIS_Cell7MiniVoltage]=4200;                                        //fenglong 20190726


//	sys_cap.val.re_cap_rate =100;						//fenglong 20190713
//	sys_cap.val.bat_cap = sys_cap.val.full_cap;			//fenglong 20190713

	power_first_flag=0;			//fenglong 20190713
	BatteryCapacityRefreshCount = TA0_10s-TA0_2s ;		//fenglong 20190713
//	sys_cap.val.bat_cap = 1250;
//	sys_cap.val.re_cap_rate = 50;

	while(1)
	{
	    WDTCTL = WDT_ARST_1000;             //fenglong 20190608

	    ReadKey();          //detect key

	    _nop ();

	    if(Powrer_On_Delay >30)
	    {
	        if(T0_INT_count>0)  //20MS
	        {
                //PT_ProtectRenew();

                SystemStatusDetect();

                ProtectRenew();//保护信息更新
                AlarmDetect();
                ImbalanceCheck();
                MosfetDetect();

                MosStatusDetect ();//MOSFET工作切换
                balance();
                NormalCapacityProc();		//fenglong 20190729
                get_RT3_temp();             //wk 190902
                T0_INT_count =0;
                if (SystemMode != SystemBootUp)
                {
                    if(BQ_GetData_Delay %5 == 0)
                    {
                        Update_BQ_Data();
                        His_DataRenew();
                    }
                    BQ_GetData_Delay++;
                    if(BQ_GetData_Delay > 200)
                    {
                        BQ_GetData_Delay =0;
                        LoadFlashData (); //init BQ Data
                    }
                }
	        }

	    }
	    else
	    {
	    	Powrer_On_Delay++;
	    }
	    if(Powrer_On_Delay == 1)
	    {
	        LoadFlashData ();   //init BQ Data
	    }

        BatteryRealData_Refresh();

        RS485_REC_Task();

        SleepCheck();

        SleepMode();

	}
}




