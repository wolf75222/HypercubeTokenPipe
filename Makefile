CC=gcc
CFLAGS=-Wall
LDFLAGS=
SOURCES=HypercubeProcessSystem.c ProcessCommunication.c SignalHandlers.c Utilities.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=hypercube

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(EXECUTABLE)

test: $(EXECUTABLE)
	./$(EXECUTABLE) 3
