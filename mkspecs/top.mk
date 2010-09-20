# Override default implicit rules

%.o: %.c
	$(A)echo -e "\tCC\t$^"
	$(A)$(CC) $(CFLAGS) -o $@ -c $^

$(TARGET): $(OBJECTS)
	$(A)echo -e "\tLD\t$^"
	$(A)$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
