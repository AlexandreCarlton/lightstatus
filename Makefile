include config.mk

.PHONY: install uninstall clean

# Set the object file names, with the source directory stripped
# from the path, and the build path prepended in its place
OBJECTS = $(SOURCES:%.c=%.o)

all: $(EXECUTABLE)

include dependencies.mk

install: $(EXECUTABLE)
	install -D -m 755 $(EXECUTABLE) $(INSTALL_DIR)/bin/$(EXECUTABLE)

uninstall:
	$(RM) $(INSTALL_DIR)/bin/$(EXECUTABLE)

clean:
	rm $(OBJECTS)
	rm $(EXECUTABLE)


$(EXECUTABLE): $(OBJECTS)
