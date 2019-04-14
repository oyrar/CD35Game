SHELL = /bin/sh

all:

.SUFFIXES:
.SUFFIXES: .cpp .o

TARGET=play.exe

CC=g++
CXX=$CC
LD=g++
CPPFLAGS=--std=c++17 -MMD -MP
LDFLAGS=

main.o: main.cpp
	$(CC) $(CPPFLAGS) -c main.cpp -o main.o
OBJS+=main.o

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

all: $(TARGET)

clean:
	-\rm -f $(OBJS)
	-\rm -f $(TARGET)

-include $(patsubst %.o,%.d,$(OBJS))

