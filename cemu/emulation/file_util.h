#ifndef FILE_UTIL_H_
#define FILE_UTIL_H_

#include <stdio.h>
#include <stdint.h>
#include "state.h"

enum rom { CPUDIAG = '6', EXM = '5', EXER = '4', PRE = '3', CPUTEST = '2', ROM = '1' };

int load_file(state_8080*, char);

#endif