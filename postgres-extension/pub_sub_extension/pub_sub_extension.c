#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(connect_stream);
Datum
connect_stream(PG_FUNCTION_ARGS)
{
    char *client_id = text_to_cstring(PG_GETARG_TEXT_PP(0));
    char* result=palloc(50+sizeof(client_id));
    sprintf(result, "new client id: %s", client_id);
    PG_RETURN_TEXT_P(cstring_to_text(result));
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
    char *client_id = text_to_cstring(PG_GETARG_TEXT_PP(0));
    char* result=palloc(50+sizeof(client_id));
    sprintf(result, "client id: %s", client_id);
    PG_RETURN_TEXT_P(cstring_to_text(result));
}