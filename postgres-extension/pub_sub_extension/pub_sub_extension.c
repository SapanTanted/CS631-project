#include <stdio.h>
#include <stdlib.h>
#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "executor/spi.h"
#include <string.h>
#include <time.h>
#include "access/htup_details.h"
#include "access/xact.h"
#include "access/reloptions.h"
#include "catalog/indexing.h"
#include "catalog/namespace.h"
#include "catalog/pg_foreign_data_wrapper.h"
#include "catalog/pg_foreign_server.h"
#include "catalog/pg_type.h"
#include "catalog/pg_user_mapping.h"
#include "executor/spi.h"
#include "foreign/foreign.h"
#include "funcapi.h"
#include "lib/stringinfo.h"
#include "mb/pg_wchar.h"
#include "miscadmin.h"
#include "parser/scansup.h"
#include "utils/acl.h"
#include "utils/builtins.h"
#include "utils/fmgroids.h"
#include "utils/guc.h"
#include "utils/lsyscache.h"
#include "utils/memutils.h"
#include "utils/rel.h"
#include "utils/timestamp.h"
#include "utils/syscache.h"
#include "utils/typcache.h"

#include "storage/ipc.h"
#include "storage/pg_sema.h"
#include "storage/shmem.h"

#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <unistd.h>

#include "miscadmin.h"
PG_MODULE_MAGIC;

void logInfo(char* key,char* value);
bool is_client_id_registered(char * client_id);
bool get_topic_id_and_relative_timeout(char* topic, int* topic_id, int64* relative_timeout); 
bool insert_payload_entry(char* client_id, int topic_id, char* payload, pg_time_t payload_timestamp, pg_time_t expiry_timestamp);

static int maxSems = 1000;
static sem_t * mySemPointers[1000];//= (sem_t **) palloc(1000* sizeof(sem_t *));;	/* keep track of created semaphores */
static int number_of_subscriptions[1000];
	
PG_FUNCTION_INFO_V1(connect_stream);
Datum
connect_stream(PG_FUNCTION_ARGS)
{
	char* client_id = text_to_cstring(PG_GETARG_TEXT_PP(0));
	logInfo("client_id",client_id);
	logInfo("Registering","client_id");

	SPI_connect();
	char* t_query = "insert into client_table(connection_timestamp,client_id,number_of_subscriptions) values(CURRENT_TIMESTAMP,'%s',0)"; // temporary query 
	char* query=palloc(sizeof(char)*(strlen(t_query)+strlen(client_id)));
	sprintf(query,t_query,client_id);
	logInfo("[connect_stream]Executing query",query);
	PG_TRY();
	{
		SPI_execute(text_to_cstring(cstring_to_text(query)),false, 0);
		SPI_finish();
	    PG_RETURN_TEXT_P(cstring_to_text("success"));
	}
	PG_CATCH();
	{ //TODO catch exceptions for duplicate key and send proper error message
		logInfo("Exception in query",query);
	}
	PG_END_TRY();
	SPI_finish();
    PG_RETURN_TEXT_P(cstring_to_text("failed"));
}

PG_FUNCTION_INFO_V1(publish);
Datum
publish(PG_FUNCTION_ARGS)
{
    char *client_id = text_to_cstring(PG_GETARG_TEXT_PP(0));
    char *topic = text_to_cstring(PG_GETARG_TEXT_PP(1));
    char *payload = text_to_cstring(PG_GETARG_TEXT_PP(2));
	//start transaction
	//If topic does not exist ignore the payload because no one subscribed to that topic
	//add a payload entry in payload_table
	//end transaction

	SPI_connect();
	BeginTransactionBlock();
	CommitTransactionCommand();
	//client_id must be there in client_table
	if(!is_client_id_registered(client_id)){
		SPI_finish();
		PG_RETURN_TEXT_P(cstring_to_text("client_id not registered!"));	
	}
	int64 *relative_timeout=(int64*) palloc(sizeof(int64));
	int *topic_id=(int*) palloc(sizeof(int));
	if(!get_topic_id_and_relative_timeout(topic,topic_id,relative_timeout)){
		SPI_finish();
		PG_RETURN_TEXT_P(cstring_to_text("topic does not exist i.e. topic not subscribed by anyone! ignoring the payload"));	
	}
	pg_time_t  payload_timestamp = timestamptz_to_time_t(GetCurrentTimestamp());
	pg_time_t expiry_timestamp = payload_timestamp+(*relative_timeout);
	if(insert_payload_entry(client_id,*topic_id,payload,payload_timestamp,expiry_timestamp)){
		//alert all subscribers
		logInfo("Alert","All subscribers");
		sem_t *semaphore = mySemPointers[*topic_id];
		for(int i=0;i<number_of_subscriptions[*topic_id];i++){
			PGSemaphoreUnlock(semaphore);
		}
	}else{
		SPI_finish();
		PG_RETURN_TEXT_P(cstring_to_text("Could not insert payload entry!"));	
	}
	EndTransactionBlock(false);
	SPI_finish();
    PG_RETURN_TEXT_P(cstring_to_text("Published!!"));
}
int abc = 1;
//USE_NAMED_POSIX_SEMAPHORES is not defined

