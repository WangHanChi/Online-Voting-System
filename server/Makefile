# import common.mk
include ../mk/common.mk

TARGET = server
OUT = build

# C source files
C_SOURCE = 			\
main-server.c		\
handler.c			\
login.c				\
syslog.c

VPATH = $(dir $(C_SOURCES))

# name the object files
OBJS = $(patsubst %.c, $(OUT)/%.o, $(notdir $(C_SOURCE)))

VPATH = src
vpath %.c

# toolchain options
CFLAGS := -Wall -Werror
LDFLAGS := -lm

ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG -O0
else ifeq ($(RELEASE), 1)
	CFLAGS += -O2
endif


.PHONY: upload clean help server

server: clean $(OUT)/$(TARGET)

# compile
$(OUT)/%.o: %.c
	$(VECHO) "  [CC]\t$@\n"
	$(Q)$(CC) -c $(CFLAGS) $< -o $@

# link
$(OUT)/$(TARGET): $(OUT) $(OBJS)
	$(VECHO) "  [LD]\t$@\n"
	$(Q)$(CC) $(OBJS) $(LDFLAGS) -o $@
	$(Q)echo "===================================================="
	$(Q)$(call pass)

# mkdir dir
$(OUT):
	-mkdir $@

format:
	clang-format -i $(C_SOURCE) *.h

clean:
	$(RM) $(OUT)/*.o build/$(TARGET)
