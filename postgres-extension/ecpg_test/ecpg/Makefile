

INCLUDES = -I$(shell pg_config --includedir)
LIBPATH = -L $(shell pg_config --libdir)
CFLAGS += $(INCLUDES) -g
LDFLAGS += -g
LDLIBS += $(LIBPATH) -lecpg -lpq
.SUFFIXES: .pgc,.pc
pub_sub_extension.c:pub_sub_extension.pgc
	ecpg -c $(INCLUDES) $?