PG_FUNCTION_INFO_V1(subscribe);
Datum
subscribe(PG_FUNCTION_ARGS)
{
	elog(INFO,"subscribe : %d",abc);
	char *client_id = text_to_cstring(PG_GETARG_TEXT_PP(0));
    char *topic = text_to_cstring(PG_GETARG_TEXT_PP(1));
    int timeout = atoi(text_to_cstring(PG_GETARG_TEXT_PP(2)));
	//check if subscription entry exists if not then add
	//check if timeout happened or not
	//
	SPI_connect();
	BeginTransactionBlock();
	CommitTransactionCommand();
	//client_id must be there in client_table
	if(!is_client_id_registered(client_id)){
		SPI_finish();
		PG_RETURN_TEXT_P(cstring_to_text("client_id not registered!"));	
	}
	PGReserveSemaphores(1000,2000);
	logInfo("Initializing","Semaphore");
	mySemPointers[1]= PGSemaphoreCreate();
	logInfo("Reseting","Semaphore");
	PGSemaphoreReset(mySemPointers[1]);
	logInfo("Locking","Semaphore");
	number_of_subscriptions[1]++;
	PGSemaphoreLock(mySemPointers[1]);
	logInfo("After","Semaphore");

	//check if subscription entry exists if not then add

	//if subscription entry exists then
	//set is_connected = true;
	//check if last_ping_timestamp+timeout is less than current_timestamp i.e. timed out already
	//if timed out then set last_ping_timestamp to current_timestamp and wait on semaphore
	//if not timed out then fetch all payloads from last_ping_timestamp till current_timestamp and set last_ping_timestamp to current_timestamp
	//if any payload found return it else wait on semaphore

	//if subscription entry does not exist then
	//add subscription entry and wait on semaphore
	//set is_connected = true

	//add entry in topic table
	//initialize mySemPointers for that topic_id
	//increment number_of_subscription for that topic_id
	EndTransactionBlock(true);

	//set is_connected = false;
	EndTransactionBlock(false);
	SPI_finish();
}

bool insert_payload_entry(char* client_id, int topic_id, char* payload, pg_time_t payload_timestamp, pg_time_t expiry_timestamp){
	StartTransactionCommand();
	bool payload_added = false;
	//insert payload with above details
	//update topic table with payload_timestamp = last_msg_recv_timestamp
	char* t_query = "insert into payload_table (client_id,topic_id,payload,payload_timestamp,expiry_timestamp) values('%s','%d','%s',to_timestamp('%ld'),to_timestamp('%ld')) "; // temporary query variable
	char* query=palloc(sizeof(char)*(strlen(t_query)+strlen(client_id)+strlen(payload))+sizeof(topic_id)+sizeof(payload_timestamp)+sizeof(expiry_timestamp));
	sprintf(query,t_query,client_id,topic_id,payload,payload_timestamp,expiry_timestamp);
	logInfo("[insert_payload_entry]Executing Query",query);
	PG_TRY();
	{
		if(SPI_execute(text_to_cstring(cstring_to_text(query)),false, 0)==SPI_OK_INSERT){
			//Update topic table
			char* t_query = "update topic_table set last_msg_recv_timestamp=to_timestamp('%ld') where topic_id='%d'"; // temporary query variable
			char* query=palloc(sizeof(char)*(strlen(t_query))+sizeof(topic_id)+sizeof(payload_timestamp));
			sprintf(query,t_query,payload_timestamp,topic_id);
			PG_TRY();
			{
				if(SPI_execute(text_to_cstring(cstring_to_text(query)),false, 0)==SPI_OK_UPDATE){	
					payload_added=true;
				}
			}
			PG_CATCH();
			{ //TODO catch exceptions
				logInfo("Exception in query",query);
			}
			PG_END_TRY();
		}
	}
	PG_CATCH();
	{ //TODO catch exceptions
		logInfo("Exception in query",query);
	}
	PG_END_TRY();
	if(!payload_added){
		logInfo("Payload could not be added!","Aborting transaction!");
		AbortCurrentTransaction();
	}
	CommitTransactionCommand();
	return payload_added;
}

