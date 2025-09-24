# CIO vs GCRS locator data

NOVAS C (and also SuperNOVAS prior to version 1.5) has bundled CIO vs GCRS locator data (`CIO_RA.TXT`). The current
version of __SuperNOVAS__ no longer needs or uses such locator data in any way. And, due to the updated P03-R06 
precession / nutation model, the original data is also not fully consistent with the internals of __SuperNOVAS__.
As such, we no longer provide a CIO locator data file.

As a result, the bundled tool `cio_file` (from `cio_file.c`), which could be used for ocnverting `CIO_RA.TXT` into 
binary format (`cio_ra.bin`), also serves no purpose, and has been removed from the __SuperNOVAS__ distribution also.

