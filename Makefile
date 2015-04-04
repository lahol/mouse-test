CC := $(CROSS)gcc
LD := $(CROSS)ld
AR := $(CROSS)ar
PKG_CONFIG := $(CROSS)pkg-config


MT_CFLAGS += -Wall -g -I$(CROSSENV)/usr/include `$(PKG_CONFIG) --cflags glib-2.0 gtk+-2.0` 
MT_LIBS += `$(PKG_CONFIG) --libs glib-2.0 gtk+-2.0`
MT_LDFLAGS += -L$(CROSSENV)/usr/lib


mt_SRC := $(wildcard *.c)
mt_OBJ := $(mt_SRC:.c=.o)
mt_HEAERS := $(wildcard *.h)

ifdef TARGET_WINDOWS
	TARGET := mouse-test.exe
	MT_CFLAGS += -mms-bitfields
	MT_LDFLAGS += -Wl,-subsystem,windows
else
	TARGET := mouse-test
endif

all: $(TARGET)

$(TARGET): $(mt_OBJ)
	$(CC) $(LDFLAGS) $(MT_LDFLAGS) -o $@ $^ $(LIBS) $(MT_LIBS)

%.o: %.c $(mt_HEADERS)
	$(CC) $(CFLAGS) $(MT_CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(mt_OBJ)

.PHONY: all clean
