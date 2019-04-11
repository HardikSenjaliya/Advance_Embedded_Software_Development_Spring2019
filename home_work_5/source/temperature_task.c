/*
 * temperature_task.c
 *
 *  Created on: 08-Apr-2019
 *      Author: hardik
 */

#include "include/temperature_task.h"

#define TMP102_SLAVE_ADDRESS                    (0x48)
#define TEMP_THRESHOLD_THIGH                    (25)
#define TEMP_THRESHOLD_TLOW                     (24)

QueueHandle_t g_pTempTaskQueue;
extern TaskHandle_t g_alertTaskHandle;
extern xSemaphoreHandle g_pTempTaskSemaphore;

/*
 * @source credit: use from the LECTURE demo code
 * */
double read_temperature()
{
    /*Set slave address*/
    I2CMasterSlaveAddrSet(I2C0_BASE, TMP102_SLAVE_ADDRESS, false);

    /*Put the address of the register to write*/
    I2CMasterDataPut(I2C0_BASE, REG_TEMPERATURE);

    /*Send data*/
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);

    /*
     while (!I2CMasterBusy(I2C0_BASE))
     ;
     */

    /*Wait till bus is busy*/
    while (I2CMasterBusy(I2C0_BASE))
        ;

    /*Set slave address*/
    I2CMasterSlaveAddrSet(I2C0_BASE, TMP102_SLAVE_ADDRESS, true);

    /*Starrt multiple bytes receiver*/
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);

    /*    while (!I2CMasterBusy(I2C0_BASE))
     ;*/

    /*Wait till bus is busy*/
    while (I2CMasterBusy(I2C0_BASE))
        ;

    /*Read MSB byte*/
    uint32_t data_one = I2CMasterDataGet(I2C0_BASE);

    /*Send receiver end command*/
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

    /*    while (!I2CMasterBusy(I2C0_BASE))
     ;*/

    /*Wait till master is busy*/
    while (I2CMasterBusy(I2C0_BASE))
        ;

    /*Recevie lsb*/
    uint32_t data_two = I2CMasterDataGet(I2C0_BASE);

    /*Set MSB byte as MSB and check for negative sign*/
    int16_t temperature = (data_one << 8) | data_two;

    temperature >>= 4;  // this should do sign extension

    float temp;
    bool negative_temp = false;

    if (temperature < 0)
    {
        temperature = ~temperature + 1;
        negative_temp = true;
    }

    temp = 0.0625 * temperature;

    if (negative_temp)
    {
        temp *= -1.0;
    }

    return temp;
}

void TMP102_init()
{

    /*Enable Clock in RCGCI2C*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

    /*Wait for I2C0 to be ready for programming*/
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0))
        ;

    /*Enable Clock for the GPIO to be used for I2C -> PB2 I2C0SCL and PB3 I2C0SDA; Pin 47 and 48*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    /*Enable GPIO Alternate Functions*/
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);

    /*Enable PB3 as Open Drain*/
    /*Configure GPIOCTL -> write 3 for both PB2 and PB3 for I2C signal*/
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);

    /*Initialize I2C Master write I2CMCR with 0x00000010*/
    /*Set the I2C clock*/
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);
    I2CMasterEnable(I2C0_BASE);

    /*Enable I2C0 interrupt*/
    //IntEnable(INT_I2C0);
}

static void TaskFunction(void *pvParameters)
{

    temp_message_t message;

    while (1)
    {
        if ( xSemaphoreTake(g_pTempTaskSemaphore, portMAX_DELAY) == pdTRUE)
        {
            message.temperature = read_temperature();
            message.time_stamp = xTaskGetTickCount();

            if (xQueueSend(g_pTempTaskQueue, &message, portMAX_DELAY) != pdPASS)
            {
                UARTprintf("\nQueue is EMPTY\n");
            }

            if (message.temperature > TEMP_THRESHOLD_THIGH
                    || message.temperature < TEMP_THRESHOLD_TLOW)
            {

                if ( xTaskNotify(g_alertTaskHandle, message.temperature,
                                 eSetValueWithOverwrite) == pdPASS)
                {
                    /* The task's notification value was updated. */
                }
                else
                {
                    /* The task's notification value was not updated. */
                }

            }

        }
    }
}

uint32_t TaskTempInit()
{

    if (xTaskCreate(TaskFunction, (const portCHAR *) "TASK_TEMP", TASKSTACKSIZE,
            NULL, tskIDLE_PRIORITY + PRIORITY_TEMP_TASK, NULL) != pdTRUE)
    {
        UARTprintf("ERROR: creating Temperature task\n");
        return (1);
    }

    g_pTempTaskQueue = xQueueCreate(QUEUE_SIZE, QUEUE_ITEM_SIZE);

    if (g_pTempTaskQueue == NULL)
    {
        UARTprintf("ERROR: while creating TEMP queue\n");
        exit(1);
    }

    TMP102_init();

    return (0);
}

