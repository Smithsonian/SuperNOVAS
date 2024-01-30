

# Compiler options
CFLAGS = -I../include -Os -Wall -Wextra -g

# Linker options
LDFLAGS = -lm

LIB = ../lib/novas.a

.PHONY: all
all: bin/combotest bin/checkout-mp bin/checkout-stars bin/checkout-stars-full

$(LIB):
	$(MAKE) -C .. static

# Regular object files
obj/%.o: src/%.c obj
	$(CC) -o $@ -c $(CFLAGS) $<

bin/%: obj/%.o $(LIB) | bin
	$(CC) -o $@ $^ $(LDFLAGS)

# Sub-directories for build targets
obj bin:
	mkdir $@

# Remove intermediate files
.PHONY: clean
clean:
	rm -rf obj

# Remove all locally built files, effectively restoring the repo to its pristine state
.PHONY: distclean
distclean: clean
	rm -rf bin
