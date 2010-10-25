# Override default implicit rules

SRCDIR := src
OBJDIR := build/obj
DEPDIR := build/dep

OBJECTS := $(patsubst src/%.c,$(OBJDIR)/%.o,$(SOURCES))
DEPENDS := $(patsubst src/%.c,$(DEPDIR)/%.d,$(SOURCES))

vpath %.c src
#vpath %.o $(OBJDIR)
#vpath %.d $(DEPDIR)

GENERATED := $(TARGET) 

.PHONY: all clean

all: $(TARGET)

clean:
	@echo -e "\tRM\t$(GENERATED)"
	$(A)rm -f $(GENERATED) 
	@echo -e "\tRM\tbuild"
	$(A)rm -rf build 

$(OBJDIR)/%.o: %.c
	@echo -e "\tCC\t$<"
	$(A)$(CC) $(CFLAGS) -o $@ -c $<

$(DEPDIR)/%.d: %.c
	@echo -e "\tDEP\t$<"
	$(A)$(CC) -o $@ -MT "$(OBJDIR)/$*.o" -MM $< 

$(TARGET): $(OBJECTS)
	@echo -e "\tLD\t$^"
	$(A)$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(OBJECTS): | $(OBJDIR)

$(DEPENDS): | $(DEPDIR)

$(OBJDIR):
	@mkdir -p $@

$(DEPDIR):
	@mkdir -p $@

-include $(DEPENDS)
