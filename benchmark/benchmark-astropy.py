# astropy benchmark for position calculations, matching the SuperNOVAS 
# benchmarks in `benchmark-place.c`
#
# It calculates CIRS positions for an Earth based observer for a number of 
# random sidereal sources. The first benchmark calculates positions for a 
# fixed observing frame (time and observer location), while the second test 
# calculates positions for individual observing frames.
#
# Author: Attila Kovacs
# Date: 2025-01-25
# ----------------------------------------------------------------------------

import astropy
import random
import time
import astropy.units as u
from astropy.coordinates import SkyCoord, EarthLocation, Longitude, Latitude, CIRS

# ----------------------------------------------------------------------------
# main program entry point

# Number of sources / iterations to test with
N = 300			

# ----------------------------------------------------------------------------
# Set up a source 'catalog' with the desired number of sources and random 
# properties
sources = []
	
for i in range(0, N):
	# Generate random source properties
	ra = Longitude(360.0 * (random.random() - 0.5) * u.degree)
	dec = Latitude(180.0 * (random.random() - 0.5) * u.degree)
	d = (1.0 + 1000.0 * random.random()) * u.pc
	pmra = 100.0 * (random.random() - 0.5) * u.mas / u.yr
	pmdec = 100.0 * (random.random() - 0.5) * u.mas / u.yr
	rv = 1000.0 * (random.random() - 0.5) * u.km / u.s

	sc = SkyCoord(ra, dec, d, pm_ra_cosdec=pmra, pm_dec=pmdec, radial_velocity=rv)
	sources.append(sc)


# ----------------------------------------------------------------------------
# Benchmark 1: calculating positions in the same frame


# Set up a fixed observing frame...
tim = astropy.time.Time("2025-01-25T15:32:00")
loc = EarthLocation.from_geodetic(lon=Longitude(6.16 * u.degree), lat=Latitude(42.7 * u.degree), height=2500.0)
frame = CIRS(obstime=tim, location=loc)

start = time.time()
for source in sources :
	source.transform_to(frame)
end = time.time()
print("same frame          " + str(N / (end - start)))


# ----------------------------------------------------------------------------
# Benchmark 2: calculating positions in individual frames

start = time.time()
for source in sources :
	tim = astropy.time.Time("2025-01-25T15:32:00") - 365.0 * random.random() * u.day
	lon = Longitude(360.0 * random.random() * u.degree)
	lat = Latitude(180.0 * (random.random() - 0.5) * u.degree)
	loc = EarthLocation.from_geodetic(lon, lat, height=2500.0)
	frame = CIRS(obstime=tim, location=loc)
	source.transform_to(frame)
end = time.time()

print("individual frame    " + str(N / (end - start)))

