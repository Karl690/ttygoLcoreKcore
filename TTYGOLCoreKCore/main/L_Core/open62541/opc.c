#include "configure.h"
#ifdef USE_OPC

#include <pthread.h>
#include "open62541.h"
#include "opc.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "K_Core/display/DisplayList.h"

bool g_bRunning  = false;
bool OpcRunning = true;
uint32_t OpcHeartBeat = 0;
UA_NodeId   g_lastNodeId;

int NodeidHeartbeat = 0;
const static char* OPC_TAG = "OPC";
static UA_UsernamePasswordLogin logins[3] = {
    {UA_STRING_STATIC("karl690"), UA_STRING_STATIC("karl690")},
    {UA_STRING_STATIC("hyrel"), UA_STRING_STATIC("hyrel")}
};

#define MAX_OPC_NODE 20
uint16_t OpcNodeCount = 0;
OpcNodeItem opcNodeList[MAX_OPC_NODE];

static UA_StatusCode
UA_ServerConfig_setUriName(UA_ServerConfig *uaServerConfig, const char *uri, const char *name)
{
    // delete pre-initialized values
    UA_String_clear(&uaServerConfig->applicationDescription.applicationUri);
    UA_LocalizedText_clear(&uaServerConfig->applicationDescription.applicationName);

    uaServerConfig->applicationDescription.applicationUri = UA_String_fromChars(uri);
    uaServerConfig->applicationDescription.applicationName.locale = UA_STRING_NULL;
    uaServerConfig->applicationDescription.applicationName.text = UA_String_fromChars(name);

    for (size_t i = 0; i < uaServerConfig->endpointsSize; i++)
    {
        UA_String_clear(&uaServerConfig->endpoints[i].server.applicationUri);
        UA_LocalizedText_clear(
            &uaServerConfig->endpoints[i].server.applicationName);

        UA_String_copy(&uaServerConfig->applicationDescription.applicationUri,
                       &uaServerConfig->endpoints[i].server.applicationUri);

        UA_LocalizedText_copy(&uaServerConfig->applicationDescription.applicationName,
                              &uaServerConfig->endpoints[i].server.applicationName);
    }

    return UA_STATUSCODE_GOOD;
}



int addNodeObject(UA_Server *server, int parentid, const char* objectName, const char* description) {
	UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
	UA_NodeId   NodeId;
	oAttr.description = UA_LOCALIZEDTEXT((char*)"en-US", (char*)description);
	oAttr.displayName = UA_LOCALIZEDTEXT((char*)"en-US", (char*)objectName);
	UA_Server_addObjectNode(server,
		UA_NODEID_NULL,
		UA_NODEID_NUMERIC(0, parentid), //UA_NS0ID_OBJECTSFOLDER),
		UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
		UA_QUALIFIEDNAME(1, (char*)objectName),
		UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
		oAttr,
		NULL,
		&NodeId);
	//lastNodeId = outNodeId;
	return NodeId.identifier.numeric;
}



int addVariable(UA_Server *server, int parentid, const char* variableName, int dataType, bool bWritable) {
	UA_VariableAttributes attr = UA_VariableAttributes_default;
	
	attr.description = UA_LOCALIZEDTEXT((char*)"en-US", (char*)variableName);
	attr.displayName = UA_LOCALIZEDTEXT((char*)"en-US", (char*)variableName);
	attr.dataType = UA_TYPES[dataType].typeId;
	
	if (bWritable)
		attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
	else
		attr.accessLevel = UA_ACCESSLEVELMASK_READ;
	UA_NodeId childId;
	UA_Server_addVariableNode(server,
		UA_NODEID_NULL, //UA_NODEID_STRING(1, (char*)variableName), //
		UA_NODEID_NUMERIC(0, parentid),
		UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT), // UA_NS0ID_ORGANIZES
		UA_QUALIFIEDNAME(1, (char*)variableName),
		UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
		attr,
		NULL,
		&childId);

	return childId.identifier.numeric;
}

