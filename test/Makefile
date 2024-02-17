include ../config.mk

CFLAGS = -I../include -O0 -g -pg -fprofile-arcs -ftest-coverage
LDFLAGS += -fprofile-arcs -ftest-coverage -lm

CFLAGS += -DDEFAULT_SOLSYS=3 -DDEFAULT_CIO_LOCATOR_FILE=\"cio_ra.bin\"

OBJECTS := $(subst obj/,,$(OBJECTS))

.PHONY: run
run: clean-cov clean-data test test_cio_file data ../cio_ra.bin
	./test
	ln -s ../cio_ra.bin
	./test_cio_file
	diff data reference

.PHONY: coverage
coverage: novas.c.gcov nutation.c.gcov solsys3.c.gcov
	make clean-test

data:
	mkdir data

.PRECIOUS: %.c.gcov
%.c.gcov: %.gcno %.gcda
	gcov $*

.PRECIOUS: %.gcno %.gcda
%.gcno %.gcda: ../src/%.c
	make clean
	make run

../cio_ra.bin:
	make -C .. cio_ra.bin

test: test.o $(OBJECTS) | Makefile data
	$(CC) -o $@ $^ $(LDFLAGS)

test_cio_file: test_cio_file.o $(OBJECTS) | Makefile data
	$(CC) -o $@ $^ $(LDFLAGS)

test%.o: src/test%.c | Makefile
	$(CC) -c -o $@ -I../include $<

%.o: ../src/%.c | Makefile
	$(CC) -c -o $@ $(CFLAGS) $<

.PHONY: clean-test
clean-test:
	rm -f test test_cio_file

.PHONY: clean-cov
clean-cov:
	rm -rf gmon.out *.gcov *.gcda *.gcno

.PHONY: clean-data
clean-data:
	rm -rf data cio_ra.bin

.PHONY: clean
clean: clean-test clean-cov clean-data
	rm -f *.o

vpath %.c ../src
vpath %.c src

