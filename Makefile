CC := gcc 

INCLUDES := $(addprefix -I, $(shell find src -type d))

CPPFLAGS := $(shell pkg-config --cflags gtk4) $(INCLUDES)
CFLAGS := -Wall -Wextra -g -pthread
LDLIBS := $(shell pkg-config --libs gtk4 libsodium) -pthread
#LDFLAGS :=

# OS detecting
ifeq ($(OS),Windows_NT)
	CPPFLAGS += -DWIN32
	RM := del /f /q
	RMDIR := rd /s /q
	MKDIR := mkdir -p
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

HDRS := $(shell find src -name "*.h")
SRCS := $(shell find src -name "*.c")

OBJS := $(patsubst src/%.c, build/%.o, $(SRCS))

TARGET := netpulse-noc$(EXE_EXT) # ex.: netpulse-noc.exe (Windows) or netpulse-noc (Unix)

.PHONY: all clean 

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDLIBS)

build/%.o: src/%.c
	@$(MKDIR) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(TARGET)
	$(RMDIR) build
