# Override default implicit rules

OBJECTS = $(subst .c,.o,$(SOURCES))
DEPENDS = $(subst .c,.d,$(SOURCES))

GENERATED = $(TARGET) $(OBJECTS) $(DEPENDS)

.PHONY: all clean

all: $(TARGET)

clean:
	@echo -e "\tRM\t$(GENERATED)"
	$(A)rm -f $(GENERATED) 

%.o: %.c
	@echo -e "\tCC\t$<"
	$(A)$(CC) $(CFLAGS) -o $@ -c $<

%.d: %.c
	@echo -e "\tDEP\t$<"
	$(A)$(CC) -o $@ -MM $< 

$(TARGET): $(OBJECTS)
	@echo -e "\tLD\t$^"
	$(A)$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

-include $(DEPENDS)
