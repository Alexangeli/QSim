CC = gcc
CFLAGS = -Wall -Wextra -pthread -Iinclude
LDFLAGS = -lm -pthread

SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)

TARGET = qsim

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)