bool WriteIntVariable(UA_Server *server, int nodeId, int value)
{
	UA_NodeId NodeId = UA_NODEID_NUMERIC(0, nodeId);
	UA_VariableAttributes Attr;
	UA_Variant_init(&Attr.value);
	UA_Variant_setScalar(&Attr.value, &value, &UA_TYPES[UA_TYPES_INT32]); //UA_TYPES_INT32
	UA_StatusCode status = UA_Server_writeValue(server, NodeId, Attr.value);
	if (status != UA_STATUSCODE_GOOD) {
		return false;
	}
	return true;
}

bool WriteFloatVariable(UA_Server *server, int nodeId, float value)
{
	UA_NodeId NodeId = UA_NODEID_NUMERIC(0, nodeId);
	UA_VariableAttributes Attr;
	UA_Variant_init(&Attr.value);
	UA_Variant_setScalar(&Attr.value, &value, &UA_TYPES[UA_TYPES_FLOAT]); //UA_TYPES_INT32
	UA_StatusCode status = UA_Server_writeValue(server, NodeId, Attr.value);
	if (status != UA_STATUSCODE_GOOD) {
		return false;
	}
	return true;
}
bool WriteDoubleVariable(UA_Server *server, int nodeId, double value)
{
	UA_NodeId NodeId = UA_NODEID_NUMERIC(0, nodeId);
	UA_VariableAttributes Attr;
	UA_Variant_init(&Attr.value);
	UA_Variant_setScalar(&Attr.value, &value, &UA_TYPES[UA_TYPES_DOUBLE]); //UA_TYPES_INT32
	UA_StatusCode status = UA_Server_writeValue(server, NodeId, Attr.value);
	if (status != UA_STATUSCODE_GOOD) {
		return false;
	}
	return true;
}

bool WriteBooleanVariable(UA_Server *server, int nodeId, bool value)
{
	UA_NodeId NodeId = UA_NODEID_NUMERIC(0, nodeId);
	UA_VariableAttributes Attr;
	UA_Variant_init(&Attr.value);
	UA_Variant_setScalar(&Attr.value, &value, &UA_TYPES[UA_TYPES_BOOLEAN]); //UA_TYPES_INT32
	UA_StatusCode status = UA_Server_writeValue(server, NodeId, Attr.value);
	if (status != UA_STATUSCODE_GOOD) {
		return false;
	}
	return true;
}

bool WriteStringVariable(UA_Server *server, int nodeId, char* value)
{
	UA_NodeId NodeId = UA_NODEID_NUMERIC(0, nodeId);
	UA_VariableAttributes Attr;
	UA_Variant_init(&Attr.value);	
	UA_String s = UA_STRING_ALLOC((char*)value);
	UA_Variant_setScalar(&Attr.value, &s, &UA_TYPES[UA_TYPES_STRING]); //UA_TYPES_INT32
	UA_StatusCode status = UA_Server_writeValue(server, NodeId, Attr.value);
	if (status != UA_STATUSCODE_GOOD) {
		return false;
	}
	return true;
}


bool writeVariable(UA_Server *server, int nodeId, void* val, int type) {	

	switch (type)
	{
	case UA_TYPES_STRING: 
		WriteStringVariable(server, nodeId, (char*)val);
		break;
	case UA_TYPES_FLOAT: 
		WriteFloatVariable(server, nodeId, *(float*)val);
		break;
	case UA_TYPES_DOUBLE:
		WriteFloatVariable(server, nodeId, *(float*)val);
		break;
	case UA_TYPES_BOOLEAN:
		WriteBooleanVariable(server, nodeId, *(bool*)val);
		break;
	case UA_TYPES_INT16:
	case UA_TYPES_INT32:
		WriteBooleanVariable(server, nodeId, *(int*)val);
		break;
	}
	return true;
}

