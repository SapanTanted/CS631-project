/* Processed by ecpg (4.11.0) */
/* These include files are added by the preprocessor */
#include <ecpglib.h>
#include <ecpgerrno.h>
#include <sqlca.h>
/* End of automatic include section */

#line 1 "ecpg_test.pgc"
#include<stdio.h>

#line 1 "/usr/include/postgresql/sqlca.h"
#ifndef POSTGRES_SQLCA_H
#define POSTGRES_SQLCA_H

#ifndef PGDLLIMPORT
#if  defined(WIN32) || defined(__CYGWIN__)
#define PGDLLIMPORT __declspec (dllimport)
#else
#define PGDLLIMPORT
#endif   /* __CYGWIN__ */
#endif   /* PGDLLIMPORT */

#define SQLERRMC_LEN	150

#ifdef __cplusplus
extern		"C"
{
#endif

struct sqlca_t
{
	char		sqlcaid[8];
	long		sqlabc;
	long		sqlcode;
	struct
	{
		int			sqlerrml;
		char		sqlerrmc[SQLERRMC_LEN];
	}			sqlerrm;
	char		sqlerrp[8];
	long		sqlerrd[6];
	/* Element 0: empty						*/
	/* 1: OID of processed tuple if applicable			*/
	/* 2: number of rows processed				*/
	/* after an INSERT, UPDATE or				*/
	/* DELETE statement					*/
	/* 3: empty						*/
	/* 4: empty						*/
	/* 5: empty						*/
	char		sqlwarn[8];
	/* Element 0: set to 'W' if at least one other is 'W'	*/
	/* 1: if 'W' at least one character string		*/
	/* value was truncated when it was			*/
	/* stored into a host variable.             */

	/*
	 * 2: if 'W' a (hopefully) non-fatal notice occurred
	 */	/* 3: empty */
	/* 4: empty						*/
	/* 5: empty						*/
	/* 6: empty						*/
	/* 7: empty						*/

	char		sqlstate[5];
};

struct sqlca_t *ECPGget_sqlca(void);

#ifndef POSTGRES_ECPG_INTERNAL
#define sqlca (*ECPGget_sqlca())
#endif

#ifdef __cplusplus
}
#endif

#endif

#line 2 "ecpg_test.pgc"


/* exec sql begin declare section */
     

#line 5 "ecpg_test.pgc"
 char dbname [ 1024 ] ;
/* exec sql end declare section */
#line 6 "ecpg_test.pgc"


int main (){
    printf("Hello world!\n");
    { ECPGconnect(__LINE__, 0, "postgres@localhost" , "postgres" , NULL , "conn", 0); }
#line 10 "ecpg_test.pgc"

 { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "create table foo ( number integer , ascii char ( 16 ) )", ECPGt_EOIT, ECPGt_EORT);}
#line 11 "ecpg_test.pgc"

{ ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "create unique index num1 on foo ( number )", ECPGt_EOIT, ECPGt_EORT);}
#line 12 "ecpg_test.pgc"

{ ECPGtrans(__LINE__, NULL, "commit");}
#line 13 "ecpg_test.pgc"

}