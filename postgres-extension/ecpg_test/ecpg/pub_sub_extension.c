/* Processed by ecpg (4.11.0) */
/* These include files are added by the preprocessor */
#include <ecpglib.h>
#include <ecpgerrno.h>
#include <sqlca.h>
/* End of automatic include section */

#line 1 "pub_sub_extension.pgc"
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
    { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "create table new_table ( name varchar ( 100 ) )", ECPGt_EOIT, ECPGt_EORT);}
#line 14 "pub_sub_extension.pgc"

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