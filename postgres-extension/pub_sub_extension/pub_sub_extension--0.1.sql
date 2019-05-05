-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pub_sub_extension" to load this file. \quit
-- CREATE TYPE dblink_pkey_results AS (position int, colname text);
CREATE TYPE subscribe_results AS (topic text, payload_timestamp text, payload text);

-- CREATE FUNCTION subscribe (text)
-- RETURNS setof dblink_pkey_results
-- AS '$libdir/pub_sub_extension'
-- LANGUAGE C STRICT ;

CREATE FUNCTION connect_stream(text) RETURNS text
AS '$libdir/pub_sub_extension'
LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION publish(text,text,text) RETURNS text
AS '$libdir/pub_sub_extension'
LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION subscribe(text,text,text) 
RETURNS setof subscribe_results
AS '$libdir/pub_sub_extension'
LANGUAGE C IMMUTABLE STRICT;