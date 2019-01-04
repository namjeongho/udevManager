CC=g++
CFLAGS=
LDFLAGS=
LDLIBS=-ludev
OBJS=main.o udevManager.o
TARGET=app.out
 
all: $(TARGET)
 
clean:
	rm -f *.o
	rm -f $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDLIBS)

main.o: main.cpp

udevManager.o: udevManager.cpp