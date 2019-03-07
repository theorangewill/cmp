cflags= -g -Wall -std=c99 -pedantic -lm

compile = gcc

program = main

class = seismicunix

csources = $(program).c  $(class).c

cobjects = $(program).o $(class).o

$(program): $(cobjects)
	$(compile) -o $(program) $(cobjects) $(cflags) -O0

$(cobjects): $(csources)
	$(compile) $^ -c

clean:
	rm -r $(program)
	rm -r $(cobjects)
	rm -r *.h.gch
