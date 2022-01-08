/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    touch_app.c

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

#include "touch_app.h"
#include "touch/touch.h"
#include "config/default/peripheral/port/plib_port.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
// Global Variable
extern volatile uint8_t measurement_done_touch;

//// Define Functions
//static void touch_LED_status_display(void);

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the TOUCH_APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

TOUCH_APP_DATA touch_appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/
void touch_LED_status_display(void)
{
    uint8_t key_status = 0u;
    static int l1;
    key_status = get_sensor_state(0) & KEY_TOUCHED_MASK;
    if (0u != key_status) {
        if(l1==1)
        {
            //Touch detect
            G3_Set();
            W3_Clear();
            GM_Clear();
            WM_Set();

            // Relay Status
            RLY3_Set();
            l1=0;
        }
        else
        {
            //Touch No detect
            G3_Clear();
            W3_Set();

            // Relay Status
            RLY3_Clear();
            l1=1;
        }
    }

    key_status = get_sensor_state(1) & KEY_TOUCHED_MASK;
    if (0u != key_status) {
        if(l1==1)
        {
            //Touch detect
            G2_Set();
            W2_Clear();
            GM_Clear();
            WM_Set();

            // Relay Status
            RLY2_Set();
            l1=0;
        }
        else
        {
            //Touch No detect
            G2_Clear();
            W2_Set();

            // Relay Status
            RLY2_Clear();
            l1=1;
        } 
    }
    

    key_status = get_sensor_state(2) & KEY_TOUCHED_MASK;
    if (0u != key_status) {
        if(l1==1)
        {
            //Touch detect
            G1_Set();
            W1_Clear();
            GM_Clear();
            WM_Set();

            // Relay Status
            RLY1_Set();
            l1=0;
        }
        else 
        {
            //Touch No detect
            G1_Clear();
            W1_Set();

            // Relay Status
            RLY1_Clear();
            l1=1;
        }
    }
    

    key_status = get_sensor_state(3) & KEY_TOUCHED_MASK;
    if (0u != key_status) {
        if(l1==1)
        {
            //Touch detect
            GM_Set();
            WM_Clear();
            G1_Clear();
            W1_Set();
            G2_Clear();
            W2_Set();
            G3_Clear();
            W3_Set();

            // Relay Status
            RLY1_Clear();
            RLY2_Clear();
            RLY3_Clear();
            l1=0;
        }
        else
        {
            //Touch No detect
            GM_Clear();
            WM_Set();
            G1_Set();
            W1_Clear();
            G2_Set();
            W2_Clear();
            G3_Set();
            W3_Clear();

            // Relay Status
            RLY1_Set();
            RLY2_Set();
            RLY3_Set();
            l1=1;
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
    void TOUCH_APP_Initialize ( void )

  Remarks:
    See prototype in touch_app.h.
 */

void TOUCH_APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    touch_appData.state = TOUCH_APP_STATE_INIT;



    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void TOUCH_APP_Tasks ( void )

  Remarks:
    See prototype in touch_app.h.
 */

void TOUCH_APP_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( touch_appData.state )
    {
        /* Application's initial state. */
        case TOUCH_APP_STATE_INIT:
        {
            touch_appData.state = TOUCH_APP_STATE_SERVICE_TASKS;
            break;
        }

        case TOUCH_APP_STATE_SERVICE_TASKS:
        {
            while(1)
            {
                /* call touch process function */
                touch_process();

                if(measurement_done_touch == 1)
                {
                    measurement_done_touch = 0;
                    // process touch data
                    touch_LED_status_display();
                }
            }
            break;
        }

        /* TODO: implement your application state machine.*/


        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
