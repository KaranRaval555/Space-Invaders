CC = gcc
CFLAGS = -O3 -Wall -Wextra -pedantic
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt
TARGET = main

$(TARGET): main.c base.h
	$(CC) main.c $(CFLAGS) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)
