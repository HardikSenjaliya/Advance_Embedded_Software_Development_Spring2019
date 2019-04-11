/*
 * alert_task.c
 *
 *  Created on: 09-Apr-2019
 *      Author: hardi
 */

#include "include/alert_task.h"

TaskHandle_t g_alertTaskHandle;

static void TaskFunction(void *pvParameters)
{
    UARTprintf("Alert TASK\n");

    uint32_t ulNotifiedValue = 0;

    while (1)
    {
        xTaskNotifyWait(0x00, 0xFFFF, &ulNotifiedValue, portMAX_DELAY);

        UARTprintf("\n\nNotification Received @Temperature value of - %d\n\n", ulNotifiedValue);

    }
}

uint32_t TaskAlertInit()
{

    if (xTaskCreate(TaskFunction, (const portCHAR *) "TASK_ALERT",
            TASKSTACKSIZE, NULL, tskIDLE_PRIORITY + PRIORITY_ALERT_TASK,
            &g_alertTaskHandle) != pdTRUE)
    {
        UARTprintf("ERROR: creating Alert task\n");
        return (1);
    }

    return (0);
}

