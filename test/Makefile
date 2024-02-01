include ../Makefile

CFLAGS += -O0 -g -pg -fprofile-arcs -ftest-coverage
LDFLAGS += -pg -fprofile-arcs -ftest-coverage

SRC = ../src
LIB = ../lib/novas.a

test: src/test.c $(SOURCES) | Makefile
	$(CC) -o $@ -O0 $(CFLAGS) -DDEFAULT_SOLSYS=3 $^ $(LDFLAGS) -lm

run: clean test
	rm -rf data
	mkdir data
	./test

# We are dependent on the project Makefile...
Makefile: ../Makefile

# Remove intermediate files
.PHONY: test-clean
test-clean:
	rm -rf obj data gmon.out *.gcov *.gcda *.gcno

clean: test-clean