# Adjusted configuration for test builds
BUILTIN_SOLSYS3 := 0 
BUILTIN_DEFAULT_SOLSYS := 0 
CIO_LOCATOR_FILE := cio_ra.bin

# Use above config in sub-makes also
export BUILTIN_SOLSYS3 BUILTIN_DEFAULT_SOLSYS CIO_LOCATOR_FILE

include ../config.mk

CFLAGS += -I../include -pg -Wall -fprofile-arcs -ftest-coverage
LDFLAGS += -fprofile-arcs -ftest-coverage -lm

# cppcheck options for 'check' target. You can add additional options by
# setting the CHECKEXTRA variable (e.g. in shell) prior to invoking 'make'.
CHECKOPTS = --enable=performance,warning,portability --language=c \
            --error-exitcode=1 --std=c99 --inline-suppr $(CHECKEXTRA)

OBJECTS := $(subst $(OBJ)/,,$(OBJECTS))
COVFILES := $(subst .o,.c.gcov,$(OBJECTS))

TESTS := test-compat test-cio_file test-cio-data test-super test-errors

ifeq ($(CALCEPH_SUPPORT), 1)
  LDFLAGS += -lcalceph
  OBJECTS += solsys-calceph.o
  TESTS += test-calceph
  COVFILES += solsys-calceph.c.gcov
endif

ifeq ($(CSPICE_SUPPORT), 1)
  LDFLAGS += -lcspice
  OBJECTS += solsys-cspice.o
  TESTS += test-cspice
  COVFILES += solsys-cspice.c.gcov
endif

.PHONY: check-compat
check-compat:
	@sed -i.tmp "s:-nan: nan:g" data/* && rm -f data/*.tmp
	diff data reference

.PHONY: check-cio_file
check-cio_file: 
	@ln -s ../data/cio_ra.bin
	./test-cio_file
	@rm -f cio_ra.bin

.PHONY: run
run: clean-cov clean-data $(TESTS) data bad-data
	./test-compat
	$(MAKE) check-cio_file
	$(MAKE) check-compat
	./test-cio-data
	./test-super ../data
	./test-errors
ifeq ($(CALCEPH_SUPPORT), 1)
	./test-calceph ephem
endif
ifeq ($(CSPICE_SUPPORT), 1)
	./test-cspice ephem
endif

cov: cov.info
	genhtml $< -o $@

cov.info:
	geninfo . -b . -o $@

.PHONY: coverage

coverage: $(COVFILES)
	$(MAKE) clean-test
	$(MAKE) cov

data:
	mkdir data

.PHONY: bad-data
bad-data: ../data/cio_ra.bin bad-cio-data
	@touch bad-cio-data/empty
	@dd status=none if=../data/cio_ra.bin of=bad-cio-data/bad-1.bin bs=27 count=1
	@dd status=none if=../data/cio_ra.bin of=bad-cio-data/bad-2.bin bs=63 count=1
	@dd status=none if=../data/CIO_RA.TXT of=bad-cio-data/bad-1.txt bs=14 count=1
	@dd status=none if=../data/CIO_RA.TXT of=bad-cio-data/bad-2.txt bs=79 count=1
	@dd status=none if=../data/CIO_RA.TXT of=bad-cio-data/bad-3.txt bs=21 count=1
	@dd status=none if=../data/CIO_RA.TXT of=bad-cio-data/bad-4.txt bs=21 count=1
	@echo "blah" >> bad-cio-data/bad-4.txt
	@dd status=none if=../data/CIO_RA.TXT of=bad-cio-data/bad-5.txt bs=79 count=1
	@echo "blah blah blah blah blah blah" >> bad-cio-data/bad-5.txt

bad-cio-data:
	mkdir $@

.PRECIOUS: %.c.gcov
%.c.gcov: %.gcno %.gcda
	gcov -b $*

.PRECIOUS: %.gcno %.gcda
%.gcno %.gcda: ../src/%.c
	$(MAKE) clean
	$(MAKE) run

test-%: test-%.o $(OBJECTS) | Makefile data
	$(CC) -o $@ $^ $(LDFLAGS)

test-%.o: src/test-%.c | Makefile
	$(CC) -c -o $@ $(CPPFLAGS) $(CFLAGS) $<

%.o: ../src/%.c | Makefile
	$(CC) -c -o $@ $(CPPFLAGS) $(CFLAGS) $<


test-calceph: LDFLAGS += -lcalceph

test-cspice: LDFLAGS += -lcspice

# Static code analysis using 'cppcheck'
.PHONY: analyze
analyze:
	@echo "   [analyze]"
	@cppcheck $(CPPFLAGS) $(CHECKOPTS) $(SRC)

.PHONY: clean-test
clean-test:
	rm -f test-*

.PHONY: clean-cov
clean-cov:
	rm -rf gmon.out *.gcov *.gcda *.gcno cov cov.info

.PHONY: clean-data
clean-data:
	rm -rf data cio_ra.bin bad-cio-data

.PHONY: clean
clean: clean-test clean-cov clean-data
	rm -f *.o

.PHONY: help
help:
	@echo
	@echo "Syntax: make [target]"
	@echo
	@echo "The following targets are available:"
	@echo
	@echo "  run           (default) Compiles and runs regression tests."
	@echo "  coverage      Extracts test coverage data."
	@echo "  analyze       Static analysis with cppcheck."
	@echo "  clean         Removes intermediate products."
	@echo "  distclean     Deletes all generated files."
	@echo


vpath %.c ../src
vpath %.c src

