EXTENSION = pub_sub_extension        # the extensions name
DATA = pub_sub_extension--0.1.sql  # script files to install
# REGRESS = base36_test     # our test script file (without extension)
MODULES = pub_sub_extension          # our c module file to build

# postgres build stuff
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
