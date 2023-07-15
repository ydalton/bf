CFLAGS := -Wall -Wextra -Wpedantic

objs := bf.o

all: $(objs)
	cc -o bf bf.o
clean: 
	rm -f bf $(objs)

.PHONY: all clean
