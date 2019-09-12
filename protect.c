/*
 * protect.c
 *
 *  Created on: 2019年5月9日
 *      Author: 20190211012
 */

#include "protect.h"

#include "system.h"


//extern UINT8 TEST_3;
extern union C1 Battery;
//extern UINT16 BatteryModeChangeDelayCount;
extern UINT16 SW_PressCount;
extern UINT16 SW_ReleaseCount;
extern UINT16 BatteryCapacityRefreshCount;
extern UINT16 DC_IN_Count;
extern UINT16 DC_IN_Release_Count;
extern UINT16 SleepCount;
extern UINT16 SysSHIPCount;
extern UINT16 PT_UV_Count;
extern UINT16 Cell_PF_UV_Count;
extern UINT16 PF_OCC_Count;
extern UINT16 Cell_PF_OV_Count;
extern UINT16 RT3_AD;

extern UINT8 BatteryCapacity;
extern UINT16 CellMiniVoltage;
extern UINT16 CellMAXiVoltage;

extern UINT16 PF_OTC_Count;

extern UINT16 PF_OTM_Count;                                         //permanent fail safety over  temperature MOSFET
extern UINT16 PT_UV_Count;
extern UINT16 PT_OV_Count;

extern UINT16 PF_OCD_Count;

extern UINT16 PT_OCP_Count;                                         //over charge current protection
extern UINT16 PT_ODP_Count;                                         //over discharge current protection
extern UINT16 PT_OTCP_Count;                                        //over temperature charge protection
extern UINT16 PT_OTDP_Count;                                        //over temperature discharge protection
extern UINT16 PT_UTCP_Count;                                        //under temperature charge protection
extern UINT16 PT_UTDP_Count;
extern UINT16 AL_OTDA_Count;
extern UINT16 AL_UTDA_Count;
extern UINT16 AL_OTMA_Count;
extern UINT16 PT_ODP_AutoReleaseCount;
extern UINT16 HW_PT_OCD_SCD_AutoReleaseCount;

//extern RegisterGroup   BQ76930Data;   fenglong 20190621
extern _BatteryData    BatteryData;
//extern _FlashData      BQ76930ProtectData;
extern union C1 Battery;
//extern union _DataSource     BQ76930ProtectDataSource;
extern union FW_PF FW_PF_Status;
extern union HW_PT HW_PT_Status;
extern union FW_PT FW_PT_Status;

extern UINT8 SystemMode;

extern UINT8 T0_INT_count;

UINT16  Imbalance_Count=0;


