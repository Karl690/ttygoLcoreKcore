
#include "cmdprocessor.h"

uint32_t cmd_nextcommand_insertion_pointer = 1;
uint32_t cmd_current_pointer = 1;
uint32_t cmd_commands_in_que = 0;
char cmd_commands_in_que_buffer[SIZE_OF_COMMAND_QUEUE][MAX_COMMAND_LEN];

GMCommandStructure *cmd_execution_ptr = NULL;
GMCommandStructure cmd_que[SIZE_OF_COMMAND_QUEUE];
