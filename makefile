CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lpthread

SRC = wish.c dupfinder.c netstatus.c remindme.c todocli.c wcleanup.c
OBJ = $(SRC:.c=.o)

TARGET = wish

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean

