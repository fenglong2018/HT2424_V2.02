//************************************************************
//  Data        :   2019/04/17
//  Programmer  :   Garfield.Lin
//  Function    :   System initial
//  MCU         :   MSP430G2553IPW28
//  Oscillator  :   Internal DOC 12MHZ
//  Version     :   V0.1
//
//
//****************************************************************


#include "System.h"
#include "Initial.h"
#include "uart.h"

extern union FW_PT FW_PT_Status;
extern union SW_ALARM ALARM_PT_Status;        //����ֵ״̬
extern _BatteryData BatteryData;
extern RegisterGroup   BQ76930Data;
UINT16 his_data[FLASH_DATA_NUM];
//extern _FlashData BQ76930ProtectData;

void    Calibration (void)
{
    DCOCTL = 0x60;
    BCSCTL1 = BCSCTL1 | 0x0F;
    BCSCTL2 = 0x00;                          //MCLK = DCOCLK,(1:1), SMCLK = DCOCLK,(1:1)
    BCSCTL3 |= LFXT1S_2;                      // LFXT1 = VLO

    if (CALBC1_12MHZ==0xFF)                    // If calibration constant erased
    {
        while(1)                               // If calibration constants erased                                          // do not load, trap CPU!!
        {
            asm("  MOV &0xFFFE,PC;");//��PCת�Ƶ�Ӧ�ó���ĸ�λ������..���û���ʱ��ǵ�ǰ���пո񣡣���              //fenglong 20190615
        }

    }

    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_12MHZ;                    // Set DCO to 12MHz
    DCOCTL =  CALDCO_12MHZ;

}

void    GPIOInitial (void)
{
 //   EN_ACLK;        //Test
 //   EN_SMCLK;


    EN_LED_1_P21;
    EN_LED_2_P22;
    EN_LED_3_P33;
    EN_LED_4_P32;
    EN_LED_5_P24;
    EN_LED_6_P25;

    ADC_TEMP;

    EN_SW_pin;
    EN_DC_IN_pin;
    EN_76930_Alert_Pin;
    EN_WakeUP_Pin;

    EN_RE_485_Pin;
    EN_CP_Pin;
    EN_DE_485_Pin;
    EN_MCU_SAFE_Pin;

    LED_OFF();

    CP_High;
    EN_RT_ON_Pin;
    RT_ON_LOW;     //wk    190830
    //RT_ON_HIGH;

}


#ifndef UART_PRINTF

void UART_Initial (void)
{
    EN_UART_RX_P11;
    EN_UART_TX_P12;

    RS485_RE;

    UCA0CTL0 = 0x00;
    UCA0CTL1 = UCA0CTL1 | UCSSEL1;
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**

    UCA0BR0 = 78;                             // 12MHz 9600
    UCA0BR1 = 0;                              // 12MHz 9600

    UCA0MCTL = UCBRS_0 + UCOS16 + UCBRF_2;    // Modulation UCBRSx = 0, UCBRSx = 2, UCOS16 = 1;

    IFG2 &= ~(UCA0RXIFG);
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
//    IE2 |= UCA0TXIE;                        // Enable USCI_A0 TX interrupt
}

#endif


void I2C_Initial (void)
{
    IIC_Init();
    /*
    EN_P16_OUT;
    EN_P17_OUT;
    _nop ();
    //EN_I2C_SCL_P16;
    //EN_I2C_SDA_P17;

    UCB0CTL1 |= UCSWRST;                      // Enable SW reset
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // Master, I2C Mode,synchronous mode
    UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
//    UCB0BR0 = 120;                             // fSCL = SMCLK/120 = ~100kHz
    UCB0BR0 = 140;                             // fSCL = SMCLK/120 = 80kHz
    UCB0BR1 = 0;
    UCB0I2CSA = BQ76930_Address;                         // Set slave address
    UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
//    IE2 |= UCB0RXIE;                          // Enable RX interrupt
 //   TACTL = TASSEL_2 + MC_2;                  // SMCLK, contmode
  * */

}


