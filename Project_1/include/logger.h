/*
 * logger.h
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

#include "utils.h"

/*function prototypes*/
void *run_logger(void *params);
void log_message(FILE *fp, long int time_stamp, unsigned int thread_id, int log_level, char *message);

#endif /* LOGGER_H_ */
