/*
 * led_task.c
 *
 *  Created on: 08-Apr-2019
 *      Author: hardi
 */

#include "include/led_task.h"
#include "include/logger_task.h"
#include <string.h>

#define QUEUE_ITEM_SIZE                  sizeof(led_message_t)

/*Queue Handle for sending data to Logger task*/
QueueHandle_t g_pLEDTaskQueue;



extern xSemaphoreHandle g_pLEDTaskSemaphore;

void turn_led_on(void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
}

void turn_led_off(void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x0);
}

void configure_gpio_led()
{

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
}

static void TaskFunction(void *pvParameters)
{
    static int toggle_count = 0;
    led_message_t message;

    while (1)
    {
        if ( xSemaphoreTake(g_pLEDTaskSemaphore, portMAX_DELAY) == pdTRUE)
        {
            if ((toggle_count % 2) == 0)
            {
                turn_led_off();
            }
            else
            {
                turn_led_on();
            }

            message.toggle_count = toggle_count;
            message.time_stamp = xTaskGetTickCount();
            strcpy(message.name, "Hardik");
            //UARTprintf("Toggle Count = %d\n", toggle_count);
            if (xQueueSend(g_pLEDTaskQueue, &message, portMAX_DELAY) != pdPASS)
            {
                UARTprintf("\nQueue is EMPTY\n");
            }

            toggle_count += 1;

        }
    }
}

uint32_t TaskLEDInit()
{

    if (xTaskCreate(TaskFunction, (const portCHAR *) "TASK_LED", TASKSTACKSIZE,
            NULL, tskIDLE_PRIORITY + PRIORITY_LED_TASK, NULL) != pdTRUE)
    {
        UARTprintf("ERROR: creating LED task\n");
        return (1);
    }

    g_pLEDTaskQueue = xQueueCreate(QUEUE_SIZE, QUEUE_ITEM_SIZE);

    if (g_pLEDTaskQueue == NULL)
    {
        UARTprintf("ERROR: while creating LED queue\n");
        exit(1);
    }

    configure_gpio_led();

    return (0);
}
