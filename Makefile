CC=gcc
CFLAGS=-c -Wall -g
LDFLAGS=-ljpeg
SOURCES= mandel.c jpegrw.c mandelmovie.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=mandel mandelmovie

all: $(SOURCES) $(EXECUTABLE) 

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d)

mandel: mandel.o jpegrw.o
	$(CC) mandel.o jpegrw.o $(LDFLAGS) -o $@

mandelmovie: mandelmovie.o
	$(CC) mandelmovie.o $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@
	$(CC) -MM $< > $*.d

clean:
	rm -rf *.o $(EXECUTABLES) *.ds

# $(EXECUTABLE): $(OBJECTS)
# 	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

# .c.o: 
# 	$(CC) $(CFLAGS) $< -o $@
# 	$(CC) -MM $< > $*.d

# clean:
# 	rm -rf $(OBJECTS) $(EXECUTABLE) *.d
