include config.mk

.PHONY: install uninstall clean

# Set the object file names, with the source directory stripped
# from the path, and the build path prepended in its place
OBJECTS = $(SOURCES:%.c=$(OBJECT_DIR)/%.o)

all: $(EXECUTABLE)

install: $(EXECUTABLE)
	install -D -m 755 $(EXECUTABLE) $(INSTALL_DIR)/bin/$(EXECUTABLE)

uninstall:
	$(RM) $(INSTALL_DIR)/bin/$(EXECUTABLE)

clean:
	rm $(OBJECTS)
	rmdir $(OBJECT_DIR)
	rm $(EXECUTABLE)


$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(OBJECT_DIR)/%.o: %.c
	@mkdir -p $(OBJECT_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
