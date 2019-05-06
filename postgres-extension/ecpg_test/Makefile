

INCLUDES = -I$(shell pg_config --includedir)
LIBPATH = -L$(shell pg_config --libdir)
CFLAGS += $(INCLUDES) 
LDFLAGS += -g
LDLIBS += $(LIBPATH) -lecpg_compat -lpq

# ecpg_test.so:ecpg_test.o
	# gcc $(CFLAGS) -o ecpg_test.so ecpg_test.o libecpg.so libpq.so

ecpg_test.o:ecpg_test.c 
	gcc  $(CFLAGS) -g $(LDLIBS) -Wall -g -o ecpg_test.o ecpg_test.c libecpg.so libpq.so

ecpg_test.c:ecpg_test.pgc
	ecpg -c $(INCLUDES) $?

clean:
	rm ecpg_test.c ecpg_test.o ecpg_test.so