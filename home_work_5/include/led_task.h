/*
 * led_task.h
 *
 *  Created on: 08-Apr-2019
 *      Author: hardi
 */

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include "FreeRTOSConfig.h"
#include "utils/uartstdio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include <stdlib.h>

#define QUEUE_SIZE                  (10)

#define TASKSTACKSIZE               (128)
#define PRIORITY_LED_TASK           (3)


/*typedef struct{

    TickType_t time_stamp;
    int toggle_count;
}led_message_t;*/

uint32_t TaskLEDInit(void);
void configure_gpio_led(void);
void turn_led_on(void);
void turn_led_off(void);
