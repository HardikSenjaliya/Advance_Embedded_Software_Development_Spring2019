/*
 * temperature_task.h
 *
 *  Created on: 08-Apr-2019
 *      Author: hardi
 */

#ifndef INC_TEMPERATURE_TASK_H_
#define INC_TEMPERATURE_TASK_H_

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include "FreeRTOSConfig.h"
#include "utils/uartstdio.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "include/logger_task.h"

#include <stdlib.h>


#define TASKSTACKSIZE               (128)
#define PRIORITY_TEMP_TASK          (4)
#define QUEUE_ITEM_SIZE                         sizeof(temp_message_t)
#define QUEUE_SIZE                  (10)


#define REG_TEMPERATURE             (0)
#define REG_TLOW                    (2)
#define REG_THIGH                   (3)
#define REG_CONFIG                  (1)

#define NEGATIVE_CHECK              (10000000)

uint32_t TaskTempInit(void);
void TMP102_init(void);
uint8_t i2c_read_one_byte(uint8_t slave_address);
void i2c_write_one_byte(uint8_t slave_address, uint8_t write_data);
double read_temperature(void);
double binary_to_decimal(int16_t binary_temperature);

#endif /* INC_TEMPERATURE_TASK_H_ */
