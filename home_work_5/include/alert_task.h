/*
 * alert_task.h
 *
 *  Created on: 09-Apr-2019
 *      Author: hardi
 */

#ifndef INCLUDE_ALERT_TASK_H_
#define INCLUDE_ALERT_TASK_H_

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include "FreeRTOSConfig.h"
#include "utils/uartstdio.h"

#define TASKSTACKSIZE               (128)
#define PRIORITY_ALERT_TASK         (1)
#define QUEUE_SIZE                  (10)

uint32_t TaskAlertInit(void);

#endif /* INCLUDE_ALERT_TASK_H_ */
