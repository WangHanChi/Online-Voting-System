.PHONY: clean help build

OUT = build
DEBUG ?= 0
RELEASE ?= 0

help:
	@echo ""
	@echo "+------------------- command manual ------------------------+"
	@echo "|   command   |                description                  |"
	@echo "|-------------+---------------------------------------------|"
	@echo "| help        | show command manual                         |"
	@echo "| all         | compile, link and assemble the FTP files    |"
	@echo "| clean       | remove the intermediate objects and image   |"
	@echo "| format      | use clang format to make code tidy          |"
	@echo "+-----------------------------------------------------------+"
	@echo ""

all: clean $(OUT)
	make -C client/ DEBUG=$(DEBUG) RELEASE=$(RELEASE)
	make -C server/ DEBUG=$(DEBUG) RELEASE=$(RELEASE)
	cp client/build/client $(OUT)/client
	cp server/build/server $(OUT)/server

# mkdir dir
$(OUT):
	mkdir -p $@

clean:
	rm -rf $(OUT)/* client/build/* server/build/* syslog.txt

test: all
	./script/test.sh

format:
	make format -C client/
	make format -C server/