void createNodeList(UA_Server* server,  char* name, DisplayVariableInfo* variableList )
{
	int parentId = addNodeObject(server, UA_NS0ID_OBJECTSFOLDER, name, "NameSpace");
	int i = 0, childId = 0;
	int c = 0;
	while (variableList[i].VariablePointer)
	{
		childId = 0;
		switch (variableList[i].FuncType)
		{
		case FUNC_INT:
		case FUNC_INT16:
		case FUNC_INT32:
			childId = addVariable(server, parentId, variableList[i].Label, UA_TYPES_INT32, true);
			variableList[i].OpcNodeId = childId;
			opcNodeList[OpcNodeCount].nodeType = UA_TYPES_INT32;
			break;
		case FUNC_ASCII:
			childId = addVariable(server, parentId, variableList[i].Label, UA_TYPES_STRING, true);
			variableList[i].OpcNodeId = childId;
			opcNodeList[OpcNodeCount].nodeType = UA_TYPES_STRING;
			break;
		case FUNC_FLOAT:
			childId = addVariable(server, parentId, variableList[i].Label, UA_TYPES_FLOAT, true);
			variableList[i].OpcNodeId = childId;
			opcNodeList[OpcNodeCount].nodeType = UA_TYPES_FLOAT;
			break;
		case FUNC_BOOLEAN:
			childId = addVariable(server, parentId, variableList[i].Label, UA_TYPES_BOOLEAN, true);
			variableList[i].OpcNodeId = childId;
			opcNodeList[OpcNodeCount].nodeType = UA_TYPES_BOOLEAN;
			break;
		default:
			break;
		}
		if (childId != 0)
		{
			opcNodeList[OpcNodeCount].nodeId = childId;
			strcpy(opcNodeList[OpcNodeCount].nodeName, variableList[i].Label);
			opcNodeList[OpcNodeCount].nodeValue = variableList[i].VariablePointer;
			OpcNodeCount++;
		}
		i++;
	}
}

void thread_opc_task(void* arg) {
	UA_Int32 sendBufferSize = 1024 * 8;
	UA_Int32 recvBufferSize = 1024 * 8;
    
	ESP_LOGI(OPC_TAG, "Fire up OPC UA Server.");
    UA_Server *server = UA_Server_new();
    UA_ServerConfig *config = UA_Server_getConfig(server);
    UA_ServerConfig_setMinimalCustomBuffer(config, 4840, 0, sendBufferSize, recvBufferSize);
			
    const char *appUri = "open62541.esp321.server";
    UA_String hostName = UA_STRING("opcua-esp32");

    UA_ServerConfig_setUriName(config, appUri, "OPC_UA_Server_ESP32");
    UA_ServerConfig_setCustomHostname(config, hostName);

    config->accessControl.clear(&config->accessControl);
    config->shutdownDelay = 5000.0;
    logins[1].username = UA_STRING("hyrel");
	logins[1].password = UA_STRING("hyrel");
    UA_StatusCode retval = UA_AccessControl_default(config, false,
        &config->securityPolicies[config->securityPoliciesSize - 1].policyUri, 2, logins);
	
	createNodeList(server, "LcdVarsTable", LcdVarsTable);
	//createNodeList(server, "Lcd1VarsTable", Lcd1VarsTable);
	
	retval = UA_Server_run_startup(server);
    g_bRunning = true;
	uint16_t count = 0;
    if (retval == UA_STATUSCODE_GOOD)
    {
        while (g_bRunning) {
	        if (OpcRunning)
	        {
		        UA_Server_run_iterate(server, false);		        
		        writeVariable(server, opcNodeList[count].nodeId, opcNodeList[count].nodeValue, opcNodeList[count].nodeType);
		        count++;
		        if (count >= OpcNodeCount) count = 0;
		        
		        OpcHeartBeat++; 
	        }      
	        vTaskDelay(100 / portTICK_PERIOD_MS);
            
        }
    }
    if (!g_bRunning)
        UA_Server_run_shutdown(server);
    g_bRunning = false;
}

void InitOPC() {    
	//xTaskCreatePinnedToCore(thread_opc_task, "opcua_task", 1024*10, NULL, 5, NULL, 0);
	xTaskCreate(thread_opc_task, "opcua_task", 10240, NULL, 10, NULL);
}
#endif