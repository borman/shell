SOURCES = $(wildcard *.c)
OBJECTS = $(subst .c,.o,$(SOURCES))

TARGET = shell

# Link sources to headers
%.c: %.h

include ../mkspecs/config.mk ../mkspecs/top.mk
