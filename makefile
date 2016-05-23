CC = gcc

TARGET = testing

CFLAGS = -g -Wall

INCLUDE = -I.

$(TARGET): test.c xmlparser.c
	$(CC) $(CFLAGS) $(INCLUDE) -o $(TARGET) test.c xmlparser.c

clean:
	rm *.o
