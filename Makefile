CC=gcc
CFLAGS=-Wall -Iinclude
LDFLAGS=
SOURCES=$(addprefix src/,HypercubeProcessSystem.c ProcessCommunication.c SignalHandlers.c Utilities.c)
OBJECTS=$(addprefix build/,$(notdir $(SOURCES:.c=.o)))
EXECUTABLE=build/hypercube

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

build/%.o: src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

clean: clf/clean_files
	./clf/clean_files
	rm -f build/*.o build/$(EXECUTABLE) clf/*.o $<

clf/clean_files.o: clf/clean_files.c
	$(CC) -c $(CFLAGS) $< -o $@
	
clf/clean_files: clf/clean_files.o build/Utilities.o
	$(CC) $(LDFLAGS) $^ -o $@

test: $(EXECUTABLE)