void his_data_init(void)//����PFʱ���������ʷ��¼������֮�����ԭ���Ų�
{
 his_data[HIS_Cell1MiniVoltage]=4200;                                        //fenglong 20190617
 his_data[HIS_Cell2MiniVoltage]=4200;                                        //fenglong 20190617
 his_data[HIS_Cell3MiniVoltage]=4200;                                        //fenglong 20190617
 his_data[HIS_Cell4MiniVoltage]=4200;                                        //fenglong 20190617
 his_data[HIS_Cell5MiniVoltage]=4200;                                        //fenglong 20190617
 his_data[HIS_Cell6MiniVoltage]=4200;                                        //fenglong 20190617
 his_data[HIS_Cell7MiniVoltage]=4200;                                        //fenglong 20190617

 his_data[HIS_Cell1MAXiVoltage]=0;                                        //fenglong 20190730
 his_data[HIS_Cell2MAXiVoltage]=0;                                        //fenglong 20190730
 his_data[HIS_Cell3MAXiVoltage]=0;                                        //fenglong 20190730
 his_data[HIS_Cell4MAXiVoltage]=0;                                        //fenglong 20190730
 his_data[HIS_Cell5MAXiVoltage]=0;                                        //fenglong 20190730
 his_data[HIS_Cell6MAXiVoltage]=0;                                        //fenglong 20190730
 his_data[HIS_Cell7MAXiVoltage]=0;                                        //fenglong 20190730

 his_data[HIS_MINVOLT] = 0xffff;
 his_data[HIS_MAXVOLT] = 0;

 his_data[HIS_MOSFET_MAXTEMP]=4000;           //wk 190905

 his_data[HIS_CHG_MAXCURRENT] = 0;            //wk 190905
 his_data[HIS_DSG_MAXCURRENT] = 0;            //wk 190905

 his_data[HIS_CHG_MAXTEMP] = 0;               //wk 190905
 his_data[HIS_DSG_MAXTEMP] = 0;               //wk 190905
}


void Timer_A_initial (void)
{
    TA0CTL = TASSEL_2 + ID_3 + MC_1 + TACLR + TAIE;     //Timer 0 , SMCLK / 8 , TA0CCTL0 , INT

    TA0CCR0 = TA0_20ms;
    TA0CCR1 = 0xF000;
    TA0CCR2 = 0xF000;

    Clear_TA0CCTL0_CCIFG;
    Clear_TA0CCTL1_CCIFG;
    Clear_TA0CCTL2_CCIFG;


    TA1CTL = TASSEL_2 + ID_0 + MC_1 + TACLR;            //Timer 1 , SMCLK / 1 , TA1CCTL0 , PWM
    TA1CCTL0 = OUTMOD_6;
    TA1CCTL1 = OUTMOD_6;
    TA1CCTL2 = OUTMOD_6;

    TA1CCR0 = 0xFFFF;
   // TA1CCR1 = 0x2000;
    //TA1CCR2 = 0x2000;

    Clear_TA1CCTL0_CCIFG;
    Clear_TA1CCTL1_CCIFG;
    Clear_TA1CCTL2_CCIFG;
}

void ConfigWDT(void)
 {
//  WDTCTL = WDTPW + WDTHOLD;   // �رտ��Ź�
    WDTCTL = WDT_ARST_1000;             //AID 20190717
    //IFG1 &= ~WDTIFG;            //������Ź��жϱ�־
    //IE1 |= WDTIE;               // ���Ź��ж�ʹ��10/23xxy


 }

void RAM_Init(void)
{
    ALARM_PT_Status.Byte = 0 ;
    BatteryData.CC.CCWord =0;
    BatteryData.VBat.VBatWord =22000;
    BatteryData.TS1.TS1Word =3000;
    BatteryData.TS2.TS2Word =3000;
    BatteryData.VCell1.VCell1Word =3600;
    BatteryData.VCell2.VCell2Word =3600;
    BatteryData.VCell3.VCell3Word =3600;
    BatteryData.VCell4.VCell4Word =3600;
    BatteryData.VCell5.VCell5Word =3600;
    BatteryData.VCell6.VCell6Word =3600;
    BatteryData.VCell7.VCell7Word =3600;

    FW_PT_Status.Word=0;        //fenglong 20190602

}

void    SystemInitial (void)
{
    Calibration ();
    FCTL2 = FWKEY + FSSEL_1 + FN5;        //Set fFTG = MCLK / 32,

    GPIOInitial ();

    Timer_A_initial ();


#ifndef UART_PRINTF

    UART_Initial ();

#else

    Uart_Init_Debug();

#endif

    I2C_Initial ();
    ConfigWDT();

    SWInit();

    RAM_Init();


    __bis_SR_register(  GIE);       // Enter LPM3 w/ interrupt
    //BQ DELAY
    _delay_cycles(2000);
    _delay_cycles(2000);
    _delay_cycles(2000);
}


#if 0

