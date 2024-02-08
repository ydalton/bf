CFLAGS	:=	-g -Wall -Wextra -Wpedantic -Werror=implicit-function-declaration -Wno-unused-function
TARGET	:=	bf
OBJ	:=	bf.o

$(TARGET): $(OBJ)
	gcc -o $@ $<

clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: $(TARGET) clean
