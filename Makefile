CC=gcc
CFLAGS= -I./include -g
LDFLAGS=

SRCDIR=./src
SOURCES=$(wildcard $(SRCDIR)/*.c)
OBJECTS=$(SOURCES:.c=.o)

all: server client

server: $(OBJECTS) server.o
    $(CC) $(LDFLAGS) $(OBJECTS) server.o -o $@ -lm

client: $(OBJECTS) client.o
    $(CC) $(LDFLAGS) $(OBJECTS) client.o -o $@ -lm

$(SRCDIR)/%.o: $(SRCDIR)/%.c
    $(CC) $(CFLAGS) -c $< -o $@ -lm 

server.o: server.c
    $(CC) $(CFLAGS) -c $< -o $@ -lm

client.o: client.c
    $(CC) $(CFLAGS) -c $< -o $@ -lm

clean:
    rm -f server.o client.o $(OBJECTS)