void InitIO()
{
    //���õ�IO��������������IOĬ��״̬Ϊ����״̬
    P1DIR &=~ (BIT0 | BIT3 | BIT4 | BIT5);  //1:output; 0:input
    //P2.0  ALERT P2.4 DLOAD
    P2DIR |= (BIT1 | BIT2 | BIT3 | BIT5);
    //P3.0     P3.6 ADAPTER
//    P3DIR |= ( BIT2  | BIT4 | BIT5 | BIT6 | BIT7);
    P3DIR |= (BIT1|BIT2|BIT3|BIT4|BIT5 |BIT7 );
    P3DIR &=~ (  BIT0 | BIT6 |BIT5);

    P1OUT &= ~(BIT0 | BIT3 | BIT4);     //0:low  1:high
    P2OUT &= ~(BIT1|BIT2|BIT4 | BIT5);
//    P3OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6);
    P3OUT &= ~(BIT1 |BIT2| BIT3 | BIT4  | BIT7);
    DIS_NTC();
    CTRL_SW_OFF();
}

void SW_IO_INIT(void)
{
//  P2REN |= BIT0;
//  P2OUT |= BIT0;  //����

    P1SEL &=~BIT5;
//  P1IES &= ~BIT5;     // �����ش���
    P1IES |= BIT5;      // �½��ش���
    P1IFG &= ~BIT5;     //���IO���жϱ�־
    P1IE  |= BIT5;      // �ж�ʹ��
}

void BQ_ALERT_IO_INIT(void)
{
//  P2REN |= BIT0;
//  P2OUT |= BIT0;  //����

    P2SEL &=~BIT0;
    P2IES &= ~BIT0;     // �ж�  P2.1�����ش���
    P2IFG &= ~BIT0;     //���IO���жϱ�־
    P2IE  |= BIT0;      // �ж�ʹ��
}

#pragma vector=WDT_VECTOR       //���Ź��ж����
interrupt void WDT(void)
{
    IFG1 &= ~WDTIFG;            //������Ź��жϱ�־
    _bic_SR_register_on_exit(LPM3_bits);// �˳��͹���ģʽ
}

#pragma vector=PORT1_VECTOR
interrupt void PORT1_ISR (void)
{
    if(P1IFG&BIT5)
    {
        P1IFG &= ~BIT5;         //���IO���жϱ�־
        if(SW_IN)
        {
            CTRL_SW_ON();
            delay_us(100);
            CTRL_SW_OFF();
            P1IES |= BIT5;      // �½��ش���
            g_sys_flags.val.key_down_flag = FALSE;
            l_key_down_cnt = 0 ;
            /*zzy�����������*/
            if(FALSE == g_my_flags.val.keycontin2 )
            {
                g_my_flags.val.keycontin2 = TRUE;
            }
//          keycontin_toSD_tick = 0;//�ſ���ť����
        }
        else
        {
            /*zzy�����������*/
            if(TRUE == g_my_flags.val.keycontin2 )
            {
                keycontin10 ++;
                g_my_flags.val.keycontin2 =FALSE;
            }
            if((FALSE == g_sys_flags.val.en_soc_disp_flag))
            {
                delay_ms(3);
                l_shutdown_cnt = g_250ms_tick;
                if(SW_IN)
                {

                }
                else
                {
                    if((FALSE == g_sys_flags.val.en_soc_disp_flag))
                    {
                        g_key_first_down =1;
                        g_sys_flags.val.en_soc_disp_flag = TRUE;
//                      g_my_flags.val.keycontin10_Open_led = FALSE;
                        keycontin10 = 0 ;//���¼���
                        if((FALSE == g_my_flags.val.keycontin10_Open_led )) //���������������
                        {
                            keycontin_tick = 0 ;//���¼�ʱ
                        }
                    }

                    if(g_sys_flags.val.key_down_flag == FALSE)
                    {
                        g_my_flags.val.en_fail_led_flag = TRUE;
                        g_sys_flags.val.key_down_flag = TRUE;
                    }
                }
            }
            P1IES &= ~BIT5;     // �����ش���

        }
         _bic_SR_register_on_exit(LPM3_bits);// �˳��͹���ģʽ
    }
}

#pragma vector=PORT2_VECTOR
interrupt void PORT2_ISR (void)
{
    if(P2IFG&BIT0)
    {
        P2IFG &= ~BIT0;         //���IO���жϱ�־
         _bic_SR_register_on_exit(LPM3_bits);// �˳��͹���ģʽ
    }
}

#endif






