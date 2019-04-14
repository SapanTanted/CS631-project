-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pub_sub_extension" to load this file. \quit
CREATE FUNCTION connect_stream(text) RETURNS text
AS '$libdir/pub_sub_extension'
LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION publish(text,text) RETURNS text
AS '$libdir/pub_sub_extension'
LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION subscribe(text) RETURNS text
AS '$libdir/pub_sub_extension'
LANGUAGE C IMMUTABLE STRICT;