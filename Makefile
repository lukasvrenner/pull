TURTLS = ../turtls

CFLAGS = -Wall -Wextra -Wpedantic -I$(TURTLS)/include
CFLAGS_DEBUG = $(CFLAGS) -g -O0
CFLAGS_RELEASE = $(CFLAGS) -O3

LDFLAGS = -lturtls
LDFLAGS_DEBUG = $(LDFLAGS) -L$(TURTLS)/target/debug -Wl,-rpath=$(TURTLS)/target/debug
LDFLAGS_RELEASE = $(LDFLAGS) -L$(TURTLS)/target/release -Wl,-rpath=$(TURTLS)/target/release

SRC_FILES = src/main.c

OBJ_FILES_DEBUG = $(SRC_FILES:%.c=build/%.o)
DEP_FILES_DEBUG = $(OBJ_FILES_DEBUG:.o=.d)

OBJ_FILES_RELEASE = $(SRC_FILES:%.c=buildrel/%.o)
DEP_FILES_RELEASE = $(OBJ_FILES_RELEASE:.o=.d)

EXEC_NAME = pull

all: debug

################
# DEBUG MODE #
################
debug: build/$(EXEC_NAME)

build/$(EXEC_NAME): $(OBJ_FILES_DEBUG)
	cc $(OBJ_FILES_DEBUG) $(LDFLAGS_DEBUG) -o build/$(EXEC_NAME)

build/src/%.o: src/%.c | build/src
	cc $(CFLAGS_DEBUG) -MMD -MP -c $< -o $@

build/src:
	mkdir -p $@

-include $(DEP_FILES_DEBUG)

################
# RELEASE MODE #
################
release: buildrel/$(EXEC_NAME)

buildrel/$(EXEC_NAME): $(OBJ_FILES_RELEASE)
	cc $(OBJ_FILES_RELEASE) $(LDFLAGS_RELEASE) -o buildrel/$(EXEC_NAME)

buildrel/src/%.o: src/%.c | buildrel/src
	cc $(CFLAGS_RELEASE) -MMD -MP -c $< -o $@

buildrel/src:
	mkdir -p $@

-include $(DEP_FILES_RELEASE)

clean:
	rm -rf build/
	rm -rf buildrel/

.PHONY: all debug release clean
