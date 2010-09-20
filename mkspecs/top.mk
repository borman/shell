# Override default implicit rules

.PHONY: all clean

all: $(TARGET)

clean:
	@echo -e "\tRM\t$(TARGET) $(OBJECTS)"
	$(A)rm -f $(TARGET) $(OBJECTS)

%.o: %.c
	@echo -e "\tCC\t$^"
	$(A)$(CC) $(CFLAGS) -o $@ -c $^

$(TARGET): $(OBJECTS)
	@echo -e "\tLD\t$^"
	$(A)$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
