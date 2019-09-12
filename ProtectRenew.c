/*
 * ProtectRenew.c
 *
 *  Created on: 2019��5��8��
 *      Author: 20190211012
 */

#include "ProtectRenew.h"
#include "system.h"

extern UINT8 T0_INT_count;




void ProtectRenew(void)
{
    if(T0_INT_count>0)
    {
//        T0Counter ();
        ProtectDetect();

        _nop ();

        LEDForBootUpMode ();
        TA11_Breathing ();

        ShipMode();
    }
}
