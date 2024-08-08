CC := cc
CC_CMD := $(CC) -Wall -Wextra

LD := cc
LD_CMD := $(LD)

.PHONY: all clean

all: bin/tcp-mirror

bin/tcp-mirror: bin/main.o
	$(LD_CMD) bin/main.o -o bin/tcp-mirror

bin/main.o: bin/.dirstamp main.c
	$(CC_CMD) -c main.c -o bin/main.o

bin/.dirstamp:
	mkdir -p bin
	touch bin/.dirstamp

clean:
	git clean -fdx -e "*.swp"
