CIO_LOCATOR_FILE := cio_ra.bin

CFLAGS = -I../include -O0 -g -pg -fprofile-arcs -ftest-coverage
LDFLAGS += -fprofile-arcs -ftest-coverage -lm

include ../config.mk


#OBJECTS := $(subst obj/,,$(OBJECTS))
OBJECTS := novas.o nutation.o solsys3.o

.PHONY: run
run: clean-cov clean-data test-compat test-cio_file test-super test-errors data ../cio_ra.bin
	./test-compat
	ln -s ../cio_ra.bin
	./test-cio_file
	diff data reference
	./test-super
	rm -f cio_ra.bin
	./test-errors

cov: cov.info
	genhtml $< -o $@

cov.info:
	geninfo . -b . -o $@

.PHONY: coverage
coverage: novas.c.gcov nutation.c.gcov solsys3.c.gcov
	make clean-test
	make cov

data:
	mkdir data

.PRECIOUS: %.c.gcov
%.c.gcov: %.gcno %.gcda
	gcov -b $*

.PRECIOUS: %.gcno %.gcda
%.gcno %.gcda: ../src/%.c
	make clean
	make run

../cio_ra.bin:
	make -C .. cio_ra.bin

test-%: test-%.o $(OBJECTS) | Makefile data
	$(CC) -o $@ $^ $(LDFLAGS)

test-%.o: src/test-%.c | Makefile
	$(CC) -c -o $@ -g -I../include $<

%.o: ../src/%.c | Makefile
	$(CC) -c -o $@ $(CFLAGS) $<

.PHONY: clean-test
clean-test:
	rm -f test-*

.PHONY: clean-cov
clean-cov:
	rm -rf gmon.out *.gcov *.gcda *.gcno cov cov.info

.PHONY: clean-data
clean-data:
	rm -rf data cio_ra.bin

.PHONY: clean
clean: clean-test clean-cov clean-data
	rm -f *.o

vpath %.c ../src
vpath %.c src

