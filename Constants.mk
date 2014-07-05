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
CURL=CURL

#
# The path leading to the gdal binaries -- in particular,
# gdal_rasterize and gdal_translate
#
GDAL_PATH=/opt/local/bin

#
# Sometimes the C programs in src will compile but die at runtime
# due to obscure, hard to diagnose dynamic library problems,
# usually having to do with -lgmt or -lnetcdf. If that happens
# you can try setting the STATIC variable below to "-static":
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

#
# cpt files needed for plotting
#
VS30_CPT = ../Misc/usa.cpt
WEIGHTS_CPT = ../Misc/weights.cpt

#
# The velocity assigned to water-covered areas
#
WATER = 600

#
# Identify the target into which we will insert the regional maps. 
# If the target isn't there, set it to the base target "global_vs30.grd"
#
INGRD = $(wildcard ../global_vs30*.grd)
ifeq ($(strip $(INGRD)),)
        INGRD = ../global_vs30.grd
endif

#
# Add this region's extension to the base global file name
# to create the output file name
#
OUTGRD = $(basename $(notdir $(INGRD)))_$(MYEXT).grd

# End
