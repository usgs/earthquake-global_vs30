#
# Project-wide constants for Makefiles
#

#
# Set these to the directories where libnetcdf, the CDF include file,
# libgmt, and the GMT include files reside, respectively.
#
CDFLIB  = /usr/local/lib
CDFINC  = /usr/local/include
GMTLIB  = /usr/local/lib
GMTINC  = /usr/local/include

#
# CURL (or curl) utility. Many systems have this by default, others
# can easily install it with a package manager, or it can be had 
# from http://curl.haxx.se.
#
CURL=curl

#
# The path leading to the gdal binaries -- in particular,
# gdal_rasterize and gdal_translate
#
GDAL_PATH=/opt/local/bin

#
# Sometimes the C programs in src will compile but die at runtime
# due to obscure, hard to diagnose dynamic library problems,
# usually having to do with -lgmt or -lnetcdf. If that happens
# you can try setting the STATIC macro below to "-static":
#
# STATIC = -static
#
STATIC =

#
# The region for which we have a DEM (GMTED2010); this defines
# the extent of the global map.
#
GXMIN = -180
GXMAX = 180
GYMIN = -56
GYMAX = 84
GLOBAL_REGION=$(GXMIN)/$(GXMAX)/$(GYMIN)/$(GYMAX)

#
# A region of the map covering the western US
#
WUS_REGION = -130.0/-113.0/31.0/50.0

#
# A region of the map covering the western US including Utah
#
BIG_WUS_REGION = -130.0/-107.0/31.0/50.0

#
# The gridfile resolution in arc-seconds (RES) and decimal degrees (RES_DD);
# both of these must represent the same resolution; the GMTED2010 is available 
# in 30, 15, and 7.5 arc second resolutions (0.008333333333, 0.004166666667, 
# 0.002083333333 degrees), so those are good choices; other resolutions will
# require makefile modifications.
#
RES = 30
RES_DD = 0.00833333333333
#RES = 15
#RES_DD = 0.004166666667
#RES = 7.5
#RES_DD = 0.002083333333

#
# IRES is an integer version of RES. Normally it will be whatever
# RES is, but if RES is 7.5 we'll set it to 7, below, so that the
# -gt/-lt style tests in other Makfiles will work
#
IRES = $(RES)

#
# Depending on the resolution, we want to set the width of the
# smoothing filter (in grid points). Our defaults are to use
# smoothing filters for the regions of 0.7x0.7 degrees (but these
# are cut in half, to 0.35x0.35 degrees, by the processing), and
# for the global grid the smoothing from active tectonic to
# stable craton is over 2x2 degrees.
#
ifeq ($(RES), 7.5)
# IRES gets reset to an integer
IRES = 7
REGION_FX = 339
REGION_FY = 339
GLOBE_FX = 959
GLOBE_FY = 959
else ifeq ($(RES), 15)
REGION_FX = 169
REGION_FY = 169
GLOBE_FX = 479
GLOBE_FY = 479
else ifeq ($(RES), 30)
REGION_FX = 85
REGION_FY = 85
GLOBE_FX = 239
GLOBE_FY = 239
else
$(error Unknown grid resolution -- you must specify filter sizes.)
endif

#
# cpt files needed for plotting
#
VS30_CPT = ../Misc/usa.cpt
WEIGHTS_CPT = ../Misc/weights.cpt

#
# The velocity assigned to water-covered areas
#
WATER = 600

# Don't mess with this definition -- the blank lines are necessary
define N


endef

# End
