TARGET = vm
CC = gcc

INCLUDE = -I./includes

CFLAGS = -Wall -Wextra -Werror $(INCLUDE)

SRC_FOLDER = src/
SRCS_RAW =	main.c \
			open_file.c \
			bytecode.c \
			stack.c\
			endian.c

SRCS = $(addprefix $(SRC_FOLDER), $(SRCS_RAW))

OBJ_FOLDER = obj/
OBJS_RAW = $(SRCS_RAW:.c=.o)
OBJS = $(addprefix $(OBJ_FOLDER), $(OBJS_RAW))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_FOLDER)%.o: $(SRC_FOLDER)%.c
	@mkdir -p $(OBJ_FOLDER)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJ_FOLDER)

fclean: clean
	rm -f $(TARGET)

re: fclean all

test: $(TARGET)
	clear
	./$(TARGET) test2 -v
.PHONY: all clean fclean re