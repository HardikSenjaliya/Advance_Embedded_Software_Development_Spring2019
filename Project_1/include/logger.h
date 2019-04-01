/*
 * logger.h
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include "utils.h"

typedef struct {

	char file_name[100];

} logfile_attr_t;

/*function prototypes*/
void *run_logger(void *params);
void log_message(FILE *fp, long int time_stamp, char *thread_name,
		int log_level, char *message);
void file_backup(char *source_file);

#endif /* LOGGER_H_ */
