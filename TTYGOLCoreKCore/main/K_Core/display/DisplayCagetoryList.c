#include "K_Core/K_Core.h"
#include "DisplayCagetoryList.h"
#include "L_Core/wifi/wifi.h"
#include "RevisionHistory.h"

AmplifierInfo amplifierInfoList[GROUP_SIZE] = {};
DisplayGroupInfo displayGroupInfo[GROUP_SIZE] = { 
	{ 
		.name = "Amplifer - 1",
		.LiveParameters = {
			.name = LIVE_PARAM_NAME,
			.parameters = {
				{ &amplifierInfoList[0].liveData.Unit,				"Unit#",					FUNC_INT16,		COLOR_WHITE, COLOR_RED, 0, EDIALBE, OPC_TRUE,0, NULL },
				{ &amplifierInfoList[0].liveData.Channel,			"Channel#",					FUNC_ASCII,		COLOR_WHITE, COLOR_RED, 0, EDIALBE, OPC_TRUE, 0, NULL },
				{ &amplifierInfoList[0].liveData.Power1,			"Power1",					FUNC_INT16,		COLOR_WHITE, COLOR_RED, 0, EDIALBE, OPC_TRUE, 0, NULL },
				{ &amplifierInfoList[0].liveData.Power2,			"Power2",					FUNC_INT16,		COLOR_WHITE, COLOR_RED, 0, EDIALBE, OPC_TRUE, 0, NULL },
				{ &amplifierInfoList[0].liveData.Frequence1,		"Frequence1",				FUNC_INT16,		COLOR_WHITE, COLOR_RED, 0, READONLY, OPC_TRUE, 0, NULL },
				{ &amplifierInfoList[0].liveData.Frequence2,		"Frequence2",				FUNC_INT16,		COLOR_WHITE, COLOR_RED, 0, READONLY, OPC_TRUE, 0, NULL },
				{ &amplifierInfoList[0].liveData.BathTemperature,	"BathTemperature",			FUNC_INT16,		COLOR_WHITE, COLOR_RED, 0, READONLY, OPC_TRUE, 0, NULL },
				{ &amplifierInfoList[0].liveData.TC1,				"TC1",						FUNC_INT16,		COLOR_WHITE, COLOR_RED, 0, EDIALBE, OPC_TRUE, 0, NULL },
				{ &amplifierInfoList[0].liveData.TC2,				"TC2",						FUNC_INT16,		COLOR_WHITE, COLOR_RED, 0, EDIALBE, OPC_TRUE, 0, NULL },
				{ &amplifierInfoList[0].liveData.TC6,				"TC6",						FUNC_INT16,		COLOR_WHITE, COLOR_RED, 0, EDIALBE, OPC_TRUE, 0, NULL },
				{ &amplifierInfoList[0].liveData.Status,			"Status1",					FUNC_BOOLEAN,	COLOR_WHITE, COLOR_RED, 0, READONLY, OPC_FALSE, 0, NULL },
				{ &amplifierInfoList[0].liveData.Status,			"Status2",					FUNC_BOOLEAN,	COLOR_WHITE, COLOR_RED, 0, EDIALBE, OPC_TRUE, 0, NULL },
				{0},
			},
		},
		.ReceipeParameters = {
			.name = RECIEPE_PARAM_NAME,
			.parameters = {
				{ &amplifierInfoList[0].reciepeData.ProcessTime,	"ProcessTime", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, 0, 0, 0, NULL },
				{ &amplifierInfoList[0].reciepeData.DesiredPower, "Desired Power", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, 0, 0, 0, NULL },				
				{0},
			}
		},
		.FrequenceParameters = {
			.name = FREQUENCY_PARAM_NAME,
			.parameters = {
				{ &amplifierInfoList[0].freqData.Frequence1, "Frequence1", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, 0, 0, 0, NULL },
				{ &amplifierInfoList[0].freqData.Frequence2, "Frequence2", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, 0, 0, 0, NULL },
				{ &amplifierInfoList[0].freqData.Frequence3, "Frequence3", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, 0, 0, 0, NULL },
				{ &amplifierInfoList[0].freqData.Frequence4, "Frequence4", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, 0, 0, 0, NULL },
				{ &amplifierInfoList[0].freqData.Frequence5, "Frequence5", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, 0, 0, 0, NULL },
				{ &amplifierInfoList[0].freqData.Frequence6, "Frequence6", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, 0, 0, 0, NULL },
				{ &amplifierInfoList[0].freqData.Frequence7, "Frequence7", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, 0, 0, 0, NULL },
				{0},
			}
		},
	},
	
	{ 
		.name = "Amplifer - 2",
		.LiveParameters = {
			.name = LIVE_PARAM_NAME,
			.parameters = {
				{ &amplifierInfoList[1].liveData.Unit, "Unit#", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, EDIALBE, OPC_TRUE, 0, NULL },
				{ &amplifierInfoList[1].liveData.Channel, "Channel#", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, READONLY, OPC_TRUE, 0, NULL },
				{0},
			},
		},
		.ReceipeParameters = {
			.name = RECIEPE_PARAM_NAME,
			.parameters = {
				{ &amplifierInfoList[1].reciepeData.ProcessTime, "ProcessTime", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, EDIALBE, OPC_TRUE, 0, NULL },
				{ &amplifierInfoList[1].reciepeData.DesiredPower, "Desired Power", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, EDIALBE, OPC_TRUE, 0, NULL },
				{0},
			}
		},
		.FrequenceParameters = {
			.name = FREQUENCY_PARAM_NAME,
			.parameters = {
				{ &amplifierInfoList[1].freqData.Frequence1, "Frequence1", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, READONLY, OPC_TRUE, 0, NULL },
				{ &amplifierInfoList[1].freqData.Frequence2, "Frequence2", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, READONLY, OPC_TRUE, 0, NULL },				
				{ &amplifierInfoList[1].freqData.Frequence3, "Frequence3", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, READONLY, OPC_TRUE, 0, NULL },
				{ &amplifierInfoList[1].freqData.Frequence4, "Frequence4", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, READONLY, OPC_TRUE, 0, NULL },
				{ &amplifierInfoList[1].freqData.Frequence5, "Frequence5", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, READONLY, OPC_TRUE, 0, NULL },
				{ &amplifierInfoList[1].freqData.Frequence6, "Frequence6", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, READONLY, OPC_TRUE, 0, NULL },
				{ &amplifierInfoList[1].freqData.Frequence7, "Frequence7", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, READONLY, OPC_TRUE, 0, NULL },
				{0},
			}
		},
	},
	
	{ 
		.name = "Amplifer - 3",
		.LiveParameters = {
			.name = LIVE_PARAM_NAME,
			.parameters = {
				{ &amplifierInfoList[2].liveData.Unit, "Unit#", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, READONLY, OPC_TRUE, 0, NULL },
				{ &amplifierInfoList[2].liveData.Channel, "Channel#", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, READONLY, OPC_TRUE, 0, NULL },
				{0},
			},
		},
		.ReceipeParameters = {
			.name = RECIEPE_PARAM_NAME,
			.parameters = {
				{ &amplifierInfoList[2].reciepeData.ProcessTime, "ProcessTime", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, READONLY, OPC_TRUE, 0, NULL },
				{ &amplifierInfoList[2].reciepeData.DesiredPower, "Desired Power", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, READONLY, OPC_TRUE, 0, NULL },
				{0},
			}
		},
		.FrequenceParameters = {
			.name = FREQUENCY_PARAM_NAME,
			.parameters = {
				{ &amplifierInfoList[2].freqData.Frequence1, "Frequence1", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, READONLY, OPC_TRUE, 0, NULL },
				{ &amplifierInfoList[2].freqData.Frequence2, "Frequence2", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, READONLY, OPC_TRUE, 0, NULL },
				{0},
			}
		},
	},
	
	{ 
		.name = "Amplifer - 4",
		.LiveParameters = {
			.name = LIVE_PARAM_NAME,
			.parameters = {
				{ &amplifierInfoList[3].liveData.Unit, "Unit#", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, READONLY, OPC_TRUE, 0, NULL },
				{ &amplifierInfoList[3].liveData.Channel, "Channel#", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, READONLY, OPC_TRUE, 0, NULL },
				{0},
			},
		},
		.ReceipeParameters = {
			.name = RECIEPE_PARAM_NAME,
			.parameters = {
				{ &amplifierInfoList[3].reciepeData.ProcessTime, "ProcessTime", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, READONLY, OPC_TRUE, 0, NULL },
				{ &amplifierInfoList[3].reciepeData.DesiredPower, "Desired Power", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, READONLY, OPC_TRUE, 0, NULL },
				{0},
			}
		},
		.FrequenceParameters = {
			.name = FREQUENCY_PARAM_NAME,
			.parameters = {
				{ &amplifierInfoList[3].freqData.Frequence1, "Frequence1", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, READONLY, OPC_TRUE, 0, NULL },
				{ &amplifierInfoList[3].freqData.Frequence2, "Frequence2", FUNC_INT16, COLOR_WHITE, COLOR_RED, 0, READONLY, OPC_TRUE, 0, NULL },
				{0},
			}
		},
	},
};

