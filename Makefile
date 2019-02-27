CFLAGS  = -std=c99
CFLAGS += -g
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -pedantic
CFLAGS += -Werror

VFLAGS  = --quiet
VFLAGS += --tool=memcheck
VFLAGS += --leak-check=full
VFLAGS += --error-exitcode=1

test: tests.out
	@./tests.out

memcheck: tests.out
	@valgrind $(VFLAGS) ./tests.out
	@echo "Memory check passed"

clean:
	rm -rf *.o *.out *.out.dSYM

tests.out: src/test_main.c src/dyn_array.c src/dyn_array.h src/bst.c src/bst.h
	@$(CC) $(CFLAGS) src/test_main.c src/dyn_array.c src/bst.c -lm -o tests.out
