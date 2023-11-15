
#include "bcode.h"
#include "k_core/tools/tools.h"
#include "cmdprocessor.h"

/* BCODE 10: Reqeust the head position
	format: B10 
	response: >RG:[Head position]
**/
void BCODE_10(GMBCOMMAND* cmd)
{
	tools_report_register();
}
/* BCODE 100: Set the tool params
	format: B100 T# P# Q# R# S#	
		   T: Head address
		   P: Desired Temperature
		   Q: Desired Heater Duty cycle
		   R: Desired Fan Duty cycle
		   S: Aux analog
**/
void BCODE_100(GMBCOMMAND* cmd)
{
	if (!ARG_PRESENT(cmd->T)) return;
	if (ARG_PRESENT(cmd->P)) toolInfo.DesiredTemperature = cmd->P;
	if (ARG_PRESENT(cmd->Q)) toolInfo.DesiredHeaterDutyCycle = cmd->Q;
	if (ARG_PRESENT(cmd->R)) toolInfo.DesiredFanDutyCycle = cmd->R;
	if (ARG_PRESENT(cmd->S)) toolInfo.ActualAux = cmd->S;
	
	/* this is for testing.*/
	toolInfo.ActualTemperature = toolInfo.DesiredTemperature;
	toolInfo.ActualHeaterDutyCycle = toolInfo.DesiredHeaterDutyCycle;
	toolInfo.ActualFanDutyCycle = toolInfo.DesiredFanDutyCycle;
	
}