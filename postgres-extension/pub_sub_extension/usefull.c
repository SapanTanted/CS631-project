#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "executor/spi.h"
#include <string.h>
#include <time.h>

#include "access/htup_details.h"
// #include "access/relation.h"
#include "access/reloptions.h"
// #include "access/table.h"
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
#include "utils/syscache.h"
#include "utils/typcache.h"
// #include "utils/varlena.h"
// char *query = text_to_cstring(PG_GETARG_TEXT_PP(0));
//     elog(INFO,"%s",query);
//     // char* result=palloc(50+sizeof(query));
//     // sprintf(result, "new client id: %s", query);
//     execq(cstring_to_text(query),0);
PG_MODULE_MAGIC;

int execq(text *sql, int cnt);

PG_FUNCTION_INFO_V1(connect_stream);
Datum
connect_stream(PG_FUNCTION_ARGS)
{
    char* client_id = text_to_cstring(PG_GETARG_TEXT_PP(0));
    //  elog(INFO,"clientID: %s",client_id);
     SPI_connect();
    char * query=palloc(1000*sizeof(char));
    sprintf(query,"insert into client_table(ts,client_id) values(CURRENT_TIMESTAMP,'%s')",client_id);
    // elog(INFO,"Query: %s",query);
    int inserted_rows = SPI_exec(text_to_cstring(cstring_to_text(query)), 0);
    elog(INFO,"Number of inserted rows: %d",inserted_rows);
    SPI_finish();
    for(int i=0;i<10;i++)
    PG_RETURN_TEXT_P(PG_GETARG_TEXT_PP(0));
    while(true);
}

PG_FUNCTION_INFO_V1(publish);
Datum
publish(PG_FUNCTION_ARGS)
{
    char *client_id = text_to_cstring(PG_GETARG_TEXT_PP(0));
    char* result=palloc(50+sizeof(client_id));
    sprintf(result, "client id: %s", client_id);
    PG_RETURN_TEXT_P(cstring_to_text(result));
}

PG_FUNCTION_INFO_V1(subscribe);
Datum
subscribe(PG_FUNCTION_ARGS)
{
    // char *client_id = text_to_cstring(PG_GETARG_TEXT_PP(0));
    // char* result=palloc(50+sizeof(client_id));
    // sprintf(result, "client id: %s", client_id);
    // PG_RETURN_TEXT_P(cstring_to_text(result));

	FuncCallContext *funcctx;
	int32		call_cntr;
	int32		max_calls;
	AttInMetadata *attinmeta;
	MemoryContext oldcontext;

	/* stuff done only on the first call of the function */
	if (SRF_IS_FIRSTCALL())
	{
		Relation	rel;
		TupleDesc	tupdesc;

		/* create a function context for cross-call persistence */
		funcctx = SRF_FIRSTCALL_INIT();

		/*
		 * switch to memory context appropriate for multiple function calls
		 */
		oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

		/*
		 * need a tuple descriptor representing one INT and one TEXT column
		 */
		tupdesc = CreateTemplateTupleDesc(2,false);
		TupleDescInitEntry(tupdesc, (AttrNumber) 1, "position",
						   INT4OID, -1, 0);
		TupleDescInitEntry(tupdesc, (AttrNumber) 2, "colname",
						   TEXTOID, -1, 0);

		/*
		 * Generate attribute metadata needed later to produce tuples from raw
		 * C strings
		 */
		attinmeta = TupleDescGetAttInMetadata(tupdesc);
		funcctx->attinmeta = attinmeta;
		MemoryContextSwitchTo(oldcontext);
	}

	/* stuff done on every call of the function */
	funcctx = SRF_PERCALL_SETUP();

	/*
	 * initialize per-call variables
	 */
	call_cntr = funcctx->call_cntr;
	max_calls = 10;

	// results = (char **) funcctx->user_fctx;
	attinmeta = funcctx->attinmeta;

	if (call_cntr < max_calls)	/* do when there is more left to send */
	{
		char	  **values;
		HeapTuple	tuple;
		Datum		result;

		values = (char **) palloc(2 * sizeof(char *));
		values[0] = psprintf("%d", call_cntr + 1);
		values[1] = "Result 123";

		/* build the tuple */
		tuple = BuildTupleFromCStrings(attinmeta, values);

		/* make the tuple into a datum */
		result = HeapTupleGetDatum(tuple);

		SRF_RETURN_NEXT(funcctx, result);
	}
	else
	{
		pg_usleep(5*1000000L);
		SRF_RETURN_DONE(funcctx);
	}

}



int execq(text *sql, int cnt)
{
    char *command;
    int ret;
    int proc;

    /* Convert given text object to a C string */
    command = text_to_cstring(sql);
    // command =(sql);

    SPI_connect();

    ret = SPI_exec(command, cnt);

    proc = SPI_processed;
    /*
     * If some rows were fetched, print them via elog(INFO).
     */
    if (ret > 0 && SPI_tuptable != NULL)
    {
        TupleDesc tupdesc = SPI_tuptable->tupdesc;
        SPITupleTable *tuptable = SPI_tuptable;
        char buf[8192];
        int i, j;

        for (j = 0; j < proc; j++)
        {
            HeapTuple tuple = tuptable->vals[j];

            for (i = 1, buf[0] = 0; i <= tupdesc->natts; i++)
                snprintf(buf + strlen (buf), sizeof(buf) - strlen(buf), " %s%s",
                        SPI_getvalue(tuple, tupdesc, i),
                        (i == tupdesc->natts) ? " " : " |");
            elog(INFO, "EXECQ: %s", buf);
        }
    }

    SPI_finish();
    pfree(command);

    return (proc);
}