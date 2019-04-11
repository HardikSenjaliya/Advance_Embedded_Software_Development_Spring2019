//*****************************************************************************
//
// freertos_demo.c - Simple FreeRTOS example.
//
// Copyright (c) 2012-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.4.178 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "utils/uartstdio.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "portmacro.h"
#include "task.h"
#include "include/main.h"
//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>FreeRTOS Example (freertos_demo)</h1>
//!
//! This application demonstrates the use of FreeRTOS on Launchpad.
//!
//! The application blinks the user-selected LED at a user-selected frequency.
//! To select the LED press the left button and to select the frequency press
//! the right button.  The UART outputs the application status at 115,200 baud,
//! 8-n-1 mode.
//!
//! This application utilizes FreeRTOS to perform the tasks in a concurrent
//! fashion.  The following tasks are created:
//!
//! - An LED task, which blinks the user-selected on-board LED at a
//!   user-selected rate (changed via the buttons).
//!
//! - A Switch task, which monitors the buttons pressed and passes the
//!   information to LED task.
//!
//! In addition to the tasks, this application also uses the following FreeRTOS
//! resources:
//!
//! - A Queue to enable information transfer between tasks.
//!
//! - A Semaphore to guard the resource, UART, from access by multiple tasks at
//!   the same time.
//!
//! - A non-blocking FreeRTOS Delay to put the tasks in blocked state when they
//!   have nothing to do.
//!
//! For additional details on FreeRTOS, refer to the FreeRTOS web page at:
//! http://www.freertos.org/
//

#define TEMP_TASK_PERIOD            (1000)
#define LED_TASK_PERIOD             (100)
#define SYSTEM_CLOCK                (120000000)

/*Function Prototypes*/
void initialize_hardware_timer0(void);
void disable_hardware_timer0(void);

/*Binary semaphores for scheduling tasks from the timer handler*/

xSemaphoreHandle g_pTempTaskSemaphore;
xSemaphoreHandle g_pLoggerTaskSemaphore;
xSemaphoreHandle g_pLEDTaskSemaphore;

/*Mutex for sharing UART*/
xSemaphoreHandle g_pUARTSemaphore;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}

#endif

//*****************************************************************************
//
// This hook is called by FreeRTOS when an stack overflow error is detected.
//
//*****************************************************************************
void vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName)
{
    //
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    //
    while (1)
    {
    }
}

/*TImer0 Interrupt Handler*/
void Timer0BIntHandler(void)
{

    //UARTprintf("Timer Handler!\n");

    /*Clear the TIMER0 interrupt*/
    TimerIntClear(TIMER0_BASE, TIMER_TIMB_TIMEOUT);

    static uint32_t count = 0;

    count = (count + 1) % 1000;
    //UARTprintf("Count value is %d\n", counter);

    if ((count % TEMP_TASK_PERIOD) == 0)
    {
        xSemaphoreGive(g_pTempTaskSemaphore);
    }

    if ((count % LED_TASK_PERIOD) == 0)
    {
        xSemaphoreGive(g_pLEDTaskSemaphore);
    }

}

void diable_hardware_timer0()
{
    /*Diable TIMER0*/
    IntDisable(INT_TIMER0B);

    /*Disable TIMER0 Interrupt*/
    TimerIntDisable(TIMER0_BASE, TIMER_TIMB_TIMEOUT);

    /*Clear pending interrupts if any*/
    TimerIntClear(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
}

//*****************************************************************************
//
// Configure Hardware Timer0 to fire every 1ms.
// Reference - Tivaware Examples for Timer peripheral
//*****************************************************************************

void initialize_hardware_timer0()
{

    /*Enable TIMER0 peripheral*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    /*Enable Processor Interrupts*/
    IntMasterEnable();

    /*Configure TIMER0*/
    TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_PERIODIC);

    TimerLoadSet(TIMER0_BASE, TIMER_B, SysCtlClockGet() / 1000);

    /*Enable Interrupt for TIMER0 when timer time out and enable
     * the interrupt for timer0 in processor*/
    TimerIntEnable(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
    IntEnable(INT_TIMER0B);

    /*Enable the timer*/
    TimerEnable(TIMER0_BASE, TIMER_B);

}

//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
// @source credit - TI Examples
//*****************************************************************************
void ConfigureUART(void)
{

    /*Enable GPIO for UART module*/
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    /*Enable UART module*/
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    /*COnfigure GPIO alternate fucntions for UART0*/
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    /*Set UART clock*/
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    /*Config UART for STDOUT*/
    UARTStdioConfig(0, 115200, 16000000);
}

//*****************************************************************************
//
// Initialize FreeRTOS and start the initial set of tasks.
//
//*****************************************************************************
int main(void)
{

    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
    SYSCTL_XTAL_16MHZ);

    ConfigureUART();

    UARTprintf("\n\nStarted Executing Main thread !\n");
    UARTprintf("System Clock Value %d\n", SysCtlClockGet());

    g_pUARTSemaphore = xSemaphoreCreateMutex();

    /*Create a binary semaphore to signal the task from the timer handler*/
    g_pUARTSemaphore = xSemaphoreCreateMutex();
    g_pTempTaskSemaphore = xSemaphoreCreateBinary();
    g_pLoggerTaskSemaphore = xSemaphoreCreateBinary();
    g_pLEDTaskSemaphore = xSemaphoreCreateBinary();

    /*Create a Timer*/
    initialize_hardware_timer0();

    /*Create task for Temperature Sensor*/
    if (TaskTempInit() != 0)
    {

        while (1)
        {
        }
    }

    /*Create task for Toggling LEDs*/
    if (TaskLEDInit() != 0)
    {

        while (1)
        {
        }
    }

    /*Create task for Logging data*/
    if (TaskLoggerInit() != 0)
    {

        while (1)
        {
        }
    }

    /*Create task for Logging data*/
    if (TaskAlertInit() != 0)
    {

        while (1)
        {
        }
    }

    /*Start Scheduler*/
    vTaskStartScheduler();

    while (1)
    {
    }
}
