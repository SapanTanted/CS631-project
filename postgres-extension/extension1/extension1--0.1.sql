-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION extension1" to load this file. \quit
CREATE FUNCTION extension1_test(integer) RETURNS text
AS '$libdir/extension1'
LANGUAGE C IMMUTABLE STRICT;