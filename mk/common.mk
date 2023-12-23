UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    PRINTF = printf
else
    PRINTF = env printf
endif

# Control the build verbosity
ifeq ("$(VERBOSE)","1")
    Q :=
    VECHO = @true
    REDIR =
else
    Q := @
    VECHO = @$(PRINTF)
    REDIR = >/dev/null
endif

# Test suite
PASS_COLOR = \e[32;01m
NO_COLOR = \e[0m
GREEN_COLOR = \e[32;01m
YELLOW_COLOR = \e[33;01m
BLUE_COLOR = \e[34;01m
CYAN_COLOR = \e[36;01m
BLINK = \e[5m

pass = $(PRINTF) "$(PASS_COLOR)$1 Compile successfully !$(NO_COLOR)\n"