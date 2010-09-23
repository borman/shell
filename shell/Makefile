# Target description
SOURCES = $(wildcard *.c)
TARGET = shell
USE_GNU_READLINE = 1

# Default config
include ../mkspecs/config.mk

ifeq ($(USE_GNU_READLINE), 1)
	CFLAGS += -DUSE_GNU_READLINE
	LDFLAGS += -lreadline
endif

# Default rules
include ../mkspecs/rules.mk
