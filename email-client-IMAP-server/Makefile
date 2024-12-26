EXE=fetchmail
SRCS=fetchmail.c net_utils.c imap_utils.c email_parser.c mime_handler.c email_list.c retrieve_handler.c
OBJS=$(SRCS:.c=.o)

# Compiler and compiler flags
CC=cc
CFLAGS=-Wall

# Rule for building the final executable
$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Generic rule for compiling each object file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Format all C files in the directory
format:
	clang-format -style=file -i *.c

# Clean the build
clean:
	rm -f $(EXE) $(OBJS)

.PHONY: format clean
