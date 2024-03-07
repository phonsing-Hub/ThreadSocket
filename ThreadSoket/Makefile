# TARGET = $(notdir $(shell pwd))

# SOURCES = $(wildcard *.cpp)
# OBJECTS = $(SOURCES:.cpp=.o)

# CC = g++
# CFLAGS = -Wall -Werror
# OPTIONS = -pthread

# all: $(OBJECTS)
# 	$(CC) $(CFLAGS) $^ -o $(TARGET) $(OPTIONS)

# %.o: %.cpp
# 	$(CC) $(CFLAGS) -c $< -o $@ $(OPTIONS)

# clean: 
# 	rm -f *.o $(TARGET)

all: threadserver.o threadclient.o
	g++ threadserver.o -o threadserver
	g++ threadclient.o -o threadclient
threadserver.o: threadserver.cpp
	g++ -c threadserver.cpp -o threadserver.o
threadclient.o: threadclient.cpp
	g++ -c threadclient.cpp -o threadclient.o
clean:
	rm -f *.o threadserver threadclient

