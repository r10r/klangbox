LIBCX_DIR := $(BASE_DIR)/libcx
MODULES := .

TEST_RUNNER := $(SMAKE_DIR)/bin/mleaks-testrunner.sh

# FIXME 
# * do not run decover.sh / coverage script  when coverage is disabled
# * create coverage script for gcc

# set a compiler
compiler ?= clang
CC_GCC := gcc
CC_CLANG := clang

# TODO ignore parameter/functions/values/variables with a macro
# #define UNUSED(x) (void)(x)

CFLAGS += -Werror -Wall -pedantic -std=c99 -D_POSIX_C_SOURCE=200809L -D_C99_SOURCE -D_CX_ASSERT \
	-Wno-error=padded \
	-Wno-error=cast-align \
	-Wno-error=switch-enum
	
# [ profile specific CFLAGS ] 
# ===========================
# TODO split up into separate included files with
# arch/system/compiler specific settings

# default profile
profile ?= debug

# TODO Check whether autoconf is of use here ?
# TODO add profile test (with -D_CX_DEBUG_MEM)

ifeq ($(profile),release)
	CFLAGS += -Os
else ifeq ($(profile),debug)
	CFLAGS += -O0 -g -fno-inline --coverage -D_CX_DEBUG -D_CX_TRACE
	ifeq ($(OS),Darwin)
		CFLAGS += -gdwarf-2
		# when clang is installed through homebrew
		LDFLAGS += -L/usr/local/lib/llvm-3.4/usr/lib
	endif
	ifeq ($(OS),Linux)
		ifeq ($(ARCH),armv6l)
			ifeq ($(compiler),clang)
				# no coverage support for clang on armv6l
				CFLAGS := $(filter-out --coverage,$(CFLAGS))
			endif
		endif
	endif
endif

# sys/time.h TIMEVAL_TO_TIMESPEC
ifeq ($(OS),Darwin)
	CFLAGS += -D_DARWIN_C_SOURCE
else
	CFLAGS += -D_GNU_SOURCE
endif

TEST_OBJS := $(LIBCX_DIR)/base/unity.o

# ignore unity errors
UNITY_FLAGS += \
 	-Wno-unused-macros \
	-Wno-sign-conversion \
	-Wno-float-equal \
	-Wno-missing-field-initializers \
	-Wno-missing-braces \
	-Wno-unused-variable \
	-Wno-cast-align

$(LIBCX_DIR)/base/unity.o: CFLAGS += $(UNITY_FLAGS)