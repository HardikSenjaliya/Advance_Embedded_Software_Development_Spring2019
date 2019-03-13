/*
 * utils.c
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#include "../include/utils.h"

/**
 * @brief this function writes logs to the specified logfile
 * @param fp - pointer to the logfile
 * @param file - logfile name
 * @param line - line number in the source file
 * @param message - message to be logged in to the logfile
 * @return void
 */

void _log(FILE *fp, char *file, int line, char *message){

	fprintf(fp, "%s:%d %s\n", file, line, message);
}
