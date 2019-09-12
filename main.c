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
 *     V1.1  20190521    AID   �޸�;
 *
 *
 *     V1.4   20190530 fenglong
 *      1. �ͻ�Ҫ��ͨ���ܹػ�
 *      2. �ͻ�Ҫ�����SOC�� δ������֤
 *      3. ����б����������޸�Ϊ150mA
 *      4. �����˵������͸�����书�ܣ�
 *      5. �����˸�����Ź���
 *      6. PF_OCC����Ϊ12A
 *      7. ����ͨ�ſ���ָʾ�Ʋ�������������������
 *
 *
 *      V1.5    20190611 fenglong
 *
 *       1. ���ӷŵ���ʼ�¶ȱ������趨61��
 *       2. �޸��˸���MOSFET����BUG
 *       3. 2.8V�������ϵ缤���ֹ�����ֱ����絽3.4V
 *       4. �����˵�о���µ��±�����MOSFET���±�������
 *
 *       5. ���й��ܲ���OK, ׼�����
 *
 *
 *      V1.6    20190615  fenglong
 *
 *       1.���ӳ����⣬ ����lifetimes ,CELLS max/min voltage record
 *       2. ������������д���ȡ��
 *       3. ���������̶�ȡ��
 *       2.�޸ķ���ʵʱ�����ṹ�弰���ͷ�ʽ���Խ�ʡRAM
 *       3. ����100ms��ʱ�޸�λ20ms
 *       4. �ر�4.17V ��ѹ���湦�ܡ�29.
 *       5. �����˵�����ʾ���ܡ�
 *       6. �޸ĳ����ܲ�ѯ����BUG
 *       7. �޸Ĳ�ѯ�������ܺ���BUG
 *
 *      V1.7    20190701 fenglong
 *       1. ����SLEEP,ʵ�⹦��С��150uA
 *
 *      V1.8    20190702 fenglong
 *       1. ���뱨����Ч
 *       2. ���������ػ���������߰���ʧЧBUG
 *       3. �޸Ŀ�����Ч
 *       4.
 *
 *
 *���ģ�  1  �������
 *���ģ� 2. ��Ч����
 *���ģ� 3 .ͨ����PF
 *
 *      V1.81    20190708 fenglong
 *       1. ��������޷�����BUG
 *       2. ������䰴�ͻ�Ҫ���޸�Ϊ����ƾ���
 *       3. ����ͨ�����PF
 *       4. �޸���д����������ָ����
 *
 *      V1.82    20190708 fenglong
 *       1. �������һ����ѹ�޷���ʾ����BUG
 *		 2. ������������޷�����			20190709
 *
 *      V1.83    20190709 fenglong
 *       1. ȡ�����������������
 *		 2. ɾ������û���õĲ�ѯ��������
 *
 *
 *       V1.84    20190709 fenglong 20190710
 *       1. ���Ŀ�����ˮ��Ч
 *		 2. ���� д�����޷��� DD
 *
 *       V1.86    20190713 fenglong 20190713
 *       1. ����������ˮ��ЧBUG
 *		 2. ���� д�����޷��� DD
 *		 3. �ĵ�ѹ���,����10����ʱ���
 *
 *        V1.87    20190714 fenglong 20190714
 *       1. Ӧ�ͻ�Ҫ���ֳ����Ŀ�����ˮ��Ч *
 *		 2. ���� д�����޷��� DD
 *
 *         V1.88    20190714 fenglong 20190714
 *       1. Ӧ�ͻ�Ҫ���ֳ�����SOC���ʱ��
 *       2. �������һ����ѹ��SHIPPING
 *		 2. ���� д�����޷��� DD
 *
 *        V1.89 ܛ������,ܛ���޸�PTǷ�����o���ܻ֏�BUG
 *
 *
 *
 * 		  V2.00    20190801
 *		 1. Ӳ���Ե�LED3,LED4���ţ���Ӧ��������
 *		 2. ������ѭ����������Ϊ�ۼƳ�90%Ϊһ��ѭ��
 *		 3. ��PF������Ч��Ϊ������ȼ�
 *		 4. �޸��˷ŵ���±������������Ʋ���BUG
 *		 5. Ӧ�ͻ�Ҫ���޸Ķ�·��������Ϊ80A
 *		 6. Ӧ�ͻ�Ҫ���޸�һ������Ϊ25A
 *
 *       V2.02    20190903
 *       1. �����˵������͵��³���жϣ���ֹ��������λ���󣬱��������ĵ�Ч�رգ��澯��
 *       2.�����Ƴ��жϺ󣬼������¶���ر�־λ���������ֹ�ڷŵ������ִ��һЩ����Ҫ�ĳ�籣����ص��жϡ�
 *       3.����RT3�¶Ȳɼ��������ŵ�ߵ��±���
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
extern union FW_PF FW_PF_Status;        //  ����������
extern union FW_PT FW_PT_Status;               //CPU           һ������
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
        if( (his_data[HIS_DSG_UVP_ADD] & 0x01) != 0 )           //20190912 fenglong Ϊ׷���޹ʽ�SHIPPING���ӵĴ���
        {
            FW_PT_Status.Bits.PT_UV = 1; // ��ʱ����
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
            if( (his_data[HIS_DSG_UVP_ADD] & 0x01) != 0 )               //20190912 fenglong Ϊ׷���޹ʽ�SHIPPING���ӵĴ���
            {
                FW_PT_Status.Bits.PT_UV = 1;// ��ʱ����
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

                ProtectRenew();//������Ϣ����
                AlarmDetect();
                ImbalanceCheck();
                MosfetDetect();

                MosStatusDetect ();//MOSFET�����л�
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




