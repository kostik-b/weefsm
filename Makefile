# Copyright Konstantin Bakanov 2018

.PHONY : clean uninstall

SOURCE_DIR    := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

all : libwfsm.a # basic_test

SRCS = wfsm.c basic_test.c

SRC_DIRS = src tests

# ----------------- macro for dependencies -------------
define depend_macro

%.o.d: $(1)
	@set -e; rm -f $$@; \
	$$(CC) -MM $$(CXXFLAGS) $$(CXX_OPTS) $$< > $$@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $$@ : ,g' < $$@.$$$$ > $$@; \
	rm -f $$@.$$$$

endef

$(foreach DIR, $(SRC_DIRS), $(eval $(call depend_macro,   $(SOURCE_DIR)/$(DIR)/%.c)))
# ------- end dependencies ------------------------------

include $(SRCS:%.c=%.o.d)

# ---------------- macro for the build pattern rules ----------
define build_macro

%.o : $(1)
	$${CC} $$< -o $$@ -c $${CXXFLAGS} $${CXX_OPTS}

endef

$(foreach DIR, $(SRC_DIRS), $(eval $(call build_macro, $(SOURCE_DIR)/$(DIR)/%.c)))
#---------------- end build macro -----------------------------


libwfsm.a : wfsm.o
	ar rcfv $@ $^

#clean_agent : 
#	- rm $(GENERAL_FILES) $(GENERAL_FILES:%.o=%.o.d) indie

#clean : clean_agent

