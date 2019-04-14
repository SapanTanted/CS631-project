EXTENSION = extension1        # the extensions name
DATA = extension1--0.1.sql  # script files to install
# REGRESS = base36_test     # our test script file (without extension)
MODULES = extension1          # our c module file to build

# postgres build stuff
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)