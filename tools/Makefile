include ../config.mk

# ASCII interpolation table for CIO locations
CIO_IN = data/CIO_RA.TXT

# Binary output equivalent to the above
CIO_OUT = data/cio_ra.bin

.PHONY: cio_file
cio_file: $(CIO_OUT)

# Create CIO locator binary file
$(CIO_OUT): bin/cio_file $(CIO_IN)
	$^ $(CIO_OUT)
	rm -f $<

# Build tool for creating CIO locator binary file
bin/cio_file: src/cio_file.c ../lib/novas.a | bin Makefile
	$(CC) -o $@ $^ $(LFLAGS)

# Remove intermediate files
.PHONY: clean-data
clean-data:
	rm -rf $(CIO_OUT)

clean-local: clean-data

include ../build.mk