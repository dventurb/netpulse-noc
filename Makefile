CC := gcc 

CPPFLAGS := $(shell pkg-config --cflags gtk4) -Iinc/ -Iui/inc 
CFLAGS := -Wall -Wextra -g -pthread
LDLIBS := $(shell pkg-config --libs gtk4) -pthread
# LDFLAGS := 

# OS detecting
ifeq ($(OS),Windows_NT)
	CPPFLAGS += -DWIN32
	RM := del /f /q
	RMDIR := rd /s /q
	MKDIR := mkdir 
	CP := copy /y
	EXE_EXT := .exe
else 
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CPPFLAGS += -DLINUX
	endif
	ifeq ($(UNAME_S),Darwin)
		CPPFLAGS += -DOSX
	endif 
	RM := rm -f
	RMDIR := rm -rf
	MKDIR := mkdir -p
	CP := cp
	EXE_EXT := 
endif

HDRS := $(wildcard inc/*.h) $(wildcard ui/inc/*.h)
SRCS := $(wildcard *.c) $(wildcard src/*.c) $(wildcard ui/src/*.c)
OBJS := $(SRCS:%.c=build/%.o)

TARGET := netpulse-noc$(EXE_EXT) # ex.: netpulse-noc.exe (Windows) or netpulse-noc (Unix)

.PHONY: all clean 

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(LDLIBS)

build/%.o: %.c $(HDRS)
	$(MKDIR) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(TARGET)
	$(RMDIR) build
