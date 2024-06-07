#ifndef LOGGER_H_
#define LOGGER_H_
/*
	we have three files to log to -
		STATUS - we print whether an init was successfull or failed.
*/
#include <stdio.h>
#include <stdbool.h>
#define LOG_LEVELS 3
#define LOG_FILES 2

enum log_level{
	TRACE,	//specific line logs - idk debugger seems more useful, might remove later
	ERROR,	//error logs
	DEBUG,	//debug logs - emulator status
	BDOS	//bdos calls - printing characters or outputs
};

enum log_files {
	STATUS,
	INSTRUCTIONS
};

extern bool continue_test;

void init_logger();							//init the files used for logging
void debug_print(const char*, int);			//to stdout. param - string, log-level
void log_instruction_state(state_8080);	//to file. param - cpu-state BEFORE INSTRUCTION EXECUTION.
void log_instruction_state_ret(state_8080);	
void log_instruction_state_2(state_8080);	
void log_emulator_status_ch(const char ch);
void log_emulator_status(const char*, int);
void terminate_logger();					//flush and close files.
#endif