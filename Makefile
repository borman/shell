# Target description
SOURCES = $(wildcard src/*.c)
TARGET = shell

USE_COLORS ?= 1
USE_GNU_READLINE ?= 1

# Default config
include makefiles/config.mk

ifeq ($(USE_GNU_READLINE), 1)
	CFLAGS += -DUSE_GNU_READLINE
	LDFLAGS += -lreadline
endif

ifeq ($(USE_COLORS), 1)
	CFLAGS += -DUSE_TERMINAL_COLORS
endif

# Default rules
include makefiles/rules.mk
