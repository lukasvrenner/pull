TURTLS = ../turtls

CFLAGS = -std=c17 -Wall -Wextra -Wpedantic -Werror -I$(TURTLS)/include
CFLAGS_DEBUG = $(CFLAGS) -g -O0
CFLAGS_RELEASE = $(CFLAGS) -O3

LDFLAGS = -lturtls
LDFLAGS_DEBUG = $(LDFLAGS) -L$(TURTLS)/target/debug -Wl,-rpath=$(TURTLS)/target/debug
LDFLAGS_RELEASE = $(LDFLAGS) -L$(TURTLS)/target/release -Wl,-rpath=$(TURTLS)/target/release

SRC = src/main.c

OBJ_DEBUG = $(SRC:%.c=build/%.o)
DEP_DEBUG = $(OBJ_DEBUG:.o=.d)

OBJ_RELEASE = $(SRC:%.c=buildrel/%.o)
DEP_RELEASE = $(OBJ_RELEASE:.o=.d)

NAME = pull

all: debug

################
# DEBUG MODE #
################
debug: build/$(NAME)

build/$(NAME): $(OBJ_DEBUG)
	cc $(OBJ_DEBUG) $(LDFLAGS_DEBUG) -o build/$(NAME)

build/src/%.o: src/%.c | build/src
	cc $(CFLAGS_DEBUG) -MMD -MP -c $< -o $@

build/src:
	mkdir -p $@

-include $(DEP_DEBUG)

################
# RELEASE MODE #
################
release: buildrel/$(NAME)

buildrel/$(NAME): $(OBJ_RELEASE)
	cc $(OBJ_RELEASE) $(LDFLAGS_RELEASE) -o buildrel/$(NAME)

buildrel/src/%.o: src/%.c | buildrel/src
	cc $(CFLAGS_RELEASE) -MMD -MP -c $< -o $@

buildrel/src:
	mkdir -p $@

-include $(DEP_RELEASE)

clean:
	-rm -rf build/
	-rm -rf buildrel/

.PHONY: all debug release clean
