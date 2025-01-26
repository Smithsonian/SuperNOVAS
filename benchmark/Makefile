CC ?= gcc
CFLAGS ?= -O2 -Wall -I../include

BENCHMARKS = benchmark-nutation benchmark-place

.PHONY: run
run: all
	@for prog in $(BENCHMARKS) ; do ./$${prog} ; done

.PHONY: all
all: $(BENCHMARKS)

benchmark-%: benchmark-%.c
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -L../lib -lsupernovas -lm

.PHONY: clean
clean:
	rm -f *.o

.PHONY: distclean
distclean: clean
	rm -f $(BENCHMARKS)