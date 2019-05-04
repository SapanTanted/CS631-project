INCLUDES = -I$(shell pg_config --includedir)
LIBPATH = -L$(shell pg_config --libdir) 
PG_CFLAGS += $(INCLUDES) -g 
PG_LDFLAGS += -g
LDFLAGS+= -g
LDFLAGS_SL+= $(LIBPATH) -lecpg -lpq
# PG_LIBS_INTERNAL += $(LIBPATH) -lecpg -lpq
# PG_LIBS += $(LIBPATH) -lecpg -lpq
# LDLIBS += $(LIBPATH) -lecpg -lpq
# LIBS += $(LIBPATH) -lecpg -lpq
# SHLIB_LINK+=$(LIBPATH) -lecpg -lpq
# SHLIB_LINK_INTERNAL+=$(LIBPATH) -lecpg -lpq
# CFLAGS_SL+=$(INCLUDES) -g

EXTENSION = pub_sub_extension        # the extensions name
DATA = pub_sub_extension--0.1.sql  # script files to install
MODULES = pub_sub_extension          # our c module file to build

# postgres build stuff
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
