CC=gcc
CFLAGS= -I./include -g
LDFLAGS=

SRCDIR=./src
SOURCES=$(wildcard $(SRCDIR)/*.c)
OBJECTS=$(SOURCES:.c=.o)

EXECUTABLE=appli_projet

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) main.o
	$(CC) $(LDFLAGS) $(OBJECTS) main.o -o $@ -lm

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -lm 

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@ -lm

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)