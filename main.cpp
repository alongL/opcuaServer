#include "open62541.h"

#include <iostream>
#include <string>
#include <thread>
using namespace std;




UA_Boolean running = true;




static void manuallyDefineStudent(UA_Server * server)
{
    UA_NodeId studentId; /* get the nodeid assigned by the server */
    UA_ObjectAttributes stuAttr = UA_ObjectAttributes_default;
    stuAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Student");
    UA_Server_addObjectNode(server, UA_NODEID_NULL,
                            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                            UA_QUALIFIEDNAME(1, "Student"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
                            stuAttr, NULL, &studentId);
    
    // add name
    UA_VariableAttributes nameAttr = UA_VariableAttributes_default;
    UA_String studentName = UA_STRING("Xiao Ming");
    UA_Variant_setScalar(&nameAttr.value, &studentName, &UA_TYPES[UA_TYPES_STRING]);
    UA_NodeId nameNodeId = UA_NODEID_STRING(1, "name");
    nameAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Name");
    UA_Server_addVariableNode(server, nameNodeId, studentId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              UA_QUALIFIEDNAME(1, "StudentName"),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), nameAttr, NULL, NULL);

    // add gender
    UA_VariableAttributes genderAttr = UA_VariableAttributes_default;
    UA_String gender = UA_STRING("Male");
    UA_Variant_setScalar(&genderAttr.value, &gender, &UA_TYPES[UA_TYPES_STRING]);
    UA_NodeId genderNodeId = UA_NODEID_STRING(1, "gender");
    genderAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Gender");
    UA_Server_addVariableNode(server, genderNodeId, studentId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              UA_QUALIFIEDNAME(1, "Gender"),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), genderAttr, NULL, NULL);
    
    // add age
    UA_VariableAttributes ageAttr = UA_VariableAttributes_default;
    UA_Byte age = 16;
    UA_Variant_setScalar(&ageAttr.value, &age, &UA_TYPES[UA_TYPES_BYTE]);
    UA_NodeId ageNodeId = UA_NODEID_STRING(1, "age");
    ageAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Age");
	ageAttr.accessLevel |= UA_ACCESSLEVELMASK_WRITE; // default readonly, add write privilege
    UA_Server_addVariableNode(server, ageNodeId, studentId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              UA_QUALIFIEDNAME(1, "Age"),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), ageAttr, NULL, NULL);

    // add height
    UA_VariableAttributes heightAttr = UA_VariableAttributes_default;
    UA_UInt16 height = 170;
    UA_Variant_setScalar(&heightAttr.value, &height, &UA_TYPES[UA_TYPES_UINT16]);
    UA_NodeId heightNodeId = UA_NODEID_STRING(1, "height");
    heightAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Height (cm)");
	heightAttr.accessLevel |= UA_ACCESSLEVELMASK_WRITE; // default readonly, add write privilege
    UA_Server_addVariableNode(server, heightNodeId, studentId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              UA_QUALIFIEDNAME(1, "Height (cm)"),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), heightAttr, NULL, NULL);
    
    // add weight                          
    UA_VariableAttributes weightAttr = UA_VariableAttributes_default;
    UA_Float weight = 60.12f;
    UA_Variant_setScalar(&weightAttr.value, &weight, &UA_TYPES[UA_TYPES_FLOAT]);
    UA_NodeId weightNodeId = UA_NODEID_STRING(1, "weight");
    weightAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Weight (kg)");
	weightAttr.accessLevel |= UA_ACCESSLEVELMASK_WRITE; // default readonly, add write privilege	
    UA_Server_addVariableNode(server, weightNodeId, studentId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              UA_QUALIFIEDNAME(1, "Weight (kg)"),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), weightAttr, NULL, NULL);
}



/**
 * Now we change the value with the write service. This uses the same service
 * implementation that can also be reached over the network by an OPC UA client.
 */

