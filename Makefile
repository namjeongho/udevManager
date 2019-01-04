CC=g++
CFLAGS=
LDFLAGS=
LDLIBS=-ludev
OBJS=main.o udev.o
TARGET=app.out
 
all: $(TARGET)
 
clean:
	rm -f *.o
	rm -f $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDLIBS)

main.o: main.cpp

udev.o: udev.cpp