void ProtectDetect(void)
{
    UINT16 cur_tmp;





//----------------------------------------------------

//    if (BatteryModeChangeDelayCount <= TA0_1m)
//        BatteryModeChangeDelayCount++;

    if (BatteryCapacityRefreshCount <= TA0_20m)
    {
    	BatteryCapacityRefreshCount++;
    }

//fenglong 20190726
    if( SystemMode == SystemChargeMode )
    {
    	if(   (FW_PT_Status.Bits.PT_OTSCP == 1)
    		||(FW_PT_Status.Bits.PT_UTSCP == 1)
    	  )
    	{
    		DC_IN_Count = 0;
    	}
    }

    if (DC_IN)                 //DC in  ,charge
    {
        SysSHIPCount =0;
        if (DC_IN_Count < TA0_5s)      //cancel =   fenglong 20190607
        {
            DC_IN_Count++;
            if (DC_IN_Count < TA0_3s)
            {
                Temp_CHG_Start_Check();
            }
        }
        DC_IN_Release_Count = 0;
    }
    else                                //DC pull out
    {
        FW_PT_Status.Bits.PT_UTSCP=0;
        FW_PT_Status.Bits.PT_OTSCP=0;
        DC_IN_Count = 0;
        if (DC_IN_Release_Count <= TA0_5s)
        {
            DC_IN_Release_Count = DC_IN_Release_Count + 1;
        }

    }

    if(SleepCount < TA0_21m)
    {
        SleepCount++;
    }


//    if(SysRunCount<TA0_1m)
//        SysRunCount++;




    if (PT_UV_Count <= TA0_10s)
    {
        if (PT_UV_Default >= CellMiniVoltage)
            PT_UV_Count = PT_UV_Count + 1;
        else
            PT_UV_Count = 0;
    }
    if (PT_OV_Count <= TA0_10s)
    {
        if (PT_OV_Default < CellMAXiVoltage)
            PT_OV_Count = PT_OV_Count + 1;
        else
            PT_OV_Count = 0;
    }






    if (Cell_PF_UV_Count <= TA0_10s)
    {
        if (PF_UV_Default > CellMiniVoltage)
            Cell_PF_UV_Count = Cell_PF_UV_Count + 1;
        else
            Cell_PF_UV_Count = 0;           //permanent fail safety under voltage
    }

    if (Cell_PF_OV_Count <= TA0_10s)
    {
        if (CellMAXiVoltage > PF_OV_Default)
            Cell_PF_OV_Count = Cell_PF_OV_Count + 1;
        else
            Cell_PF_OV_Count = 0;           //4.23mV    -   8s      //permanent fail safety over voltage
    }

    if (PF_OTC_Count <= TA0_4s)
    {
        if (BatteryData.TS1.TS1Word < PF_OTC_Default)// || RT3_AD > PF_OTC_Def_RT3)   //190902 wk(增加RT3)
            PF_OTC_Count = PF_OTC_Count + 1;
        else
            PF_OTC_Count = 0;
    }

    if (PF_OTM_Count <= TA0_8s)
    {
        if (BatteryData.TS2.TS2Word < PF_OTM_Default)
            PF_OTM_Count = PF_OTM_Count + 1;
        else
            PF_OTM_Count = 0;                   //permanent fail safety over  temperature MOSFET
    }
    //過流保护
    if (BatteryData.CC.CCWord >= 0x8000)        //库仑计   coulomb counter
    {
    	PF_OCC_Count = 0;
    	PT_OCP_Count = 0;
    	cur_tmp = 0-BatteryData.CC.CCWord;
        if (PF_OCD_Count <= TA0_8s)             //permanent fail  over current in discharge
        {
            if ((cur_tmp) > PF_OCD_Default) //放电 50A
                PF_OCD_Count = PF_OCD_Count + 1;
            else
                PF_OCD_Count = 0;
        }
        if (PT_ODP_Count <= TA0_8s)             //over discharge current protection
        {

            if ((cur_tmp) > PT_ODP_Default) //放电 25A
                PT_ODP_Count = PT_ODP_Count + 1;
            else
                PT_ODP_Count = 0;
        }
    }
    else
    {
    	PT_ODP_Count = 0;
    	PF_OCD_Count = 0;
        //充电 过流
        if (PF_OCC_Count <= TA0_8s)
        {
            if ((BatteryData.CC.CCWord > PF_OCC_Default))   //6A       //permanent fail  over current in charge
                PF_OCC_Count = PF_OCC_Count + 1;
            else
                PF_OCC_Count = 0;
        }

        if (PT_OCP_Count <= TA0_8s)
        {
            if (BatteryData.CC.CCWord > PT_OCP_Default)  //2.6A
                PT_OCP_Count = PT_OCP_Count + 1;
            else
                PT_OCP_Count = 0;                       //over charge current protection
        }
    }





//    if (SystemMode == SystemChargeMode)
//    {
    if (PT_OTCP_Count <= TA0_8s)                //over temperature charge protection
    {
        if (BatteryData.TS1.TS1Word <= PT_OTCP_Default || RT3_AD >= PT_OTCP_Def_RT3)     //190902 wk(增加RT3)//fenglong 20190607
            PT_OTCP_Count = PT_OTCP_Count + 1;
        else
            PT_OTCP_Count = 0;
    }

    if (PT_UTCP_Count <= TA0_8s)
    {
        if (BatteryData.TS1.TS1Word >= PT_UTCP_Default || RT3_AD <= PT_UTCP_Def_RT3)    //190902 wk(增加RT3)
            PT_UTCP_Count = PT_UTCP_Count + 1;
        else
            PT_UTCP_Count = 0;                              //under temperature charge protection
    }

    if (AL_OTDA_Count <= TA0_8s)
    {
        if (BatteryData.TS1.TS1Word < ALARM_OTDA_Default)
            AL_OTDA_Count = AL_OTDA_Count + 1;
        else
            AL_OTDA_Count = 0;                              //under temperature charge protection
    }
    if (AL_UTDA_Count <= TA0_8s)
    {
       if (BatteryData.TS1.TS1Word >= ALARM_UTDA_Default)
           AL_UTDA_Count = AL_UTDA_Count + 1;
       else
           AL_UTDA_Count = 0;                              //under temperature charge protection
    }
    if (AL_OTMA_Count <= TA0_8s)
    {
       if (BatteryData.TS2.TS2Word < ALARM_OTMA_Default)
           AL_OTMA_Count = AL_OTMA_Count + 1;
       else
           AL_OTMA_Count = 0;                              //under temperature charge protection
    }

    if (PT_OTDP_Count <= TA0_8s)                            //over temperature discharge protection
    {
        if (BatteryData.TS1.TS1Word <= PT_OTDP_Default || RT3_AD >= PT_OTDP_Def_RT3)    //190902 wk(增加RT3)
            PT_OTDP_Count = PT_OTDP_Count + 1;
        else
            PT_OTDP_Count = 0;
    }

    if (PT_UTDP_Count <= TA0_8s)                        //under temperature discharge protection
    {
        if (BatteryData.TS1.TS1Word >= PT_UTDP_Default || RT3_AD <= PT_UTDP_Def_RT3)    //190902 wk(增加RT3)
            PT_UTDP_Count = PT_UTDP_Count + 1;
        else
            PT_UTDP_Count = 0;
    }

//fenglong 20190704  低压后需要充电到3.4V才能动作
    if ((CellMiniVoltage >= PT_UV_Release) && (FW_PT_Status.Bits.PT_UV == 1) && (SystemMode == SystemChargeMode) )
    {
        FW_PT_Status.Bits.PT_UV = 0;
        PT_UV_Count = 0;

    }
    //20190704
//    if (FW_PT_Status.Bits.PT_ODP == 1)          //over discharge current protection
//        PT_ODP_AutoReleaseCount = PT_ODP_AutoReleaseCount + 1;
//                //放电过流2级保护                                                  //short current detection  短路监测电流

//    if ((HW_PT_Status.Bits.HW_OCD == 1) || (HW_PT_Status.Bits.HW_SCD))			//20190704 客户要求取消
//	{
//    	HW_PT_OCD_SCD_AutoReleaseCount = HW_PT_OCD_SCD_AutoReleaseCount + 1;	//20190704 客户要求取消
//	}

}




void ImbalanceCheck(void)
{
    if(T0_INT_count>0)
    {
        if(Imbalance_Count <= TA0_10m )
        {
            if( ( (CellMAXiVoltage - CellMiniVoltage) >=700 ) && (CellMAXiVoltage>=3700) )
            {
                Imbalance_Count++;
            }
            else
            {
                Imbalance_Count=0;
            }
        }

        if(Imbalance_Count >= PF_VIMA_Delay220S )
        {
            FW_PF_Status.Bits.PF_VIMA=1;
        }
    }
}


void PortectVBat(void)
{

//    BatteryData.VBat.VBatWord;



}
