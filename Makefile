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

clean:
	rm -f build/*.o build/$(EXECUTABLE)

test: $(EXECUTABLE)