static void
writeValue(UA_Server *server, string nameStr, int value) {
    char *name = const_cast<char*>(nameStr.c_str());
    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, name);

    /* Write a different integer value */
    UA_Int32 myInteger = value;
    UA_Variant myVar;
    UA_Variant_init(&myVar);
    UA_Variant_setScalar(&myVar, &myInteger, &UA_TYPES[UA_TYPES_INT32]);
    UA_Server_writeValue(server, myIntegerNodeId, myVar);
}

static void
writeValue(UA_Server *server, string nameStr, float value) {
    char *name = const_cast<char*>(nameStr.c_str());
    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, name);

    /* Write a different integer value */
    UA_Float myFloat = value;
    UA_Variant myVar;
    UA_Variant_init(&myVar);
    UA_Variant_setScalar(&myVar, &myFloat, &UA_TYPES[UA_TYPES_FLOAT]);
    UA_Server_writeValue(server, myIntegerNodeId, myVar);
}


//write call back
static void clientWriteCallback(UA_Server *server,
                                                 const UA_NodeId *sessionId, void *sessionContext,
                                                 const UA_NodeId *nodeId, void *nodeContext,
                                                 const UA_NumericRange *range, const UA_DataValue *data)
{
    //if value update by self pass. 
    //writeValue will cause this function be called
    if(sessionId->identifier.numeric == 1)
        return;

    string nameStr = string((char *)nodeId->identifier.string.data, nodeId->identifier.string.length);
    if(! data->hasValue)
        return;

	//we just process UINT16 and FLOAT 
    if(data->value.type->typeKind == UA_DATATYPEKIND_UINT16)
    {
        UA_UInt16 currentValue = *(UA_UInt16 *)(data->value.data);
        printf("opcua client write node:%s , %d\n", nameStr.c_str(), currentValue);
    }
    else if(data->value.type->typeKind == UA_DATATYPEKIND_FLOAT)
    {
        UA_Float currentValue = *(UA_Float *)(data->value.data);
        printf("opcua client write node:%s , %f\n", nameStr.c_str(), currentValue);
    }
}

// callback before client read
static void beforeReadCallback(UA_Server *server,
			const UA_NodeId *sessionId, void *sessionContext,
			const UA_NodeId *nodeid, void *nodeContext,
			const UA_NumericRange *range, const UA_DataValue *data) 
{
    if(sessionId->identifier.numeric == 1)
        return;

	//struct timespec srecvTime;
	//clock_gettime(CLOCKID, &srecvTime);//64 bits
        //UA_Int64 rtime64 = (UA_Int64)((srecvTime.tv_sec * SECONDS) + srecvTime.tv_nsec);
	//printf("srecvtime: %ld.%ld \n", srecvTime.tv_sec, srecvTime.tv_nsec);//everyone 64 bits

	//UA_Variant value;
	//UA_Variant_setScalar(&value, &rtime64, &UA_TYPES[UA_TYPES_INT64]);	
	//UA_Server_writeValue(server, *nodeid, value);
}

//this function will be called, when the value is modified by opcua client or server
 static void
 addCallback(UA_Server *server, string nameStr)
 {
         char *name = const_cast<char*>(nameStr.c_str());
         UA_NodeId currentNodeId = UA_NODEID_STRING(1, name);
         UA_ValueCallback callback;
         callback.onRead = beforeReadCallback;//nullptr
         callback.onWrite = clientWriteCallback;
         UA_Server_setVariableNode_valueCallback(server, currentNodeId, callback);
 }






//your thread to process data and modify the value of opcua server
//we select age to write.
void fun(UA_Server* server)
{
	srand(time(0));
   
	while(1)
	{
        uint16_t value = rand()%100;
        string ioStr = "age";
        writeValue(server, ioStr, value);
        
        printf("write %s, value:%d\n",ioStr.c_str(), value);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}
}






int main(void) 
{
    UA_Server *server = UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(server));
    
    manuallyDefineStudent(server);
    addCallback(server, "age");
    addCallback(server, "gender");
    addCallback(server, "height");
    addCallback(server, "weight");


	std::thread td(fun, server);
	td.detach();
	
    UA_StatusCode retval = UA_Server_run(server, &running);
    UA_Server_delete(server);
    
    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}


