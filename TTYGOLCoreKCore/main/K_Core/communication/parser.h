#pragma once
#include "main.h"
#include "communication.h"
#define UNPROCESSED -1 // used to init cmdLink on cmdQue CleanUpPointers ARG_N_PRESENT

void parser_reset_gcode_buffer(ComBuffer* BufferToReset);
void parser_incomming_into_gcodes();