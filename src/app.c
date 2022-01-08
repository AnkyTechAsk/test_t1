/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include <stdio.h>

#include "config/default/peripheral/sercom/usart/plib_sercom0_usart.h"
#include "config/default/peripheral/port/plib_port.h"
#include "config/default/peripheral/tc/plib_tc2.h"
#include "config/default/peripheral/eic/plib_eic.h"


// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
#define SW_01_ADDR      (0xfe)   
#define SW_01_CMD       (0x50)
#define SW_02_ADDR      (0xfe)   
#define SW_02_CMD       (0xd8)
#define SW_03_ADDR      (0xfe)   
#define SW_03_CMD       (0xf8)


#define SEC_TO_US_COUNT (70000)

volatile uint32_t usec50 = 0;
volatile bool nec_ok;
volatile uint8_t nec_state, bit_n;
volatile uint32_t nec_code;

typedef struct{
    uint8_t inv_addr;
    uint8_t cmd;
}NEC_DATA;

NEC_DATA Nec_Data;

typedef struct{
    bool G1;
    bool G2;
    bool G3;
}NEC_LOGICSTATUS;

NEC_LOGICSTATUS Nec_logicStatus;

uint8_t DebugBuff[256] = {0};
uint8_t MsgLen = 0;

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************
void tc2_USec50_Callback (TC_TIMER_STATUS status, uintptr_t context)
{
    usec50 = usec50 + 50;
    TC2_TimerStart();
}

void IR_ISR (uintptr_t context)
{
    switch(nec_state)
    {
        case 0 :                // start receiving IR data (we're at the beginning of 9ms pulse)
            usec50 = 0;         // reset USec time
            TC2_TimerStart();   // Start Us timer
            nec_state = 1;      // next state: end of 9ms pulse            
            bit_n = 0;
            nec_code = 0;

            break;
 
        case 1 :

           if(usec50 > 10500)
           {
                nec_state = 2;  // next state: end of 4ms space
                usec50 = 0;         // reset USec time
           }

           break;
 
        case 2 :
            if( usec50 > 1700)  // if period is more than 1600us
                nec_code |=   (uint32_t)1 << (31 - bit_n);   // write 1 to bit (31 - bit_n)

            else                // if period is less than 1600us
                nec_code &= ~((uint32_t)1 << (31 - bit_n));  // write 0 to bit (31 - bit_n)

            bit_n++;

            if(bit_n > 28)
            {
                nec_ok = 1;   // decoding process OK
                EIC_InterruptDisable( EIC_PIN_3 );
            }
            usec50 = 0;

            break;            
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************
void WhiteLion_ResetVar(void)
{
    nec_code = 0;
    Nec_Data.inv_addr = 0;
    Nec_Data.cmd = 0;
    nec_ok = 0;
    nec_state = 0;
    usec50 = 0;
}

void WhiteLion_LedFunctions(NEC_DATA *pdata)
{
    if((pdata->inv_addr == SW_01_ADDR) && (pdata->cmd == SW_01_CMD) )
    {
        if(Nec_logicStatus.G1)
        {
            Nec_logicStatus.G1 = false;
            G1_Set();// G1 ON
            W1_Clear();// W1 OFF            
        }
        else
        {
            Nec_logicStatus.G1 = true;
            G1_Clear();// G1 OFF
            W1_Set();// W1 ON
        }
    }
    
    if((pdata->inv_addr == SW_02_ADDR) && (pdata->cmd == SW_02_CMD))
    {
        if(Nec_logicStatus.G2)
        {
            Nec_logicStatus.G2 = false;
            G2_Set();// G2 ON
            W2_Clear();// W2 OFF
        }
        else
        {
            Nec_logicStatus.G2 = true;
            G2_Clear();// G2 OFF
            W2_Set();// W2 ON
        }
    }
    
    if((pdata->inv_addr == SW_03_ADDR) && (pdata->cmd == SW_03_CMD))
    {
        if(Nec_logicStatus.G3)
        {
            Nec_logicStatus.G3 = false;
            G3_Set();// G3 ON
            W3_Clear();// W3 OFF            
        }
        else
        {
            Nec_logicStatus.G3 = true;
            G3_Clear();// G3 OFF
            W3_Set();// W3 ON
        }
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    MsgLen = sprintf((char*)DebugBuff,"WhiteLion System Init...\n");
    SERCOM0_USART_Write(DebugBuff, MsgLen );

    TC2_TimerCallbackRegister( tc2_USec50_Callback, 0 );
    TC2_TimerStart();
    
    EIC_CallbackRegister( EIC_PIN_3, IR_ISR, 0);
    EIC_InterruptEnable( EIC_PIN_3);
    
    MsgLen = sprintf((char*)DebugBuff,"WhiteLion System Init Done!\n");
    SERCOM0_USART_Write(DebugBuff, MsgLen );

    appData.state = APP_STATE_INIT;
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    switch ( appData.state )
    {
        case APP_STATE_INIT:
        {
            appData.state = APP_STATE_SERVICE_TASKS;
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            while(1)
            {
                while(nec_ok == 0)
                {
                    if (usec50 > SEC_TO_US_COUNT) // if  sec without interrupt : reset all var
                    {
                        WhiteLion_ResetVar();
                    }
                }
                TC2_TimerStop(); // disable Timer1
                EIC_InterruptDisable( EIC_PIN_3 ); // disable IR interrupt
                
                Nec_Data.inv_addr = 0;
                Nec_Data.cmd = 0;
                
                Nec_Data.inv_addr = nec_code >> 21;
                Nec_Data.cmd = nec_code >> 13;                    

                MsgLen = sprintf((char*)DebugBuff,"Inv_Addr : 0x%x, Cmd : 0x%x\n",Nec_Data.inv_addr,Nec_Data.cmd);
                SERCOM0_USART_Write(DebugBuff, MsgLen );

                WhiteLion_LedFunctions(&Nec_Data);
              
                WhiteLion_ResetVar();
                
                EIC_InterruptEnable( EIC_PIN_3 ); // enable IR interrupt
            }

            break;
        }

        default:
        {
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
