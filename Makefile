CC := $(CROSS)gcc
LD := $(CROSS)ld
AR := $(CROSS)ar
PKG_CONFIG := $(CROSS)pkg-config


CFLAGS = -Wall -g -I$(CROSSENV)/usr/include `$(PKG_CONFIG) --cflags glib-2.0 gtk+-2.0` 
LIBS = `$(PKG_CONFIG) --libs glib-2.0 gtk+-2.0`
LDFLAGS = -L$(CROSSENV)/usr/lib


mt_SRC := $(wildcard *.c)
mt_OBJ := $(mt_SRC:.c=.o)
mt_HEAERS := $(wildcard *.h)

ifdef TARGET_WINDOWS
	TARGET := mouse-test.exe
	CFLAGS += -mms-bitfields
	LDFLAGS += -Wl,-subsystem,windows
else
	TARGET := mouse-test
endif

all: $(TARGET)

$(TARGET): $(mt_OBJ)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c $(mt_HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(mt_OBJ)

.PHONY: all clean
