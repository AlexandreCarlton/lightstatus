include config.mk

.PHONY: install uninstall clean

# Set the object file names, with the source directory stripped
# from the path, and the build path prepended in its place
OBJECTS = $(SOURCES:%.c=$(OBJECT_DIR)/%.o)

all: $(EXECUTABLE)

install: $(EXECUTABLE)
	install -m 644 $(EXECUTABLE) $(INSTALL_DIR)/$(EXECUTABLE)

uninstall:
	$(RM) $(INSTALL_PREFIX)/$(EXECUTABLE)

#clean:
#	rm $(OBJECTS)
#	rmdir $(OBJECT_DIR)
#	rm $(EXECUTABLE)


# Link the executable
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@


# Source file rules
# After the first compilation they will be joined with the rules from the
# dependency files to provide header dependencies
$(OBJECT_DIR)/%.o: %.c
	@mkdir -p $(OBJECT_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
