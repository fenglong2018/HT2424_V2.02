


#include "system.h"
//#include"uart.h"

extern uchar CRC_DATA;
extern  UINT16 Sleep_delay;
extern uchar Alert_count;
#ifndef UART_FRAME

//	static uint8_t usartTxHead = 0;
//	static uint8_t usartTxTail = 0;
//
//	static uint8_t RxHead = 0;
//	static uint8_t RxTail = 0;
//
//	static uint8_t eusartTxBufferRemaining = 0;
//	static uint8_t eusartusartRxbufferRemaining = 0;
	
//	volatile uint8_t TxCount;
//	volatile uint8_t RxCount;
	
//	uint8_t usartTxBuffer[TX_BUFFER_SIZE];
volatile    uint8_t usartRxbuffer[RX_BUFFER_SIZE];

	_UsartSet RS_485;



#endif

//---------------------------------------------//


#ifdef UART_PRINTF
	int fputc(int _c, register FILE *_fp);
	int fputs(const char *_ptr, register FILE *_fp);
#endif

//---------------------------------------------//

#ifdef UART_PRINTF


	int fputc(int _c, register FILE *_fp)
	{
	
	
	//    WDT_RST_1S();
	    WDTCTL = WDT_ARST_1000;
	    while(!(IFG2&UCA0TXIFG));
	    UCA0TXBUF = (unsigned char) _c;
	
	    return((unsigned char)_c);
	}
	
	int fputs(const char *_ptr, register FILE *_fp)
	{
	    unsigned int i, len;
//	    WDT_RST_1S();
	    WDTCTL = WDT_ARST_1000;
	    len = strlen(_ptr);
	
	    for(i=0 ; i<len ; i++)
	    {
	        while(!(IFG2&UCA0TXIFG));
	        UCA0TXBUF = (unsigned char) _ptr[i];
	    }
	    return len;
	}
	
	void Uart_Init_Debug(void) //ACLK=32.678K
	{
	    //Select the IO's function to uart.
	    P1DIR  |= BIT2;             //P1.2  UART_TX
	    P1DIR  &=~BIT1;             //P1.1  UART_RX
	    P1SEL  |= BIT1+BIT2;        //select P1.1 and P1.2 as UART port
	    P1SEL2 |= BIT1+BIT2;

	    RS485_RE;

	    //Set the bus as uart.
	    UCA0CTL1 |=UCSWRST;         //reset UART module,as well as enable UART module
	    UCA0CTL1 |=UCSSEL0; // CLK = ACLK
	    UCA0BR0 = 0x78;     //0x03; // 32kHz/9600 = 3.41
	    UCA0BR1 = 0x00;
	    UCA0MCTL = UCBRS1 + UCBRS0; // UCBRSx = 0.41闂佽壈鎻幏锟�8=3.28闂備礁鎲￠悷锕傛偋濡ゅ懎姹查柨鐕傛嫹
	    UCA0CTL1 &=~UCSWRST;        //UART reset end
	    IE2 |= UCA0RXIE ;  //Open the UART recievc interrupt
	}
	
#else


    void Uart_State_Init(void)
    {
        RS_485.RxCount = 0;
        RS_485.RxStatus = USART_RX_IDLE;
        RS_485.TxCount = 0;
        RS_485.TxLength = 0;
        RS_485.TxStatus = USART_TX_IDLE;

        RS_485.NewReceiveFlag = 0;

    }

#endif

//---------------------------------------------//



	

	
//	void Uart_Init() //SMCLK
//	{
//		UCA0CTL1 = UCSWRST;
//	    P1DIR  |= BIT2;             //P1.2  UART_TX
//	    P1DIR  &=~BIT1;             //P1.1  UART_RX
//	    P1SEL  |= BIT1+BIT2;        //select P1.1 and P1.2 as UART port
//	    P1SEL2 |= BIT1+BIT2;
//
//		UCA0CTL1 |= UCSSEL_2;                     // SMCLK ,8N1
//		UCA0BR0 = 138;                            // 8M  57600
//		UCA0BR1 = 0x00;                              //
//		UCA0MCTL = 4;              //
//		UCA0CTL1 &= ~UCSWRST;      //闁荤姳绀佹晶浠嬫偪閸℃稑鐭楅柛灞剧♁濞堝爼鏌ｉ姀銏犳瀾闁哄拑鎷�
//	    IE2 |= UCA0RXIE ;  //Open the UART recievc interrupt
//
//	    Uart_State_Init();
//	}
	
	
	
	#pragma vector=USCIAB0RX_VECTOR

	__interrupt void USCI0RX_ISR(void)
	{
//      _bic_SR_register_on_exit(LPM0_bits);// �˳��͹���ģʽ		fenglong 20190628
		_bic_SR_register_on_exit(LPM3_bits);// �˳��͹���ģʽ		fenglong 20190628
		Sleep_delay=0;
	    //Byte_Recv  = UCA0RXBUF;      //load the recieve byte
	    IFG2 &=~UCA0RXIFG;         //clear RX interrupt flag

	    usartRxbuffer[RS_485.RxCount] = UCA0RXBUF;

		if (usartRxbuffer[0]!=UART_SOI)
		{
			RS_485.RxCount=0;
			RS_485.RxStatus = USART_RX_IDLE;
		}
		else
		{
            if(usartRxbuffer[RS_485.RxCount]==UART_EOI)         //RECEIVE END
            {
                RS_485.RxStatus = USART_RX_OK;
                RS_485.RxCount++;
            }
            else
            {
                RS_485.RxCount++;
                RS_485.RxStatus=USART_RX_BUSY;
            }
		}

		if(RS_485.RxCount>=RX_BUFFER_SIZE-2)
		{
		    RS_485.RxCount=0;
		    usartRxbuffer[0]=0;
		    RS_485.RxStatus = USART_RX_IDLE;
		}
//		_bic_SR_register_on_exit(LPM3_bits);// �˳��͹���ģʽ
	}

void UART_Transmit(UINT8 *txarr,UINT16 length)
{
    UINT8  temp ;
    UINT16 i;

    for(i=0;i<length;i++)
    {

        while (!(IFG2&UCA0TXIFG));
        temp=(txarr[i] & 0xF0)>>4;

        if(temp>9)
        {
            UCA0TXBUF = temp + 0x37;
            CRC_DATA += temp + 0x37;
        }
        else
        {
            UCA0TXBUF = temp + 0x30;
            CRC_DATA += temp + 0x30;
        }



        while (!(IFG2&UCA0TXIFG));

        temp=txarr[i] & 0x0F;

        if(temp>9)
        {
            UCA0TXBUF = temp + 0x37;
            CRC_DATA += temp + 0x37;
        }
        else
        {
            UCA0TXBUF = temp + 0x30;
            CRC_DATA += temp + 0x30;
        }
        while (!(IFG2&UCA0TXIFG));
    }


}
	



	



