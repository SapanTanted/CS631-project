#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(extension1_test);
Datum
extension1_test(PG_FUNCTION_ARGS)
{
    int32 arg = PG_GETARG_INT32(0);
    char* result=palloc(50*sizeof(char));
    sprintf(result, "%d input is given", arg);
    PG_RETURN_TEXT_P(cstring_to_text(result));
}