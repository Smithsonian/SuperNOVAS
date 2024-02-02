include ../config.mk


CFLAGS += -O0 -g -pg -fprofile-arcs -ftest-coverage
LDFLAGS += -pg -fprofile-arcs -ftest-coverage

SRC = ../src

.PHONY: run
run: clean test
	rm -rf data
	mkdir data
	./test
	rm -f test
	diff data reference

.PHONY: coverge
coverage: run
	gcov test-novas.c
	gcov test-nutation.c
	gcov test-solsys1.c
	gcov test-eph_manager.c
	gcov test-solsys3.c

test: src/test.c $(SOURCES) | Makefile
	$(CC) -o $@ -O0 $(CFLAGS) -DDEFAULT_SOLSYS=3 $^ $(LDFLAGS) -lm

# We are dependent on the project Makefile...
Makefile: ../Makefile

# Remove intermediate files
.PHONY: clean-data
clean-data:
	rm -rf test obj data gmon.out *.gcov *.gcda *.gcno

clean: clean-data

include ../build.mk