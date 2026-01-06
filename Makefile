CC = gcc
CFLAGS = -Wall -O2
TARGET = ctactoe

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c

install:
	install -D -m 0755 $(TARGET) $(DESTDIR)/usr/bin/$(TARGET)

clean:
	rm -f $(TARGET)