bool get_topic_id_and_relative_timeout(char* topic, int *topic_id, int64 *relative_timeout){
	StartTransactionCommand();
	bool topic_found = false;
	//insert ignore topic
	// char* t_query = "insert ignore into topic_table (topic_id,topic,last_msg_recv_timestamp,relative_timeout) values(NULL,'%s',CURRENT_TIMESTAMP,0) "; // temporary query variable
	// char* query=palloc(sizeof(char)*(strlen(t_query)+strlen(topic)));
	// sprintf(query,t_query,topic);
	// logInfo("[get_topic_id_and_relative_timeout]Executing Query",query);

	//fetch topic_id and relative_timeout of topic
	char* t_query = "select topic_id,relative_timeout from topic_table where topic='%s'"; // temporary query variable
	char* query=palloc(sizeof(char)*(strlen(t_query)+strlen(topic)));
	sprintf(query,t_query,topic);
	PG_TRY();
	{
		// if(SPI_execute(text_to_cstring(cstring_to_text(query)),false, 0)==SPI_OK_INSERT){

			if(SPI_execute(text_to_cstring(cstring_to_text(query)),true, 0)==SPI_OK_SELECT){
				SPITupleTable *spi_tuptable = SPI_tuptable;
				TupleDesc	spi_tupdesc = spi_tuptable->tupdesc;
				int64 proc = SPI_processed;
				for (int64 i = 0; i < proc; i++)
				{
					/* get the next sql result tuple */
					HeapTuple spi_tuple = spi_tuptable->vals[i];
					*topic_id = atoi(SPI_getvalue(spi_tuple, spi_tupdesc, SPI_fnumber(spi_tupdesc,"topic_id")));
					*relative_timeout = atoi(SPI_getvalue(spi_tuple, spi_tupdesc, SPI_fnumber(spi_tupdesc,"relative_timeout")));
					topic_found = true;
				}
			}
		// }
	}
	PG_CATCH();
	{ //TODO catch exceptions
		logInfo("Exception in query",query);
	}
	PG_END_TRY();
	CommitTransactionCommand();
	return topic_found;
}
bool is_client_id_registered(char * client_id){
	StartTransactionCommand();
	bool registered = false;
	char* t_query = "select count(*) as count from client_table where client_id='%s'"; // temporary query 
	char* query=palloc(sizeof(char)*(strlen(t_query)+strlen(client_id)));
	sprintf(query,t_query,client_id);
	logInfo("[is_client_id_registered]Executing query",query);
	PG_TRY();
	{
		if(SPI_execute(text_to_cstring(cstring_to_text(query)),true, 0)==SPI_OK_SELECT){
			
			SPITupleTable *spi_tuptable = SPI_tuptable;
			TupleDesc	spi_tupdesc = spi_tuptable->tupdesc;
			int64 proc = SPI_processed;
			for (int64 i = 0; i < proc; i++)
			{
				char	   *row_count;
				HeapTuple	spi_tuple;
				/* get the next sql result tuple */
				spi_tuple = spi_tuptable->vals[i];
				row_count = SPI_getvalue(spi_tuple, spi_tupdesc, 1);
				if(atoi(row_count)==0)registered=false;
				else registered=true;
				elog(INFO,"[is_client_id_registered][Row Count:%d]",atoi(row_count));
			}
		}else{
			registered= false;
		}
		//check if count is 0 or 1
		// registered = true; //TODO Delete this line otherwise all clients are registered
	}
	PG_CATCH();
	{ //TODO catch exceptions
		logInfo("Exception in query",query);
	}
	PG_END_TRY();
	CommitTransactionCommand();
	return registered;
}

void logInfo(char* key,char* value){
		time_t t;
    	time(&t);
		char * time_str = ctime(&t);
    	time_str[strlen(time_str)-1] = '\0';
		elog(INFO,"[%s][CS631][%s:%s]", time_str,key,value);
}
