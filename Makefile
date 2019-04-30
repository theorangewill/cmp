CC = gcc
SRCEXT := c

BUILDDIR := obj
SRCDIR := src
BINDIR := bin
TARGET := bin/cmp

SOURCES := $(wildcard $(SRCDIR)/*.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -O3 -Wall -std=c99 -pedantic -lm -fopenmp 
LIB := -L lib
INC := -I include


$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $^ -o $(TARGET) $(CFLAGS) $(INC)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo Cleaning...
	rm -rf bin/
	rm -rf obj/

.PHONY: clean
