CC = gcc
CFLAGS = -Wall
LDFLAGS = -lm

TARGET = graphics

SRCS = graphics.c helpers.c camera.c matrixOperations.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)

run: all
	./$(TARGET)