void ResetGroupList()
{
//	uint16_t i = 0;
//	for (i = 0; i < GROUP_SIZE; i++)
//	{
//		memset(displayGroupInfo[i].LiveParameters.parameters, 0, sizeof(DisplayVariableInfo) * PARAM_SIZE);
//		memset(displayGroupInfo[i].ReceipeParameters.parameters, 0, sizeof(DisplayVariableInfo) * PARAM_SIZE);
//		memset(displayGroupInfo[i].FrequenceParameters.parameters, 0, sizeof(DisplayVariableInfo) * PARAM_SIZE);
//	}
}

void WriteParams(FILE* fp, DisplayVariableInfo* vars)
{
	char item[256] = { 0 };
	switch (vars->FuncType)
	{
	case FUNC_INT:
		sprintf(item, "%d,%s", *(int*)vars->VariablePointer, "INT");
		break;
	case FUNC_INT16:
		sprintf(item, "%d,%s", (int)*(uint16_t*)vars->VariablePointer,"INT16");
		break;
	case FUNC_INT32:
		sprintf(item, "%d,%s", (int)*(uint32_t*)vars->VariablePointer, "INT32");
		break;
	case FUNC_ASCII:
		sprintf(item, "%s,%s", (char*)vars->VariablePointer, "ASCII");
		break;
	case FUNC_FLOAT:
		sprintf(item, "%.3f,%s", *(float*)vars->VariablePointer, "FLOAT");
		break;
	case FUNC_BOOLEAN:
		sprintf(item, "%s,%s", *(char*)vars->VariablePointer?"TRUE":"FALSE", "BOOL");
		break;
	default:
		break;
	}
	fwrite(item, strlen(item), 1, fp);
	sprintf(item, ",%d,%d,%d,%s,%s\n", (int)vars->Color_1, (int)vars->Color_2, (int)vars->Offset, vars->Editable ? "EDITABLE" : "READONLY", vars->ExposedOpc ? "EXPOSED" : "NONE");
	fwrite(item, strlen(item), 1, fp);
	
}

bool SaveDisplayGroupList(const char* path)
{
	FILE* fp = fopen(path, "w");
	if (fp == NULL) return false;
	char line[1024] = "Group name,params,name,index,value,type,color_1,color_2,offset,editable,opc_exposed\n";
	fwrite(line, strlen(line), 1, fp);
	int param_index = 0;
	for(uint16_t i = 0; i < GROUP_SIZE; i ++)
	{
		param_index = 0;
		DisplayVariableInfo* vars = displayGroupInfo[i].LiveParameters.parameters;		
		while (vars[param_index].VariablePointer)
		{
			sprintf(line, "%s,%s,%d,%s,", displayGroupInfo[i].name, displayGroupInfo[i].LiveParameters.name, param_index, vars[param_index].Label);
			fwrite(line, strlen(line), 1, fp);
			WriteParams(fp, &vars[param_index]);	
			param_index++;
		}
		param_index = 0;
		vars = displayGroupInfo[i].ReceipeParameters.parameters;
		while (vars[param_index].VariablePointer)
		{
			sprintf(line, "%s,%s,%d,%s,", displayGroupInfo[i].name, displayGroupInfo[i].ReceipeParameters.name, param_index, vars[param_index].Label);
			fwrite(line, strlen(line), 1, fp);
			WriteParams(fp, &vars[param_index]);
			param_index++;
		}
		param_index = 0;
		vars = displayGroupInfo[i].FrequenceParameters.parameters;
		while (vars[param_index].VariablePointer)
		{
			sprintf(line, "%s,%s,%d,%s,", displayGroupInfo[i].name, displayGroupInfo[i].FrequenceParameters.name, param_index, vars[param_index].Label);
			fwrite(line, strlen(line), 1, fp);
			WriteParams(fp, &vars[param_index]);
			param_index++;
		}
	}
	fclose(fp);
	return true;
}


DISPLAYFUNCTYPE get_functype_from_string(const char* var)
{
	DISPLAYFUNCTYPE type = FUNC_INT;
	if (!strcmp(var, "INT")) type = FUNC_INT;
	else if (!strcmp(var, "INT16")) type = FUNC_INT16;
	else if (!strcmp(var, "INT32")) type = FUNC_INT32;
	else if (!strcmp(var, "ASCII")) type = FUNC_ASCII;
	else if (!strcmp(var, "FLOAT")) type = FUNC_FLOAT;
	else if (!strcmp(var, "BOOL")) type = FUNC_BOOLEAN;
	return type;
}

bool LoadDisplayGroupList(const char* path)
{
	FILE* fp = fopen(path, "r");
	if (fp == NULL) return false;
	
	
	char line[1024];
	uint16_t i = 0, j = 0,param_index;
	
	ResetGroupList();
	char vec_line[CSV_FIELD_NAM][256];
	while (fgets(line, 1024, fp))
	{
		
		char *data = strtok(line, ",");
		j = 0;
		while (data != NULL)
		{
			strcpy(vec_line[j], data);
			data = strtok(NULL, ",");
			j++;
		}
		if (j != CSV_FIELD_NAM) continue; // continue if the number of item is not same as CSV_FILED_NAM
		static DisplayGroupInfo *groupinfo = NULL;
		for (i = 0; i < GROUP_SIZE; i++) //find the GroupInfo 's pointer
		{
			if (!strcmp(vec_line[CSV_GROUPNAME], displayGroupInfo[i].name))
			{
				groupinfo = &displayGroupInfo[i];
				break;
			}
		}
		if (!groupinfo) continue;
		static DisplayVariableInfo *paraminfo = NULL;
		if (!strcmp(vec_line[CSV_PARAMNAME], LIVE_PARAM_NAME)) paraminfo = groupinfo->LiveParameters.parameters;
		else if (!strcmp(vec_line[CSV_PARAMNAME], FREQUENCY_PARAM_NAME)) paraminfo = groupinfo->FrequenceParameters.parameters;
		else if (!strcmp(vec_line[CSV_PARAMNAME], RECIEPE_PARAM_NAME)) paraminfo = groupinfo->ReceipeParameters.parameters;
		
		if (!paraminfo) continue;
		param_index = atoi(vec_line[CSV_VAR_INDEX]);
		if (param_index >= PARAM_SIZE) continue;
		if (!paraminfo[param_index].VariablePointer) continue;
		strcpy(paraminfo[param_index].Label, vec_line[CSV_VAR_LABEL]);
		DISPLAYFUNCTYPE type = get_functype_from_string(vec_line[CSV_VAR_TYPE]);
		
		switch (type)
		{
		case FUNC_ASCII:
			strcpy((char*)paraminfo[param_index].VariablePointer, vec_line[CSV_VAR_VALUE]);
			break;
		case FUNC_INT: 
			*(int*)paraminfo[param_index].VariablePointer = atoi(vec_line[CSV_VAR_VALUE]);
			break;
		case FUNC_INT16:
			*(uint16_t*)paraminfo[param_index].VariablePointer = atoi(vec_line[CSV_VAR_VALUE]);
			break;
		case FUNC_INT32:
			*(uint32_t*)paraminfo[param_index].VariablePointer = atoi(vec_line[CSV_VAR_VALUE]);
			break;
		case FUNC_FLOAT:
			*(float*)paraminfo[param_index].VariablePointer = atof(vec_line[CSV_VAR_VALUE]);
			break;
		case FUNC_BOOLEAN:
			*(bool*)paraminfo[param_index].VariablePointer = !strcmp(vec_line[CSV_VAR_VALUE], "TRUE")? true: false;
			break;
		default:
			break;
		}
		paraminfo[param_index].Color_1 = atoi(vec_line[CSV_VAR_COLOR_1]);
		paraminfo[param_index].Color_2 = atoi(vec_line[CSV_VAR_COLOR_2]);
		paraminfo[param_index].Offset = atoi(vec_line[CSV_VAR_OFFSET]);
		
		paraminfo[param_index].Editable = !strcmp(vec_line[CSV_VAR_EDITABLE], "EDITABLE")? true: false;
		paraminfo[param_index].ExposedOpc = !strcmp(vec_line[CSV_VAR_OPC], "EXPOSED")? true: false;
	}
	return true;
}